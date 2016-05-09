CFLAGS=-Wall
LDLIBS=-lm

radix: radix.o
	$(CC) $(LDLIBS) -o $@ $^

clean:
	rm -f radix radix.o
