.PHONY: all clean run

CC        = cc
TARGET    = bin\raymarching

SRCDIR    = src
INCDIR    = include
LIBDIR    = lib
BLDDIR    = build

CFLAGS    = -I$(INCDIR) -fopenmp -O3 -Wall -g
LDFLAGS   = -static -L$(LIBDIR)

SRC       = $(wildcard   $(SRCDIR)/*.c)
OBJ       = $(subst      $(SRCDIR)/,$(BLDDIR)/,$(SRC:.c=.o))

$(BLDDIR)/%.o: $(SRCDIR)/%.c | $(BLDDIR)
	$(CC) -c $(CFLAGS) -o $@ $^
all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

native:
	$(MAKE) CFLAGS="$(CFLAGS) -march=native" $(MAKEFLAGS)
run: all
	$(TARGET)
clean:
	del /S /F /Q $(BLDDIR)\*
	del $(TARGET).exe
$(BLDDIR):
	mkdir $(BLDDIR)