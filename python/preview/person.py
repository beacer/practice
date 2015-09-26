#!/usr/bin/python3

class Person:
    def __init__(self, name, age, pay = 0, job=None):
        self.name   = name
        self.age    = age
        self.pay    = pay
        self.job    = job

    def lastName(self):
        return self.name.split()[-1]
    def giveRaise(self, percent):
        self.pay *= (1.0 + percent)
    def __str__(self):
        return '<%s => %s>' % (self.__class__.__name__, self.name)

class Manager(Person):
    def giveRaise(self, percent, bonus = 0.1):
        self.pay *= (1.0 + percent + bonus)

if __name__ == '__main__':
    bob = Person('Bob Smith', 42, 3000, 'software')
    sue = Person('Sue Jones', 45, 4000, 'hardware')
    print(bob.name, sue.pay)
    print(bob.lastName())
    sue.giveRaise(.10)
    print(sue.pay)

    tom = Manager('Tom Doe', age = 50, pay = 5000)
    print(tom.lastName(), tom.pay)
    tom.giveRaise(.20)
    print(tom.pay)
