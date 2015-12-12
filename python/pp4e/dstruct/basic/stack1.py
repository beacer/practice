#!/usr/bin/env  python3

stack = []
class error(Exception):
    pass

def push(obj):
    global stack
    stack = [obj] + stack

def pop():
    global stack
    if not stack:
        raise error('stack overflow')
    top, *stack = stack
    return top

def top():
    if not stack:
        raise error('stack overflow')
    return stack[0]

def empty():
    return not stack
def member(obj):
    return obj in stack
def item(offset):
    return stack[offset]
def length():
    return len(stack)
def dump():
    print('<stack: %s>' % stack)
