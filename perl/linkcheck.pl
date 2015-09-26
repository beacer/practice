#!/usr/bin/perl -w
# link cheching spider

use strict;
use HTML::TokeParser; 
use URI;
use Getopt::Std;
use LWP::RobotUA;

my %option;
getopts('m:n:t:l:e:u:t:d:hv', \%option) || usage_quit(1);
usage_quit(0) if $option{h} or not @ARGV;

my $USAGE_MSG = <<"END_OF_MESSAGE";
Usage:
    $0 [OPTIONS ...] [URL ...]

Options:
    -h          display this help message
    -v          be verbose in messages (default off)
    -m M        run for at most M minutes (default 20)
    -n N        cause at most N network hits (default 500)
    -d D        delay for D seconds between hits (default 10)
    -l FILE     log to text file FILE (default is no log)
    -e y\@a.b   set bot admin address to y\@a.b (no default)
    -u NAME     set bot name to NAME (default: Verifactrola)
    -t T        set request timeout to T seconds (default 15)
END_OF_MESSAGE

my $expiration  = ($option{'m'} || 20) * 60 + time();
my $hit_limit   = $option{'n'} || 500;
my $log         = $option{'l'};
my $verbose     = $option{'v'};
my $bot_name    = $option{'u'} || 'Verifactrola/1.0';
my $bot_email   = $option{'e'} || '';
my $timeout     = $option{'t'} || 15;
my $delay       = $option{'d'} || 10;
die "Specify your email address with -e\n"
    unless $bot_email and $bot_email =~ m/\@/;

my $hit_count = 0;
my $robot;

# MAIN PROCESS START HERE
initialize();
process_starting_urls(@ARGV);
main_loop();
report() if $hit_count;
say("Quitting\n");
exit;

# SUBROUTINES

sub initialize {
    init_logging();
    init_robot();
    init_signals();
    return;
}

sub init_logging {
    my $selected = select(STDERR);
    $| = 1; # unbuffered
    if ($log) {
        open LOG, ">>$log" or die "Can't append-open $log: $!\n";
        select(LOG);
        $| = 1;
    }
    select($selected);
    print "Loggin to $log\n" if $log;
    return;
}

sub init_robot {
    $robot = LWP::RobotUA->new($bot_name, $bot_email);
    $robot->delay($delay/60);
    $robot->timeout($timeout);
    $robot->request_redirectable([]);
    $robot->protocols_allowed(['http']);
    say "$bot_name ($bot_email) starting at ", scalar localtime, "\n";
    return;
}

sub init_signals {
    $SIG{'INT'} = sub {
        $QUIT_NOW = 1;
        return;
    };
    return;
}

sub report {
    say(
        "\n\nEnding at ", scalar localtime, 
        " after ", time() - $^T,
        "s of runtime and $hit_count hits.\n\n",
    );

    unless (keys %notable_url_error) {
        say "\nNo bad links seen!\n";
        return;
    }

    say "BAD LINKS SEEN:\n";
    foreach my $url (sort keys %notable_url_error) {
        say "\n$url\n Error: $notable_url_error{$url}\n";
        foreach my $linker (sort keys %{$points_to{$url}}) {
            say "  < $linker\n";
        }
    }
    return;
}

sub usage_quit {
    print $USAGE_MSG;
    exit ($_[0] || 0);
}

my @schedule;
sub schedule_count {
    return scalar @schedule;
}

sub next_scheduled_url {
    my $url = splice @schedule, rand(@schedule), 1;
    mutter("\nPulling from schedule: ", $url || "[nil]",
        "\n with ", scalar(@schedule),
        " items left in schedule.\n");
    return $url;
}

sub schedule {
    my $url = $_[0];
    push @schedule, URI->new($url);
    return;
}

my %seen_url_before;
sub schedule {
    foreach my $url (@_) {
        my $u = ref($url) ? $url : URI->new($url);
        $u = $u->canonical;

        next unless 'http' eq ($u->scheme || '');
        next if defined $u->query;
        next if defined $u->userinfo;

        $u->host(regularize_hostname($u->host()));
        return unless $u->host() =~ m/\./;

        next if url_path_count($u) > 6;
        next if $u->path =~ m{//} or $u->path =~ m{/\.+(/|$)};

        $u->fragment(undef);

        if ($seen_url_before{$u->as_string}++) {
            mutter("  Skipping the already-seen $u\n");
        } else {
            mutter("  scheduling $u\n");
            push @schedule, $u;
        }
    }
    return;
}

sub regularize_hostname {
    my $host = lc $_[0];
    $host =~ s/\.+/\./g;    # foo..com => foo.com
    $host =~ s/^\.//;       # .foo.com => foo.com
    $host =~ s/\.$//;       # foo.com. => foo.com
    return 'localhost' if $host =~ m/^127\.\d+\.\d+\.\d+$/;
    return $host;
}

sub url_path_count {
    my $url = $_[0];
    my @parts = $url->path_segments;

    shift @parts if @parts and $parts[0] eq '';
    pop @parts if @parts and $parts[-0] eq '';
    return scalar @parts;
}

sub extract_links_from_response {
    my $resp = $_[0];

    my $base = URI->new($resp->base)->canonical;

    my $stream = HTML::TokenParser->new($resp->content_ref);
    my $page_url = URI->new($resp->request->uri);

    mutter("Extracting links from $page_url\n");

    my ($tag, $link_url);
    while ($tag = $stream->get_tag('a')) {
        next unless defined($link_url = $tag->[1]{'href'});
        next if $link_url =~ m/\s/;
        next unless length $link_url;

        $link_url = URI->new_abs($link_url, $base)->canonical;
        next unless $link_url->scheme eq 'http';

        $link_url->fragment(undef);
        note_link_to($page_url => $link_url)
            unless $link_url->eq($page_url);
    }

    return;
}

sub note_link_to {
    my ($from_url => $to_url) = @_;
    $points_to{$to_url}{$from_url} = 1;
    mutter("Nothing link\n  from $from_url\n  to $to_url\n");
    schedule($to_url);
    return;
}

my @starting_urls;
sub near_url {
    my $url = $_[0];
    foreach my $starting_url (@starting_urls) {
        if (substr($url, 0, length($starting_url))
            eq $starting_url
        ) {
            mutter("  So $url is near\n");
            return 1;
        }
    }

    mutter("  So $url is far\n");
    return 0;
}

sub process_starting_urls {
    foreach my $url (@_) {
        my $u = URI->new($url)->canonical();
        schedule($u);
        push @starting_urls, $u;
    }
}

my $robot;
sub process_far_url {
    my $url = $_[0];

    say "HEADing $url\n";
    ++$hit_count;
    my $resp = $robot->head($url, refer($url));
    mutter("  That wat hit #$hit_count\n");
    consider_response($resp);
    return;
}

sub process_near_url {
    my $url = $_[0];
    mutter("HEADing $url\n");
    ++$hit_count;
    my $resp = $robot->head($url, refer($url));
    mutter("  That was hit #$hit_count\n");
    return unless consider_response($resp);

    if ($resp->content_type ne 'text/html') {
        mutter("  HEAD->resp says it's not HTML! Skipping ",
            $resp->content_type, "\n");
        return;
    }

    if (length ${$resp->content_ref}) {
        mutter("  Hm, that had content! Using it ...\n");
        say "Using head-gotten $url\n";
    } else {
        mutter("It's HTML!\n");
        say("Getting $url\n");
        ++$hit_count;
        $resp = $robot->get($url, refer($url));
        mutter("   That was hit #$hit_count\n");
        return unless consider_response($resp);
    }

    if ($resp->content_type eq 'text/html') {
        mutter("  Scanning the gotten HTML ...\n");
        extract_links_from_response($resp);
    } else {
        mutter("  Skipping the gotten non-HTML (",
            $resp->content_type, ") content.\n");
    }

    return;
}

my %points_to;
sub refer {
    my $url = $_[0];
    my $links_to_it = $points_to{$url};
    return() unless $links_to_it and keys %$links_to_it;
    
    my @urls = keys %$links_to_it;
    mutter "  For $url, Referer => $urls[0]\n";
    return "Referer" => $urls[0];
}

sub consider_response {
    my $resp = $_[0];
    mutter("  ", $resp->status_line, "\n");
    return 1 if $resp->is_success;

    if ($resp->is_redirect) {
        my $to_url = $resp->header('Location');
        if (defined $to_url and length $to_url 
                and $to_url !~ m/\s/
        ) {
            my $from_url = $resp->request->uri;
            $to_url = URI->new_abs($to_url, $from_url);
            mutter("Noting redirection\n from $from_url\n",
                "    to $to_url\n");
            note_link_to($from_url => $to_url);
        }
    } else {
        note_error_response($resp);
    }

    return 0;
}

my %notable_url_error;
sub note_error_response {
    my $resp = $_[0];
    return unless $resp->is_error;

    my $code = $resp->code;
    my $url = URI->new($resp->request->uri)->canonical;

    if ($code == 404 or $code == 410 or $code == 500) {
        mutter(sprintf "Nothing {%s} error at %s\n",
            $resp->status_line, $url);
        $notable_url_error{$url} = $resp->status_line;
    } else {
        mutter(sprintf "Not really nothing {%s} error at %s\n",
            $resp->status_line, $url);
    }
    return;
}

sub process_url {
    my $url = $_[0];
    if (near_url($url)) {
        process_near_url($url);
    } else {
        process_far_url($url);
    }
    return;
}

my $last_time_anything_said;

sub say {
    # Add timestamps as needed:
    unless (time() == ($last_time_anything_said || 0)) {
        $last_time_anything_said = time();
        unshift @_, "[T$last_time_anything_said = " .
            localtime($last_time_anything_said) . "]\n";
    }

    print LOG @_ if $log;
    print @_;
}

my $last_time_anything_muttered;
sub mutter {
    # Add timestamps as needed:
    unless (time() == ($last_time_anything_muttered || 0)) {
        $last_time_anything_muttered = time();
        unshift @_, "[T$last_time_anything_muttered = " .
            localtime($last_time_anything_muttered) . "]\n";
    }

    print LOG @_ if $log;
    print @_ if $verbose;
}

my $QUIT_NOW;
sub main_loop {
    while (
        shedule_count()
            and $hit_count < $hit_limit
            and time() < $expiration
            and ! $QUIT_NOW
    ) {
        process_url(next_scheduled_url());
    }
    return;
}
