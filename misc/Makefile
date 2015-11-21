CC			= gcc
MAKE		= make
LD			= ld

CFLAGS		+= -g -O0
LDFLAGS		+= -lm

TARGET		= sort
SOURCE		= sort.c
OBJS		= $(subst .c,.o,$(SOURCE))

all: $(TARGET)

config: 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	-rm -f $(TARGET) *.o 
