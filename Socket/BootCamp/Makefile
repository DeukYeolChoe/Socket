# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = gcc
CFLAGS = -g

default: all

all: main.o server.o validation.o log.o parser.o
	$(CC) $(CFLAGS) -o Server main.o server.o validation.o log.o parser.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

validation.o: validation.c
	$(CC) $(CFLAGS) -c validation.c

log.o: log.c
	$(CC) $(CFLAGS) -c log.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

clean:
	$(RM) all *.o *~
	
