CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_XOPEN_SOURCE=500
LDFLAGS =

SRCDIR = ./src
OBJDIR = ../../obj/server
BINDIR = ../../bin
TESTDIR = ./tests

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

EXECUTABLE = $(BINDIR)/server

.PHONY: all clean tests test-client

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

SOURCES_WITHOUT_SERVER = $(filter-out $(SRCDIR)/server.c, $(SOURCES))

test-client:
	$(CC) $(CFLAGS) $(TESTDIR)/test_client.c -o $(TESTDIR)/test-client

tests:
	$(CC) $(CFLAGS) $(SOURCES_WITHOUT_SERVER) $(TESTDIR)/tests.c -o $(TESTDIR)/tests