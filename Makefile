CC=gcc
CFLAGS=-Wall -Werror
SOURCES=main.c memory_info.c format_utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=free

# Unit tests
UNIT_TEST_SOURCES=unit_tests.c memory_info.c format_utils.c
UNIT_TEST_OBJECTS=$(UNIT_TEST_SOURCES:.c=.o)
UNIT_TEST_EXECUTABLE=unit_tests

# Release build
release: CFLAGS += -O2
release: $(SOURCES) $(EXECUTABLE)

# Testing build with AddressSanitizer
test: CFLAGS += -fsanitize=address -g
test: LDFLAGS += -fsanitize=address
test: $(SOURCES) $(EXECUTABLE) $(UNIT_TEST_EXECUTABLE)
	./$(UNIT_TEST_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(UNIT_TEST_EXECUTABLE): $(UNIT_TEST_OBJECTS)
	$(CC) $(CFLAGS) $(UNIT_TEST_OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(UNIT_TEST_OBJECTS) $(UNIT_TEST_EXECUTABLE)