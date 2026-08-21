CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
SRCDIR = src
BUILDDIR = build
TARGET = program

all: $(TARGET)

$(TARGET): $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $<

clean:
	rm -f $(BUILDDIR)/$(TARGET)

.PHONY: all clean
