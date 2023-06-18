CFALGS=-std=c11 -ggdb -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
CC=gcc
%.o: %.c
	$(CC)  -c  -o $@ $< $(CFALGS) $(LDFLAGS)
9cc: $(OBJS)
	$(CC) $(CFALGS)  -o $@ $^ $(LDFLAGS)
${OBJS}: 9cc.h

test: 9cc
	./test.sh
clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean