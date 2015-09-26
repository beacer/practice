#!/usr/bin/env python3

class error(Exception): pass

class Stack:
    def __init__(self, start=[]):
        self.stack = []
        for x in start:
            self.push(x)
        self.reverse()
    def push(self, obj):
        self.stack = [obj] + self.stack
    def pop(self):
        if not self.stack:
            raise error('stack overflow')
        top, *self.stack = self.stack
        return top
    def top(self):
        if not self.stack:
            raise error('stack overflow')
        return self.stack[0]
    def empty(self):
        return not self.stack

    # override
    def __repr__(self): # repr()
        return '[Stack:%s]' % self.stack
    def __eq__(self, other): # '==', '!='
        return self.stack == other.stack
    def __len__(self):
        return len(self.stack)
    def __add__(self, other):
        return Stack(self.stack + other.stack)
    def __mul__(self, reps):
        return Stack(self.stack * reps) # instance * reps
    def __getitem__(self, offset):  # instance[i], [i:j], in, for ...
        return self.stack[offset]
    def __getattr__(self, name):  # instance.sort()/reverse()/..
        return getattr(self.stack, name)
