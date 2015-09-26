#!/usr/bin/perl -w

use strict;
use HTML::TreeBuilder;

my $root = HTML::TreeBuilder->new;

$root->parse(
    q{
        <ul>
            <li>Ice cream.</li>
            <li>Whipped cream.
            <li>Hot apple pie <br>(mmm pie)</li>
        </u>
    }
);

$root->eof;
$root->dump;
$root->delete;
