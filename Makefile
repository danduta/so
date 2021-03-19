CC=cl
CFLAGS=/I$(IDIR) /std=c89 /MD /Wall

IDIR=./include
ODIR=./out
SRCDIR=./src
EXEC=so-cpp.exe

all: $(EXEC)
build: $(EXEC)

$(EXEC): $(ODIR)/so-cpp.obj $(ODIR)/so-cpp-main.obj $(ODIR)/so-cpp-hashmap.obj $(ODIR)/so-cpp-list.obj $(ODIR)/so-cpp-utils.obj
	$(CC) /out:$@ $** $(CFLAGS) $(LIBS)

$(ODIR)/so-cpp.obj: $(SRCDIR)/so-cpp.c $(IDIR)/so-cpp.h $(IDIR)/so-cpp-hashmap.h $(IDIR)/so-cpp-list.h $(IDIR)/so-cpp-utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRCDIR)/so-cpp.c /c /out:$@
	
$(ODIR)/so-cpp-main.obj: $(SRCDIR)/so-cpp-main.c $(IDIR)/so-cpp.h $(IDIR)/so-cpp-hashmap.h $(IDIR)/so-cpp-list.h $(IDIR)/so-cpp-utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRCDIR)/so-cpp-main.c /c /out:$@
	
$(ODIR)/so-cpp-hashmap.obj: $(SRCDIR)/so-cpp-hashmap.c $(IDIR)/so-cpp.h $(IDIR)/so-cpp-hashmap.h $(IDIR)/so-cpp-list.h $(IDIR)/so-cpp-utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRCDIR)/so-cpp-hashmap.c /c /out:$@
	
$(ODIR)/so-cpp-list.obj: $(SRCDIR)/so-cpp-list.c $(IDIR)/so-cpp.h $(IDIR)/so-cpp-hashmap.h $(IDIR)/so-cpp-list.h $(IDIR)/so-cpp-utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRCDIR)/so-cpp-list.c /c /out:$@
	
$(ODIR)/so-cpp-utils.obj: $(SRCDIR)/so-cpp-utils.c $(IDIR)/so-cpp.h $(IDIR)/so-cpp-hashmap.h $(IDIR)/so-cpp-list.h $(IDIR)/so-cpp-utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRCDIR)/so-cpp-utils.c /c /out:$@

.PHONY: clean

clean:
	del $(ODIR)/*.obj *~ $(EXEC) $(IDIR)/*~ 