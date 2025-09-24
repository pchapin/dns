
CC := gcc
CFLAGS := -std=c17 -g -Og \
          -Wall -Wextra -Wpedantic -Wstrict-prototypes -Wmissing-prototypes -Wformat=2 -Wconversion -Wvla
# We intend to use POSIX APIs. Set the POSIX standard desired.
CFLAGS += -D_POSIX_C_SOURCE=200809L

dnsclient:	dnsclient.o
	$(CC) -o dnsclient dnsclient.o

dnsclient.o:	dnsclient.c
	$(CC) -c $(CFLAGS) dnsclient.c

.PHONY: clean
clean:
	rm -f *.o dnsclient
