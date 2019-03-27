## Makefile.am -- Process this file with automake to produce Makefile.in
AM_CPPFLAGS = -Wall -Wextra -I include/

bin_PROGRAMS = server \
	       client \
	       tests/test1 \
	       tests/test2 \
	       tests/test3

check_PROGRAMS = tests/test1 \
		 tests/test2 \
		 tests/test3

client_SOURCES = src/tcpcontext.c \
		 src/echoclientcontext.c \
		 src/client.c
server_SOURCES = src/bagarray.c \
		 src/tcpcontext.c \
		 src/echoservercontext.c \
		 src/echoclientcontext.c \
		 src/server.c

tests_test1_SOURCES = src/tcpcontext.c \
		      tests/test1.c
tests_test2_SOURCES = src/tcpcontext.c \
		      tests/test2.c
tests_test3_SOURCES = src/bagarray.c \
		      tests/test3.c
