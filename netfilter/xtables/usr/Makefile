CFLAGS := -Wall
CFLAGS += -g -O0

CFLAGS += -I../include

CC = gcc

LIB	= xt_ipaddr

lib$(LIB).so: lib$(LIB).o
	$(CC) -shared -fPIC -o $@ $^

# xtables need _INIT for expand the _init macro, see xtables.h
lib$(LIB).o: lib$(LIB).c
	$(CC) $(CFLAGS) -D_INIT=lib$(lib)_init -fPIC -c -o $@ $^

clean:
	-rm *.o *.so $(TAR)
