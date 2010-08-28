########################################################################
#  Copyright (C) 2010 Tomas Kindahl                                    #
########################################################################
#  This file is part of mkmap                                          #
#                                                                      #
#  mkmap is free software: you can redistribute it and/or modify it    #
#  under the terms of the GNU Lesser General Public License as         #
#  published by the Free Software Foundation, either version 3 of the  #
#  License, or (at your option) any later version.                     #
#                                                                      #
#  mkmap is distributed in the hope that it will be useful,            #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of      #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       #
#  GNU Lesser General Public License for more details.                 #
#                                                                      #
#  You should have received a copy of the GNU Lesser General Public    #
#  License along with mkmap. If not, see                               #
#  <http://www.gnu.org/licenses/>.                                     #
########################################################################

CC		= gcc -g -Wall --pedantic-error
MAPS	= star.db
OBJS	= scan.o ucstr.o uctype.o usio.o

mkmap: mkmap.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

#cmap: cmap.c $(OBJS)
#	$(CC) -o $@ $< $(OBJS)

.c.o:
	$(CC) -c $<

orion.svg: mkmap maps/orion.mkm $(MAPS)
	./$< maps/orion.mkm $(MAPS) > $@

orion.png: orion.svg 
	inkscape --export-png=$@ $<

#EXAMPLE FOR SEPARATING WIN AND LINUX OBJS:
#win/xy.o: xy.c
#	cd win; $(CC) -c ../xy.c

clean:
	rm -f *.o
	rm -f *~*

distclean: clean
	rm -f cmap cmp dump mkmap

hard: clean
	$(MAKE) cmap

view: orion.svg
	epiphany $< &
