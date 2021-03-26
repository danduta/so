CC=gcc
LIBS=-lm
CFLAGS=-I$(IDIR) $(LIBS) -fPIC

IDIR=./util
ODIR=./out
SRCDIR=./src
EXEC=libso_stdio.so

DEPS = $(wildcard $(IDIR)/*.h)
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(ODIR)/%.o)

all: build
build: $(EXEC) $(ODIR)

debug: clean
debug: CFLAGS += -D DEBUG
debug: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -shared -o $@ $^ 

$(ODIR):
	mkdir -p $@

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(EXEC) $(IDIR)/*~ 