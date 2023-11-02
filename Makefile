# Compiler options
CC = gcc
# CFLAGS =
CFLAGS = -g -Wall
# CFLAGS = -D NDEBUG

# Build directory
BUILD = build

# Dependency rules for non-file targets
all: build simulate
clean:
	rm -rf $(BUILD) simulate
build:
	mkdir $(BUILD)

# Dependency rules for file targets
simulate: $(BUILD)/simulate.o $(BUILD)/importdata.o $(BUILD)/asolve.o
	$(CC) $(CFLAGS) $(BUILD)/simulate.o $(BUILD)/importdata.o $(BUILD)/asolve.o -o simulate
$(BUILD)/simulate.o: simulate.c importdata.h asolve.h types.h 
	$(CC) $(CFLAGS) -c simulate.c -o $(BUILD)/simulate.o
$(BUILD)/importdata.o: importdata.c importdata.h types.h
	$(CC) $(CFLAGS) -c importdata.c -o $(BUILD)/importdata.o
$(BUILD)/asolve.o: asolve.c asolve.h types.h
	$(CC) $(CFLAGS) -c asolve.c -o $(BUILD)/asolve.o
