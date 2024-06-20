.PHONY: all clean

CC        = cc
TARGET    = raycasting

SRCDIR    = src
BLDDIR    = build

CFLAGS    = -O3 -Wall -g

SRC       = $(wildcard   $(SRCDIR)/*.c)
OBJ       = $(subst      $(SRCDIR)/,$(BLDDIR)/,$(SRC:.c=.o))

$(BLDDIR)/%.o: $(SRCDIR)/%.c | $(BLDDIR)
	$(CC) -c $(CFLAGS) -o $@ $^
all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(BLDDIR)/$(TARGET)

clean:
	del /Q $(BLDDIR)\*
$(BLDDIR):
	mkdir $(BLDDIR)