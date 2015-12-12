#!/usr/bin/python3

from person import Person

class Manager(Person):
    def __init__(self, name, age, pay):
        Person.__init__(self, name, age, pay, job = 'manager')
    def giveRaise(self, percent, bonus = 0.1):
        self.pay *= (1.0 + percent + bonus)

if __name__ == '__main__':
    tom = Manager('Tom Doe', age = 50, pay = 5000)
    print(tom.lastName(), tom.pay)
    tom.giveRaise(.20)
    print(tom.pay)
