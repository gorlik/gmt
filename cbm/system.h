/******************************************************************************/
/*  GMT - GGLABS MEMORY TEST                                                  */
/*  A modern Apple IIgs/geoRAM memory test optimized for coverage and speed   */
/*  Copyright 2017-2020 Gabriele Gorla                                        */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GMT is distributed in the hope that it will be useful,                    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GMT.  If not, see <http://www.gnu.org/licenses/>.              */
/*                                                                            */
/******************************************************************************/

#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __SDCC
#define __fastcall__
#else
#define __reentrant
#endif

// test & memory definitions
typedef unsigned char tword_t;
#define TWORD_SIZE 1
#define BANK_SHIFT 4
#define MAX_BANK   0xff

// screen constants
#define INV_ON          "\x12"
#define INV_OFF         "\x92"
#define MTEXT_ON
#define MTEXT_OFF

// colors
#define COL_BLACK     0x00
#define COL_RED       0x02
#define COL_BLUE      0x02
#define COL_CYAN      0x03
#define COL_MAGENTA   0x04
#define COL_GREEN     0x05
#define COL_MED_BLUE  0x06

// string constants
#define MEM_TYPE  "geoRAM"
#define MEM_CARD  "GRAM"
#define EXTRA_AUTHORS ""
#define EXTRA_INFO  "System"

// library completion functions

#define TEXTFRAME_TALL 0x04

void __fastcall__ textframexy (unsigned char x, unsigned char y,
                               unsigned char width, unsigned char height,
                               unsigned char style);


#endif /* SYSTEM_H */
