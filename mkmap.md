# Mkmap #

![http://sternons.googlecode.com/svn/wiki/img/Ori-small.png](http://sternons.googlecode.com/svn/wiki/img/Ori-small.png)

## Description ##

**mkmap** generates a star map in SVG format

**Purpose:** a script for generating [SVG](http://en.wikipedia.org/wiki/SVG)
and [PostScript](http://en.wikipedia.org/wiki/PostScript) star maps from a
configuration file.

## Current state ##

**User manual:** [User Manual](http://sternons.blogspot.com/p/mkmap.html)
_(nonexistent, currently just a placeholder web page)_

**Technical manual:** See [How it works](#How_it_works.md) below!

**Development:**

**Plan:** See [Hack plan](#Hack_plan.md) below!

**Code:** Prealpha code generating star maps such as for example the one
above (the easily recognized
[constellation of Orion](http://en.wikipedia.org/wiki/Orion_(constellation))),
the command for generating _any_ star map being:
```
 ./mkmap star.db > cmap.svg
```
For now the direction and scaling is hard coded in the source code in the
[main program](http://code.google.com/p/sternons/source/browse/trunk/mkmap/mkmap.c?r=35#231)
as
```
 image_struct *frame = new_image(500, 500, 1.4);
 proj_struct *projection = init_Lambert_deg(80, 0, 10, 20);
```
but this is an atrocity and must ASAP be fixed by loading a map program file.

## Hack plan ##

  1. _introduce mkmap language_: the language loads a program then runs it, the program may contain setups of environment, then drawing stars and other objects, either by an object drawing command, or by a database load where a list of objects is constructed and each object is then drawn.
    * ~~A<sub>0</sub>: the current state (2010-08-01) is that the star drawing is hard coded, as per above,~~
    * ~~A<sub>1</sub>: next is that a settings file that does nothing is loaded~~
    * A<sub>1.5</sub>: next load a settings file that changes image frame settings
    * A<sub>2</sub>: thereafter the settings file that only changes the chosen projection and frame is loaded, the map must still be supplied as an argument
    * A<sub>3</sub>: the map is loaded by the settings file
    * _(dismissed)_ ~~A<sub>4</sub>: an extra output file is supplied as an argument~~
    * A<sub>5</sub>: stepwise make the “settings file” a program and the extra arguments real arguments read by the program

## How it works ##

### mkmap processing pipeline ###
  1. _init:_ mkmap makes the map(s) settings according to map program
  1. _load:_ mkmap loads/transforms file rows into an object list (object refers to just any drawing thing)
  1. mkmap adds object overrides from the map program
  1. _position:_ for each object mkmap applies a function to determine where it is to be drawn, if on multiple maps duplicate it to each map
  1. mkmap deletes all objects that fall outside the map, truncate the parts of the grand objects that fall outside the map ...
  1. _draw:_ mkmap determines how the object is to be drawn, what symbol, font, color dashing, label, label position etc.
> > (if creating a binary format, it should be saved here)
  1. mkmap “draws” all objects into one or more maps by writing into an SVG file

### mkmap programs ###
**UNDEFINED!** The current theory is that mkmap programs are (going to be) objects
like (in pseudocode):
```
 FILE “orion.mkm”:

 object orion() {
   inherit sky;
   init:
     // image and projection inherited from the program “sky.mkm”, then overrides:
     image = { width = 500, height = 500 };
     projection = { type = Lambert_conic, RA = 80, DE = 10, range = 10 };
   load:
     // here the loadings inherited from “sky.mkm”, then extras:
     // ...[insertme]...
     // the loads are really performed at the end of this slot
   position:
     // here the position computation settings inherited from “sky.mkm”, then changed setups:
     // ...[insertme]...
     // the positioning is really performed at the end of this slot
   draw:
     // everything from “sky.mkm” plus “orion.mkm” drawn here
     // ...[insertme]...
     // the output is really performed at the end of this slot
 }
```
The reason for this **elaborate** thinking is that the mkmap “programmer” should be
given full control over the processing, and also that mkmap should be generalizable
to other kind of maps.