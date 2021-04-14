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

#ifndef MTEST_H
#define MTEST_H

unsigned char __fastcall__ Bank_Set(unsigned char bank);
unsigned char __fastcall__ Bank_Write(void);
unsigned char __fastcall__ Bank_Read(void);
unsigned char __fastcall__ AAD_Write(void);
unsigned char __fastcall__ AAD_Read(void);

#endif /* MTEST_H */
