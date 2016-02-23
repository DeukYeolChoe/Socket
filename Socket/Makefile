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

all: Server_v3.o Client_v1.o
	$(CC) $(CFLAGS) -o Server_v3.o Client_v1.o

Server_v3.o: Server_v3.c
	$(CC) $(CFLAGS) -c Server_v3.c

Client_v1.o: Client_v1.c
	$(CC) $(CFLAGS) -c Client_v1.c

Server_echo.o: Server_echo.c
	$(CC) -pthread $(CFLAGS) -c Server_echo.c

clean:
	$(RM) all *.o *~
	
