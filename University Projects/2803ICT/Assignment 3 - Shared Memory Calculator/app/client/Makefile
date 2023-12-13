CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_XOPEN_SOURCE=500
LDFLAGS = 

SRCDIR = ./src
OBJDIR = ../../obj/client
BINDIR = ../../bin
TESTDIR = ./tests

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

EXECUTABLE = $(BINDIR)/client

.PHONY: all clean tests

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) $^ -o $@ -lncurses -lpanel -lmenu -lform -lpthread

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

SOURCES_WITHOUT_SERVER = $(filter-out $(SRCDIR)/client.c, $(SOURCES))

tests:
	$(CC) $(CFLAGS) $(SOURCES_WITHOUT_SERVER) $(TESTDIR)/tests.c -o $(TESTDIR)/tests