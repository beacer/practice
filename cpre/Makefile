
# ----------------------------------------------
# flags
OBJS = cpre.o cpre_test.o

CFLAGS = -Wall -g -O0

LIBS = -lpcre

# ----------------------------------------------
# start
all: match replace

match: cpre
	-ln -s ./cpre match

replace: cpre
	-ln -s ./cpre replace

cpre: ${OBJS}
	gcc -o cpre ${CFLAGS} ${OBJS} ${LIBS}

clean:
	-rm cpre match replace *.o

