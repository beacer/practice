#!/usr/bin/python3

## Perl version
# $s = "123457890";
# $s =~ s/(?<=\d)(?=(?:\d{3})+$)/,/g;

import re

s = '1234567890'
result = re.sub(r'(?<=\d)(?=(?:\d{3})+$)', ',', s)
result = re.sub(r'(\d)(\d{3}+$)', '\1,\2', s)

print(result)
