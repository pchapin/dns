#
# Makefile for the DNS server project.
#

all:	dnsserver

dnsserver:	dnsserver.o
	gcc -o dnsserver dnsserver.o

dnsserver.o:	dnsserver.c
	gcc -c -Wall dnsserver.c
