OBJS := packman_encode.o

LIBNAME := packman
LIB := lib$(LIBNAME).so

CFLAGS += -Wall -Werror -fPIC -D_GNU_SOURCE

default: all

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $($@_CFLAGS)

%.out: %.o
	$(CC) $< -o $@ $(CFLAGS) $($@_CFLAGS)

$(LIB): $(OBJS)
	$(LD) $^ -o $@ -shared $(LDFLAGS) $($@_LDFLAGS)

all: $(LIB)

clean:
	rm -f $(LIB) $(OBJS) *~
