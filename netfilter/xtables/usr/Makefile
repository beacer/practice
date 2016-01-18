CFLAGS := -Wall
CFLAGS += -g -O0

CFLAGS += -I../include

CC = gcc

IPADDR	= xt_ipaddr
ECHO	= xt_ECHO

all: lib$(IPADDR).so lib$(ECHO).so

lib$(IPADDR).so: lib$(IPADDR).o
	$(CC) -shared -fPIC -o $@ $^

# xtables need _INIT for expand the _init macro, see xtables.h
lib$(IPADDR).o: lib$(IPADDR).c
	$(CC) $(CFLAGS) -D_INIT=lib$(IPADDR)_init -fPIC -c -o $@ $^

lib$(ECHO).so: lib$(ECHO).o
	$(CC) -shared -fPIC -o $@ $^

# xtables need _INIT for expand the _init macro, see xtables.h
lib$(ECHO).o: lib$(ECHO).c
	$(CC) $(CFLAGS) -D_INIT=lib$(IPADDR)_init -fPIC -c -o $@ $^


clean:
	-rm *.o *.so
