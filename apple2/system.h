/******************************************************************************/
/*  GMT - GGLABS MEMORY TEST                                                  */
/*  A modern retro computer memory test optimized for coverage and speed      */
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

// test & memory definitions
typedef unsigned int tword_t;
#define BANK_SHIFT 6
#define MAX_BANK   0x7f

// screen constants
#define INV_ON          "\x0f"
#define INV_OFF         "\x0e"
#define MTEXT_ON        "\x1b"
#define MTEXT_OFF       "\x18"

// colors
#define COL_BLACK     0x00
#define COL_RED       0x01
#define COL_BLUE      0x02
#define COL_MED_BLUE  0x06
#define COL_GREEN     0x0C

// string constants
#define MEM_TYPE  "Apple IIgs"
#define MEM_CARD  "RAMGS"
#define EXTRA_AUTHORS " and Jason Andersen"
#define EXTRA_INFO "ROM"

#ifndef TEXTFRAME_TALL
#define TEXTFRAME_TALL 0x04

void __fastcall__ textframexy (unsigned char x, unsigned char y,
                               unsigned char width, unsigned char height,
                               unsigned char style);

#endif

#endif /* SYSTEM_H */
