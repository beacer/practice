#!/usr/bin/python3

import os, sys
from visitor import SearchVisitor

class EditVisitor(SearchVisitor):
    editor = '/usr/bin/vim'

    def visitmatch(self, fpathname, text):
        os.system('%s %s' % (self.editor, fpathname))

if __name__ == '__main__':
    visitor = EditVisitor(sys.argv[1])
    visitor.run('.' if len(sys.argv) < 3 else sys.argv[2])
    print('Edited %d file, visited %d' % (visitor.scount, visitor.fcount))
