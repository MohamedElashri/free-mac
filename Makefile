CC = gcc
# Base flags
CFLAGS = -Wall -Wextra -pedantic -O2 -fPIE -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wno-newline-eof
# Test build flags 
CFLAGS_TEST = $(filter-out -D_FORTIFY_SOURCE=2 -O2, $(CFLAGS)) -g -fsanitize=address -O1
LDFLAGS =

# Project files
TARGET = free
TARGET_TEST = $(TARGET) 	
SRCS = free.c 
OBJS = $(SRCS:.c=.o)
OBJS_TEST = $(SRCS:.c=.o.test)

.PHONY: all release test clean

# Default target
all: release

# Release build targets
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test build targets (with AddressSanitizer)
test: $(TARGET_TEST)

$(TARGET_TEST): $(OBJS_TEST)
	$(CC) $(LDFLAGS) -fsanitize=address -o $@ $^

%.o.test: %.c
	$(CC) $(CFLAGS_TEST) -c $< -o $@ # Use dedicated test flags

# Clean target
clean:
	rm -f $(TARGET) $(OBJS) $(OBJS_TEST)
