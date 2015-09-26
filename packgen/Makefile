CC			= gcc
MAKE		= make
LD			= ld

CFLAGS		+= -g -O0

PGENTAR		= pgen
PGENSRC		= packgen.c lex.yy.c y.tab.c
PGENSRC		+= prot_eth.c prot_ip6.c prot_udp.c prot_dhcp6.c
PGENOBJ		= $(subst .c,.o,$(PGENSRC))

all: $(PGENTAR)

config: config.l config.y
	yacc -d config.y
	lex config.l

$(PGENTAR): $(PGENOBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	-rm -f $(PGENTAR) *.o lex.yy.c y.tab.c y.tab.h
