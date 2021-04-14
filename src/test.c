#include <stdio.h>
#include <conio.h>
#include "lpeekpoke.h"
//#include "peekpoke.h"

unsigned char __fastcall__ Bank_Set2(unsigned char bank);
unsigned char __fastcall__ Bank_Write2(void);
void __fastcall__ LPOKE2(unsigned char bank, unsigned int addr, unsigned char val);
unsigned char __fastcall__ LPEEK2(unsigned char bank, unsigned int addr);


int main()
{
  unsigned char t;
  //  clrscr();
  //  gotoxy(10,2);
  printf("hello test\n");
  //  gotoxy(10,5);
  t=Bank_Set2(1);
  printf("LPOKE(0x00,0x6000,0x9a)\n");
  LPOKE(0x00,0x6000,0x9a);

  printf("PEEK(0x6000) = ");
  t=*((unsigned char *)(0x6000));
  printf("%02x\n",t);


  printf("LPOKE(0x02,0x600a,0x3f)\n");
  LPOKE(0x02,0x600a,0x3f);

  printf("LPEEK(0x02,0x600a) = ");
  t=LPEEK(0x02,0x600a);
  printf("%02x\n",t);
  printf("press a key\n");

  cgetc();
  return 0;
}


unsigned char __fastcall__ Bank_Set2(unsigned char bank)
{
  return bank+1;
}
unsigned char Bank_Write2(void)
{
  return 1;
}
void LPOKE2(unsigned char bank, unsigned int addr, unsigned char val)
{
  return;
}
unsigned char LPEEK2(unsigned char bank, unsigned int addr)
{
  return 1;
}
