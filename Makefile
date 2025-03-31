CC = gcc

# Detect if we're using GCC or Clang
COMPILER_TYPE := $(shell $(CC) --version 2>&1 | grep -q clang && echo "clang" || echo "gcc")

# Common base flags for both compilers
CFLAGS_BASE = -Wall -Wextra -pedantic -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
              -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
              -fPIE -fstack-protector-strong \
              -Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion \
              -Wno-newline-eof

# Add GCC-specific flags
ifeq ($(COMPILER_TYPE),gcc)
    CFLAGS_BASE += -D_GLIBCXX_ASSERTIONS -fstrict-flex-arrays=3 -fstack-clash-protection \
                  -Wtrampolines -Wbidi-chars=any
endif

# Add Clang-specific flags
ifeq ($(COMPILER_TYPE),clang)
    # Clang equivalents where available
    CFLAGS_BASE += -Wbidi-characters
    # Note: Clang doesn't support fstack-clash-protection, strict-flex-arrays=3, etc.
endif

# Base linker flags with security enhancements
LDFLAGS_BASE = -pie -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now \
               -Wl,--as-needed -Wl,--no-copy-dt-needed-entries

# Architecture-specific protections using compiler predefined macros
# For x86_64 architecture, add control flow protection if supported
ifneq ($(filter %gcc,$(CC)),)
    CFLAGS_BASE += -Wp,-D_FORTIFY_SOURCE=3
    # Check if running on x86_64
    ifeq ($(shell $(CC) -dumpmachine | grep -q 'x86_64'; echo $?),0)
        CFLAGS_BASE += -fcf-protection=full
    endif
    # Check if running on aarch64 or arm64
    ifeq ($(shell $(CC) -dumpmachine | grep -q 'aarch64\|arm64'; echo $?),0)
        CFLAGS_BASE += -mbranch-protection=standard
    endif
endif

# Project files
TARGET = free
SRCS = free.c 
OBJS = $(SRCS:.c=.o)

# Release build flags (production code)
CFLAGS = $(CFLAGS_BASE) -O2 -fno-delete-null-pointer-checks -fno-strict-overflow \
         -fno-strict-aliasing -ftrivial-auto-var-init=zero

LDFLAGS = $(LDFLAGS_BASE)

# Target-specific flags for 'test' (development)
test: CFLAGS = $(filter-out -O2, $(CFLAGS_BASE)) -g -fsanitize=address -O1 -Werror
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
