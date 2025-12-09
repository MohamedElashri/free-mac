CC = gcc
# Base flags
CFLAGS_BASE = -Wall -Wextra -pedantic -fPIE -fstack-protector-strong -Wformat -Wformat-security -Wno-newline-eof
LDFLAGS_BASE =

# Project files
TARGET = free
SRCS = free.c 
OBJS = $(SRCS:.c=.o)

# Default flags (Release)
CFLAGS = $(CFLAGS_BASE) -O2 -D_FORTIFY_SOURCE=2
LDFLAGS = $(LDFLAGS_BASE)

# Target-specific flags for 'test'
test: CFLAGS = $(filter-out -D_FORTIFY_SOURCE=2 -O2, $(CFLAGS_BASE)) -g -fsanitize=address -O1
test: LDFLAGS = $(LDFLAGS_BASE) -fsanitize=address

.PHONY: all release test clean

# Default target
all: release

# Build targets 
release: $(TARGET)
test: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(TARGET) $(OBJS)
