CC		= gcc --pedantic-error -Wall

tab2wiki: tab2wiki.c
	$(CC) -o $@ $<

other.wiki: other.tab tab2wiki
	cat other.tab | ./tab2wiki > other.wiki

clean:
	rm -f *~*
