Installation instructions
=========================

[Up](../dir.md) [Here](.)

(This file is currently stupid. GNU GPL v3
requires me to make installation instructions, and
even though the following information is defective
and incomplete, a programmer might actually save
half an hour from reading it. Nominally now I
fulfill the v3 installation instruction's
requirement, although praxis show that all
instructions are deficient outside of a clearly
declared context, and by defining a context
restrictive enough (f.ex. “the reader ought to be
a mindreader and guess my intentions”) the
installation instructions are implicitly provided
in the Makefile).

Requirements
============

* PostgreSQL

or other relational database management system
(RDBMS).

Download
========

make a subversion checkout from sternons by:

```
$ svn checkout http://sternons.googlecode.com/svn/trunk/ sternons-read-only
```

In Windows all commands of course should occur in
a cygwin shell window.

mkmap
=====

See [../mkmap/INSTALL](../mkmap/INSTALL.md)

cat
===

The directory `cat` contains two database tables
providing

1. Tabulae Rudolphinae published by Jacob Bartsch,
   the son-in-law of Johannes Kepler; everything
   sponsored by the Roman-German emperor Rudolph
   II with interests in astrology and the occult,

   - file: [tabulae-rudolphinae.db](tabulae-rudolphinae.db)
   - source: [TABULÆ RUDOLPHINÆ](http://www.ub.uni-kiel.de/digiport/bis1800/Arch3_436.html)

2. A catalogue of stars in the Calendarium of Al Achsasi Al Mouakket, analysed
   by E. B. Knobel

   - file: [alachsasi-knobel-1895.db](alachsasi-knobel-1895.db)
   - source: [Al Achsasi Al Mouakket, on a catalogue of stars in the Calendarium of](http://articles.adsabs.harvard.edu//full/1895MNRAS..55..429K/0000429.000.html)

Tabulae Rudolphinae
-------------------

```
$ cd sternons/trunk/cat
$ cp tabulae-rudolphinae.db ~/data/
```

In PostgreSQL create a table for Tabulae Rudolphinae by

```
joe=# CREATE TABLE Tab_Rud (
joe(#     tr_row int,
joe(#     tr_num int,
joe(#     description varchar(80),
joe(#     longitude varchar(7),
joe(#     latitude varchar(7),
joe(#     magnitude varchar(3),
joe(#     occurrence int,
joe(#     abbreviation varchar(4),
joe(#     page integer
joe(# );
```

and then import it by

```
joe=# COPY Tab_Rud FROM '/home/joe/data/tabulae-rudolphinae.db' DELIMITER '|';
```

It probably works similarly in most other database systems with anything like
standard compliance.

The original table is pretty well reproduced by the command:

```
joe=# SELECT * FROM Tab_Rud ORDER BY tr_row;
```

The table contains page information not representing stars (or supernovae),
namely those where tr_num = E'\n', so only stars are selected by:

```
joe=# SELECT * FROM Tab_Rud WHERE tr_num = E'\n' ORDER BY tr_num;
```

This presents stars measured up by Tycho Brahe,
by Hipparchos/Ptolemy and by the Dutch southern
sky observers Pieter Dirkszoon Keyser and
Frederick de Houtman (in the table erroneously
attributed to Johannes Bayer).

The entries tr_num between 1 and 1008 are from
Brahe, although it has been theorized that he
actually measured up slightly less than 1000 and
extrapolated the rest from either deficient own
observations or positions from
Hipparchos/Ptolemy. Two are not stars, but instead the reknowned supernovae of
Tycho in Cassiopeia and Kepler in Ophiuchus. These have trno in (221, 267).

The entries tr_num between 1008 and 1305 are from Hipparchos/Ptolemy not
measured up by Brahe.

The entries tr_num between 1305 and 1439 are from Keyser and de Houtman.

Al Achsasi Al Mouakket according to E. B. Knobel
------------------------------------------------

```
$ cd sternons/trunk/cat
$ cp alachsasi-knobel-1895.db ~/data/
```

In PostgreSQL create a table for Achsasi star table by

```
joe=# CREATE TABLE Achsasi (
joe(#     num int,
joe(#     arabic_name varchar(40),
joe(#     designation varchar(6),
joe(#     longitude varchar(6),
joe(#     latitude varchar(5),
joe(#     zone varchar(1),
joe(#     magnitude varchar(4),
joe(#     knobel_name varchar(40)
joe(# );
```

and then import it by

```
joe=# COPY Achsasi FROM '/home/joe/data/alachsasi-knobel-1895.db' DELIMITER '|';
```

