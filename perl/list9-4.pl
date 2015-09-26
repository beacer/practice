#!/usr/bin/perl -w
# Bold-italic headline printer

use strict;
use HTML::TreeBuilder;

my $root = HTML::TreeBuilder->new_from_content(<<"EOHTML");
<b><i>Shatner wins Award!</i></b>
Today in <b>Hollywood</b> ...
<b><i>End of World Predicted!</i><b>
Today in <b>Washington</b> ...
EOHTML
$root->eof();

my @bolds = $root->find_by_tag_name('b');

foreach my $node (@bolds) {
    my @kids = $node->content_list();
    if (@kids and ref $kids[0] and $kids[0]->tag() eq 'i') {
        print $kids[0]->as_text(), "\n";
    }
}
