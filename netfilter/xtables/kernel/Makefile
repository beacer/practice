# -*- Makefile -*-

MODULES_DIR = /lib/modules/$(shell uname -r)
KERNEL_DIR = ${MODULES_DIR}/build  # kernel source tree for build

EXTRA_CFLAGS = -DKERNEL
EXTRA_CFLAGS += -I$(PWD)/../include
obj-m += xt_ipaddr.o
obj-m += xt_ECHO.o

all:
	make -C ${KERNEL_DIR} M=$$PWD;

modules:
	make -C ${KERNEL_DIR} M=$$PWD $@;

modules_install:
	make -C ${KERNEL_DIR} M=$$PWD $@;

clean:
	make -C ${KERNEL_DIR} M=$$PWD $@;
