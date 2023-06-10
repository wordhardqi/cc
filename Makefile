CFALGS=-std=c11 -ggdb -static
9cc: 9cc.c
	cc $(CFALGS)    9cc.c   -o 9cc
test: 9cc
	./test.sh
clean:
	rm -f 9cc *.0 *~ tmp*

.PHONY: test clean