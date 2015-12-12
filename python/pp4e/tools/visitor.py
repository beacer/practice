#!/usr/bin/python3

import sys, os

class FileVisitor:
    """
    visit all non-dir file in @startDir (default is './')
    visit() method can be overridded for customization.
    @trace: 0 closed, 1 show dirs, 2 show dir and files
    """
    def __init__(self, context=None, trace=2):
        self.context = context # for sub-classes
        self.trace = trace
        self.dcount = 0
        self.fcount = 0

    def run(self, startDir=os.curdir, reset=True):
        if reset: self.reset()
        for (thisDir, dirsHere, filesHere) in os.walk(startDir):
            self.visitdir(thisDir)  # current dir
            for fname in filesHere: # all non-dir files
                fpath = os.path.join(thisDir, fname)
                self.visitfile(fpath)

    def reset(self):
        self.fcount = self.dcount = 0

    def visitdir(self, dirpath):
        self.dcount += 1
        if self.trace > 0: print(dirpath, '...');

    def visitfile(self, filepath):
        self.fcount += 1
        if self.trace > 1: print(self.fcount, '=>', filepath)

class SearchVisitor(FileVisitor):
    skip_exts = []
    text_exts = ['.txt', '.py', '.pyw', '.html', '.c', '.h']

    def __init__(self, searchkey, trace=2):
        FileVisitor.__init__(self, searchkey, trace) # pass searchkey to parent-class, use self.key is same
        self.scount = 0

    def reset(self):
        self.scount = 0

    def candidate(self, fname):
        ext = os.path.splitext(fname)[1] # /aaa/bbb/xxx.txt => "/aaa/bbb/xxx", ".txt"
        if self.text_exts:
            return ext in self.text_exts
        else:
            return ext not in self.skip_exts

    def visitfile(self, fname):  # override
        FileVisitor.visitfile(self, fname) # just extend
        if not self.candidate(fname):
            if self.trace > 0: print('Skipping', fname)
        else:
            text = open(fname).read()
            if self.context in text:
                self.visitmatch(fname, text)
                self.scount += 1

    def visitmatch(self, fname, text): # new method
        print('%s has %s' % (fname, self.context))

if __name__ == '__main__':
    dolist = 1
    dosearch = 2
    donext = 4

    def selftest(testmask):
        if testmask & dolist:
            visitor = FileVisitor(trace=2)
            visitor.run(sys.argv[2])
            print('Visited %d files and %d dirs' % (visitor.fcount, visitor.dcount))

        if testmask & dosearch:
            visitor = SearchVisitor(sys.argv[3], trace=0)
            visitor.run(sys.argv[2])
            print('Found in %d files, visited %d' % (visitor.scount, visitor.fcount))

    selftest(int(sys.argv[1])) # testmask , 3 for dolist | dosearch
