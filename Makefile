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
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)