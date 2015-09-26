#!/usr/bin/perl -w
# booksearch.pl: book search with DangDang's Advance Search Page.
# Lei Chen
# 2010.10

use strict;
use LWP;
use URI;
use HTML::TreeBuilder;
use Encode;

my $url_str = 'http://book.dangdang.com';
my $url     = URI->new($url_str)->canonical()
    or die "Invalid URL: $url_str\n";
my $browser = LWP::UserAgent->new() or die "Fail to create a browser\n";

# get html page
print "fetching $url ... ";
my $resp = $browser->get($url);
die $resp->status_line . "\n" unless $resp->is_success;
print "OK!\n";

# found out advanced search link
my $adv_url;
# can't use '高级搜索' as $link_name directlly, 
# because the page's char encode is not utf-8.
my $link_name = "高级搜索";
$link_name = encode("GB2312", decode("utf-8", $link_name));

# can't use while ($resp->content =~ m//ig) {...}
# it will always check the SAME tag,
# because $resp->content can't a simple SCALAR 
# and can't be set pos() ??
print "finding url for advanced search ... ";
my $html = $resp->content;
LOOP:
while ($html =~ m{<a\b([^>]+)>(.*?)</a>}ig) {
    my ($guts, $link) = ($1, $2);
    next unless $link =~ m/$link_name/;

    if ($guts =~ m{
                \b href
                \s* = \s*
                (?:
                    "([^"]*)"
                    |
                    '([^']*)'
                    |
                    ([^'">\s]+)
                )
            }xi) {
        print "OK!";
        $adv_url = $+;
        last LOOP;
    }
}
print "\n";

$adv_url = URI->new_abs($adv_url, $url);
die "Fail to fould link for '$link_name'\n" unless defined($adv_url);
#print "Advanced search at $adv_url\n";

# get advanced search page
print "fetching $adv_url ... ";
$resp = $browser->get($adv_url);
die $resp->status_line . "\n" unless $resp->is_success;
print "OK!\n";

my $root = HTML::TreeBuilder->new_from_content($resp->content) 
    or die "Fail to create html tree\n";

# found out advanced search form
my $form_name   = "form_send0";
my $form_id     = "form_send0";
my $form;
foreach my $node ($root->find_by_tag_name('form')) {
    next unless $node->attr('name') eq $form_name 
        and $node->attr('id') eq $form_id;
    $form = $node;
    last;
}
defined $form or die "Fail to found form for advanced search\n";

my $method = $form->attr('method');
my $action = $form->attr('action');;
$action = URI->new_abs($action, $adv_url);
if ($method !~ m/GET/i) {
    die "Method not support: $method\n";
}

print "Form action: $action\n";

my @query;
form_get_query($form, \@query)
    or die "Fail to get query from user\n";

my $search_url = URI->new($action)->canonical()
    or die "Falto create search URL\n";
$search_url->query_form(@query);

print "Search URL: $search_url\n";

my $file = 'result.html';
$resp = $browser->get($search_url, ":content_file" => $file);
die $resp->status_line . "\n" unless $resp->is_success;

print "Congratuations!!\nSee file $file to check the result.\n";

# ask User to fill the forms and produce the query
sub form_get_query {
    my ($form, $query) = @_;

    # hard code to produce form pairs for query
    push @$query, get_text_form("书名", "key1");
    push @$query, get_text_form("著译者", "key2");
    push @$query, get_text_form("出版社", "key3");
    push @$query, get_text_form("ISBN", "key");
    push @$query, get_text_form("定价（下限）", "orgminprice");
    push @$query, get_text_form("定价（上限）", "orgmaxprice");
    push @$query, "category", "01"; # hidden, for 图书 category

    1;
}

sub get_text_form {
    my ($text, $name) = @_;

    printf("$text: ");
    chomp(my $value = <STDIN>);

    # value shouldn't be undef
    $value = "" unless defined $value;
    $value = encode("GB2312", decode("utf-8", $value));
    return ($name, $value);
}
