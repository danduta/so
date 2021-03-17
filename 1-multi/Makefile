CC=gcc
LIBS=-lm
CFLAGS=-I$(IDIR) $(LIBS) -std=c89

IDIR=./include
ODIR=./out
SRCDIR=./src
EXEC=so-cpp

DEPS = $(wildcard $(IDIR)/*.h)
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(ODIR)/%.o)

all: $(EXEC)

debug: clean
debug: CFLAGS += -D DEBUG
debug: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(EXEC) $(IDIR)/*~ 