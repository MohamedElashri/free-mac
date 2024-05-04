CC=gcc
CFLAGS=-Wall -Werror
SOURCES=main.c memory_info.c format_utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=free

# Release build
release: CFLAGS += -O2
release: $(SOURCES) $(EXECUTABLE)

# Testing build with AddressSanitizer
test: CFLAGS += -fsanitize=address -g
test: LDFLAGS += -fsanitize=address
test: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
<tab>$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
<tab>$(CC) -c $(CFLAGS) $< -o $@

clean:
<tab>rm -f $(OBJECTS) $(EXECUTABLE)
