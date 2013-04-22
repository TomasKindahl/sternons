########################################################################
#  Copyright (C) 2010-2012 Tomas Kindahl                               #
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
OBJS 	= $(STROBJS) pointobj.o allstrings.o mathx.o projection.o \
		  valstack.o image.o line.o polygon.o label.o progstat.o \
		  fields.o ps_db.o ps_draw.o vmcode.o
OBJSP 	= $(OBJS) parse.o
SWOBJS	= token.obj ucstr.obj uctype.obj usio.obj
WOBJS 	= $(SWOBJS) pointobj.obj allstrings.obj mathx.obj projection.obj \
          valstack.obj image.obj line.obj polygon.obj label.obj progstat.obj \
		  fields.obj ps_db.obj ps_draw.obj vmcode.obj
WOBJSP 	= $(WOBJS) parse.obj

all: img

img: mkmap maps/orion.mkm $(MAPS)
	./$<
#	./$< maps/orion.mkm

mbf: mkmap maps/ori2.mbf
	./$< maps/ori2.mbf

mbf3: mkmap maps/ori3.mbf
	./$< maps/ori3.mbf

mkmap: mkmap.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

mkmap.c: mkmap.uc $(UCCVT)
	$(UCCVT) $<

pointobj.c: pointobj.uc $(UCCVT)
	$(UCCVT) $<

parse_prog2.c: parse_prog2.uc $(UCCVT)
	$(UCCVT) $<

parse_prog.c: parse_prog.uc $(UCCVT)
	$(UCCVT) $<

parse_program: parse_program.c $(OBJSP)
	$(CC) -o $@ $< $(OBJSP) -lm

parse_prog: parse_prog.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

parse_prog2: parse_prog2.c $(OBJS)
	$(CC) -o $@ $< $(OBJS) -lm

.c.o:
	$(CC) -c $<

orion.svg: mkmap maps/orion.mkm $(MAPS)
	./$< maps/orion.mkm

monoceros.svg: mkmap maps/monoceros.mkm $(MAPS)
	./$< maps/monoceros.mkm

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

formats: formats.c
	$(CC) -std=c99 -o $@ $< -lm

