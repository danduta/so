CC=gcc
LIBS=-lm
CFLAGS=-I$(IDIR) $(LIBS) -g

IDIR=./include
ODIR=./out
SRCDIR=./src

DEPS = $(wildcard $(IDIR)/*.h)
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(ODIR)/%.o)

all: so-cpp

so-cpp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ so-cpp $(IDIR)/*~ 