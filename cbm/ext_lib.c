/******************************************************************************/
/*  GMT - GGLABS MEMORY TEST                                                  */
/*  A modern retro computer memory test optimized for coverage and speed      */
/*  Copyright 2017, 2018 Gabriele Gorla                                       */
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

#include<stdio.h>
#include<conio.h>
#include"system.h"

void __fastcall__ textframexy (unsigned char x, unsigned char y,
                               unsigned char width, unsigned char height,
                               unsigned char style)
{
  unsigned char i;
  gotoxy(x,y);
  if(style!=TEXTFRAME_TALL) {
    cputc(0xb0); // poke value 112
    for(i=0;i<width-2;i++)
      cputc(0x60); // poke value 64
    cputc(0xae); // poke value 110
  } else {
    printf(INV_ON "\xb5");
    for(i=0;i<width-2;i++)
      putchar(' '); // poke value 32
    // the above must be a putchar otherwise cc65 will print the wrong character
    printf("\xb6" INV_OFF);
  }

  for(i=1;i<height-1;i++) {
    gotoxy(x,y+i);
    cputc(0x7d); // poke value 93
    gotoxy(x+width-1,y+i);
    cputc(0x7d); // poke value 93
  }
  gotoxy(x,y+height-1);
  cputc(0xad); // poke value 109
  for(i=0;i<width-2;i++)
    cputc(0x60); // poke value 64
  cputc(0xbd);  // poke value 125
}

void SetBorderColor(unsigned char c)
{
  bordercolor(c);
}
