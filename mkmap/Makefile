CC		= gcc -Wall --pedantic-error
MAPS	= star.db
OBJS	= scan.o ucstr.o uctype.o usio.o

mkmap: mkmap.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

cmap: cmap.c $(OBJS)
	$(CC) -o $@ $< $(OBJS)

.c.o:
	$(CC) -c $<

cmap.svg: mkmap $(MAPS)
	./$< $(MAPS) > $@

cmap.png: cmap.svg
	inkscape --export-png=$@ $< 

#EXAMPLE FOR SEPARATING WIN AND LINUX OBJS:
#win/xy.o: xy.c
#	cd win; $(CC) -c ../xy.c

clean:
	rm -f *.o

distclean: clean
	rm -f cmap cmp dump mkmap

hard: clean
	$(MAKE) cmap


