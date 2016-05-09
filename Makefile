CFLAGS=-Wall
LDLIBS=-lm

radix: radix.o
	$(CC) $(LDLIBS) -o $@ $^

doc: notes.md
	pandoc --smart --standalone -t latex -o notes.pdf notes.md

clean:
	rm -f radix radix.o
