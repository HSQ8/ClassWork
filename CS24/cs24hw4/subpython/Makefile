OBJS=repl.o global.o parse.o eval.o alloc.o

CFLAGS=-Wall -g -O0 -pedantic -Wall -Wextra -Werror
LDFLAGS=-lm -lreadline
CFILES= alloc.c eval.c global.c parse.c repl.c types.h

all: subpython

subpython: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o subpython $(LDFLAGS)

clean:
	rm -f *.o subpython

.PHONY: all clean
