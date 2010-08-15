/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU Lesser General Public License as         *
 *  published by the Free Software Foundation, either version 3 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU Lesser General Public License for more details.                 *
 *                                                                      *
 *  You should have received a copy of the GNU Lesser General Public    *
 *  License along with mkmap. If not, see                               *
 *  <http://www.gnu.org/licenses/>.                                     *
 ************************************************************************/

#ifndef _TODO_H
#define _TODO_H

/*[                                                                             
                                                                                
1. TODO, 2. ONGOING, 3. DID, 4. REJECTED AND ABORTED                            
                                                                                
TODO:                                                                           
    ITEM: 1                                                                     
        TITLE: Scan Sexagesimal                                                 
        FILE: 1-Scan60                                                          
        STATUS: vapor, not evaluated                                            
        DESCR: Scan sexagesimal notation for floating point numbers meaning     
          that 00h23m45.67s equals 0 + 23/60 + 45.67/3600.                      
        CAVEATS: this is not a unit, it is a way to express a different number  
          system and scale analogous to degrees vs. radians and decimal vs.     
          hexadecimal. How these are to be differentiated is unclear since      
          degrees can be expressed sexagesimally while radians cannot, and      
          00h23m45.67s implies a different scaling than 00°23'45.67", so that   
          there is a partial conceptual overlap between sexagesimal and scaling 
          (units).
ONGOING:
DID:
REJECTED AND ABORTED:

]*/

#endif /* _TODO_H */
