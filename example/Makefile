EXAMPLE := example
LIBDIR=../lib
LIB=$(LIBDIR)/libpackman.so
$(EXAMPLE)_CFLAGS := -L$(LIBDIR) -I$(LIBDIR) -lpackman

CFLAGS += -Wall -Werror -fPIC -D_GNU_SOURCE

default: all

$(LIB):
	$(MAKE) -C $(LIBDIR) all

%: %.c
	$(CC) $< -o $@ $(CFLAGS) $($@_CFLAGS)

all: $(EXAMPLE) $(LIB)

clean:
	rm -f $(EXAMPLE) *~

run: all
	LD_LIBRARY_PATH=$(LIBDIR) ./$(EXAMPLE)
