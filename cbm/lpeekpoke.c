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

#include <peekpoke.h>
#include "lpeekpoke.h"

void __fastcall__ LPOKE(unsigned char bank, unsigned int addr, unsigned char val)
{
  POKE(0xDFFF,bank);
  POKE(0xDFFE,addr>>8);
  POKE(0xDE00|(addr&0xFF),val);
}

unsigned char __fastcall__ LPEEK(unsigned char bank, unsigned int addr)
{
  POKE(0xDFFF,bank);
  POKE(0xDFFE,addr>>8);
  return PEEK(0xDE00|(addr&0xFF));
}
