CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
SOURCES = $(wildcard Backend/*.c)
HEADERS = $(wildcard Backend/*.h)

.PHONY: all test clean
all: loja
loja: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $@ -lm
test:
	python3 tests/test_regressions.py
clean:
	rm -f loja
	rm -f app