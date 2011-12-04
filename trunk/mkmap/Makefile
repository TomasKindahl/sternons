########################################################################
#  Copyright (C) 2010 Tomas Kindahl                                    #
########################################################################
#  This file is part of mkmap                                          #
#                                                                      #
#  mkmap is free software: you can redistribute it and/or modify it    #
#  under the terms of the GNU General Public License as published by   #
#  the Free Software Foundation, either version 3 of the License, or   #
#  (at your option) any later version.                                 #
#                                                                      #
#  mkmap is distributed in the hope that it will be useful, but        #
#  WITHOUT ANY WARRANTY; without even the implied warranty of          #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       #
#  GNU General Public License for more details.                        #
#                                                                      #
#  You should have received a copy of the GNU General Public License   #
#  along with mkmap. If not, see <http://www.gnu.org/licenses/>.       #
########################################################################

.SUFFIXES: .obj .mkm .png .svg

CC 		= gcc -g -Wall --pedantic-error
UCCVT	= ./ucliterals
MAPS 	= star.db
STROBJS	= token.o ucstr.o uctype.o usio.o
OBJS 	= $(STROBJS) pointobj.o allstrings.o mathx.o projection.o
OBJSP 	= $(OBJS) parse.o
SWOBJS	= token.obj ucstr.obj uctype.obj usio.obj
WOBJS 	= $(SWOBJS) pointobj.obj allstrings.obj mathx.obj projection.obj
WOBJSP 	= $(WOBJS) parse.obj

all: img

img: mkmap maps/orion.mkm $(MAPS)
	./$< maps/orion.mkm $(MAPS)

mkmap: mkmap.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

mkmap.c: mkmap.uc $(UCCVT)
	$(UCCVT) $<

pointobj.c: pointobj.uc $(UCCVT)
	$(UCCVT) $<

parse_program: parse_program.c $(OBJSP)
	$(CC) -o $@ $< $(OBJSP) -lm

.c.o:
	$(CC) -c $<

orion.svg: mkmap maps/orion.mkm $(MAPS)
	./$< maps/orion.mkm $(MAPS)

monoceros.svg: mkmap maps/monoceros.mkm $(MAPS)
	./$< maps/monoceros.mkm $(MAPS)

orion.png: orion.svg 
	inkscape --export-png=$@ $<

clean:
	rm -f *.o
	rm -f *.obj
	rm -f *~*
	rm -f \#*\#

distclean: clean
	rm -f cmap cmp dump mkmap
	rm -f *.exe

hard: clean
	$(MAKE) cmap

view: orion.svg
	epiphany $< &

ucliterals: ucliterals.c usio.c usio.h ucstr.c ucstr.h uctype.c $(STROBJS)
	$(CC) -o $@ $< $(STROBJS)

allstrings.o: allstrings.c
	$(CC) -c $<

#### For Windows:
mkmap.exe: mkmap.c $(WOBJS)
	$(CC) -o $@ $< $(WOBJS) -lm

parse_program.exe: parse_program.c $(WOBJSP)
	$(CC) -o $@ $< $(WOBJSP) -lm

.c.obj:
	$(CC) -o $@ -c $<

winimg: mkmap.exe maps/orion.mkm $(MAPS)
	./$< maps/orion.mkm $(MAPS)

#### svn tools:

# difference overview (preparation for commit message)
svndiff:
	svn diff --diff-cmd /usr/bin/diff -x '--brief'

# generate commit message stub diff.u8
difflist: diff.u8

diff.u8:
	svn diff | grep Index: > diff.u8

status:
	svn status | grep -v '.png' | grep -v '.svg'

