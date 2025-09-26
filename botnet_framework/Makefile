# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -I./include
LDFLAGS = -pthread

# Executable name
EXEC = bot

# Find all .c files in src and its subdirectories
SRCS = $(shell find src -name '*.c')

# Replace .c with .o to get the object file names
OBJS = $(SRCS:.c=.o)

# Default rule: build the executable
all: $(EXEC)

# Rule to link the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean
