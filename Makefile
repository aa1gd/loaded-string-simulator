# Macros
CC = gcc
# CFLAGS =
CFLAGS = -g
# CFLAGS = -D NDEBUG

# Dependency rules for non-file targets
all: simulate
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f simulate *.o

# Dependency rules for file targets
simulate: simulate.o importdata.o asolve.o
	$(CC) $(CFLAGS) simulate.o importdata.o asolve.o -o simulate
simulate.o: simulate.c importdata.h asolve.h types.h 
	$(CC) $(CFLAGS) -c simulate.c
importdata.o: importdata.c importdata.h types.h
	$(CC) $(CFLAGS) -c importdata.c
asolve.o: asolve.c asolve.h types.h
	$(CC) $(CFLAGS) -c asolve.c
