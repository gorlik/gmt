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

#include <peekpoke.h>
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
    // FIXME: non tall frames are incorrect
    cputc(0xb0);
    for(i=0;i<width-2;i++)
      cputc(0x60);
    cputc(0xae);
  } else {
    printf(INV_ON MTEXT_ON "Z" MTEXT_OFF);
    for(i=0;i<width-2;i++)
      putchar(' ');
    printf(MTEXT_ON "\xdf" INV_OFF MTEXT_OFF);
  }
  
  for(i=1;i<height-1;i++) {
    gotoxy(x,y+i);
    printf(INV_ON MTEXT_ON "Z" INV_OFF MTEXT_OFF);
    gotoxy(x+width-1,y+i);
    printf(INV_ON MTEXT_ON "\xdf" INV_OFF MTEXT_OFF);
  }
  
  gotoxy(x,y+height-1);
  printf(INV_ON MTEXT_ON "Z" INV_OFF MTEXT_OFF);
  for(i=0;i<width-2;i++)
    putchar('_'); 
  printf(INV_ON MTEXT_ON "\xdf" INV_OFF MTEXT_OFF);
}

void SetBorderColor(unsigned char c)
{
  unsigned char t;
  t=PEEK(0xC034);
  POKE(0xC034,(t&0xF0)|c&0x0F);
}
