#!/usr/bin/perl -w
# Modifying attributes

use strict;
use HTML::TreeBuilder;

my $html = shift or die "Usage: $0 HTML-FILE\n";
my $root = HTML::TreeBuilder->new;

$root->ignore_ignorable_whitespace(0);
$root->no_space_compacting(1);
$root->parse_file($html) or die "$!";

print "Before:\n";
$root->dump;

my @h3_center = $root->look_down('_tag', 'h3', 'align', 'center');
my @h3_red = $root->look_down('_tag', 'h3', 'color', 'red');

for my $h3c (@h3_center) {
    $h3c->attr('_tag', 'h2');
    $h3c->attr('class', 'scream');
    $h3c->attr('align', undef);
}

for my $h3r (@h3_red) {
    $h3r->attr('_tag', 'h4');
    $h3r->attr('class', 'mutter');
    $h3r->attr('color', undef);
}

print "\n\nAfter:\n";
$root->dump;

open OUT, ">", "out1.html"
    or die "Can't open file: $!";
print OUT $root->as_HTML(undef, "    ");
close OUT;

$root->delete;
