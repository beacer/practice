#!/usr/bin/perl -w
# rfc2html: transform RFCs from plain text to html
# Lei Chen
# 2010.11

use strict;
use Getopt::Std;
use constant USAGE_MSG => <<END;
Usage: rfc2html [OPTION ...] [FILE ...]
Options:
    -h          show this help message.
    -l          trans url and email address to link.
    -v          print verbose messages.
    -o SUFFIX   set output file's suffix.
END

# process options
my %options;
getopts('hlvo:', \%options) or die USAGE_MSG;
die USAGE_MSG if $options{'h'};

my $LINK    = $options{'l'} || undef;
my $VERBOSE = $options{'v'} || undef;
my $SUFFIX  = $options{'o'} || "html";

my $hostname_regex = qr/[-a-z0-9]+(?:\.[-a-z0-9]+)*\.(?:[a-z]{2,4})/i;

# main process here
undef $/; # enable slurp mode
while (defined(my $text = <>)) {
	warn "Processing $ARGV ...\n" if $VERBOSE;
	escape_unsafe(\$text);
	strip_carriage_return(\$text);

	if ($LINK) {
		email_to_link(\$text);
		url_to_link(\$text);
		go_top(\$text);
        $text = table_of_contents(\$text);
	}

    trim_page_separator(\$text);
    as_title(\$text);
    do_format(\$text);

	my $output = "$ARGV.$SUFFIX";
	open FH, ">", $output or die "Can't open file: $output\n";
	warn "Writing to $output ...\n" if $VERBOSE;
	print FH $text;
	close FH;
	warn "Process $ARGV over !\n" if $VERBOSE;
}

exit 0;

# belows are subroutines ...

sub go_top {
    my $text = $_[0];
    $$text =~ s{^}{<span id="top"></span>};
    $$text =~ s{^(RFC\s+\d+)}{<a href="#top">$1</a>}img;
}

sub do_format {
	my $text = $_[0];
	my $prefix = <<END;
<html>
<head>
    <style type="text/css">
        .markup_draft {
            line-height: 1.2em; margin: 0;
            font-family: "courier new"; font-size: 80%;
        }
        .title {
            font-family: arial;
            font-weight: bold;
        }
        .m_hdr, .m_ftr { color: #808080; }
        .m_ftr { border-bottom: 1px solid #a0a0a0; }
    </style>
</head>
<body>
<pre>
<span class="markup_draft">
END

	my $suffix = <<END;
</span>
</pre>
</body>
END

    $$text =~ s/\n\s*\n(\s*\n)*/\n\n/g;

	$$text = $prefix . $$text . $suffix;
}

# some strings is alway be title, so highligh them
sub as_title {
	my $text = $_[0];

    # skip lines befor "Status of this Memo"
    $$text =~ m/^Status of this Memo/img;
    substr($$text, index($$text, "Status of this Memo")) =~ s{
        ^ (?! \s* <[^>]+> ) ( \S .* ) $
    }{<span class="title">$1</span>}xgm;
}

# process table of contents (index)
# make links in table to thire contents
my %titles;
sub table_of_contents {
	my $text = $_[0];
	my $output;
	my $in_table = 0;

	foreach (split /\n/, $$text) {
		chomp;
		$in_table = 1 if m/^\s*Table of Contents\s*$/i;
		$in_table = 0 if m/^1\s+\S+/;

		if ($in_table) {
			$titles{$2}++ if s{
                (^[\s+*o]*)(\d[\d.]*)
			}{$1<a href="#$2">$2</a>}x;
		} else {
			if (m/^(\d[\d.]*)/x and $titles{$1}) {
                my $content = $1;
                #s{($content)}{<span class="title" id="$1">$1</span>}x;

				s{^}{<span class="title" id="$content">}x;
				s{$}{</span>}x;
			}
		}

		$output .= "$_\n";
	}

	$output;
}

# strip CR
sub strip_carriage_return {
	my $text = $_[0];
	$$text =~ s/\x0D//g;
}

# trim page separators and the line before/after it
sub trim_page_separator {
	my $text = $_[0];
	$$text =~ s{
		^
        ( .* \n )
         \s* \x0C \s* \n
		( .* ) \n?
        $
	}{<span class="m_ftr">$1</span><span class="m_hdr">$2</span>}xgm;
}

# escape unsafe characters linke "&", "<" and ">"
sub escape_unsafe {
	my $text = $_[0];
	$$text =~ s/&/&amp;/g;
	$$text =~ s/</&lt;/g;
	$$text =~ s/>/&gt;/g;
}

# change mail address
sub email_to_link {
    my $text = $_[0];
	$$text =~ s{
		\b
		(
			\w[-.\w]*
			\@
			$hostname_regex
		)
		\b
	}{<a href="mailto:$1">$1</a>}gix;
}

# change URLs to link
sub url_to_link {
    my $text = $_[0];
	$$text =~ s{
		\b
		(
			[a-z]+:// $hostname_regex \b
            (?:
                : (?: \d+ )?
            )?
			(?:
				/ [-a-z0-9_:\@&?=+,.!/~*'%\$]*
				(?<![.,?!])
			)?
		)
	}{<a href="$1">$1</a>}gix;
}
