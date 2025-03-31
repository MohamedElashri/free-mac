CC = clang

# macOS-specific security flags
CFLAGS_BASE = -Wall -Wextra -pedantic -Wformat -Wformat=2 -Wconversion \
              -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
              -fPIE -fstack-protector-strong \
              -Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion \
              -Wno-newline-eof

# macOS linker flags
LDFLAGS_BASE = -pie

# Architecture-specific protections for macOS
ifeq ($(shell $(CC) -dumpmachine | grep -q 'x86_64'; echo $$?),0)
    # For Intel Macs, if supported
    CFLAGS_BASE += $(shell $(CC) -Werror -fcf-protection -c -x c /dev/null -o /dev/null 2>/dev/null && echo "-fcf-protection" || echo "")
endif

# Project files
TARGET = free
SRCS = free.c 
OBJS = $(SRCS:.c=.o)

# Release build flags
CFLAGS = $(CFLAGS_BASE) -O2 -fno-strict-overflow -fno-strict-aliasing

# Development build flags
test: CFLAGS = $(filter-out -O2, $(CFLAGS_BASE)) -g -fsanitize=address -O1
test: LDFLAGS = $(LDFLAGS_BASE) -fsanitize=address

LDFLAGS = $(LDFLAGS_BASE)

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
