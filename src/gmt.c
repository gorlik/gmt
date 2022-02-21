/******************************************************************************/
/*  GMT - GGLABS MEMORY TEST                                                  */
/*  A modern retro computer memory test optimized for coverage and speed      */
/*  Copyright 2017-2022 Gabriele Gorla                                        */
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <conio.h>
#include <peekpoke.h>

#include "system.h"
#include "mtest.h"
#include "lpeekpoke.h"

#if defined(__C128__)
#define SCR_WIDTH       80
#else
#define SCR_WIDTH       40
#endif

#define VER "0.48"
#define LOG_LINES 10
#define LOG_LEN   (80)
#define STATUS_X  (SCR_WIDTH/2+2)
#define STATUS_Y  7

#if SCR_WIDTH==40
#define CFG_X 2
#define FATAL_X 0
#else
#define CFG_X 3
#define FATAL_X 20
#endif

#define LOG_X (CFG_X)


// define TEST to enable error injection for emulator testing
//#define TEST
#define ERR_BANK 0x12
#define ERR_ADDR 0x1040

// This structure is shared with the assebmly code. Any modification will
// likely require changes in the assembly as well
struct error {
  unsigned int addr;
  tword_t      expected;
  tword_t      read1;
  tword_t      read2;
  tword_t      read3;
  tword_t      read4;
  tword_t      pad1;
  // struct error is 8 bytes for 8-bit tword
  // and 16 bytes for 16-bit tword
#if TWORD_SIZE==2
  tword_t        pad2;
#endif
};

typedef void (*test_ptr)(unsigned char, const tword_t *, unsigned char) __reentrant;

struct test_t {
  const char *name;
  test_ptr run;
  const tword_t *pat;
  unsigned char pat_len;
};

/*****************************************************************************/
/*                                Prototypes                                 */
/*****************************************************************************/

// actual tests
void MATS(unsigned char bank, const tword_t *pat, unsigned char len) __reentrant;
void SSO (unsigned char bank, const tword_t *pat, unsigned char len) __reentrant;
void AAD (unsigned char bank, const tword_t *pat, unsigned char len) __reentrant;

// test helpers
void Set_Pat(tword_t pat);
void Test_Complete(unsigned char bb);
int  Start_Test(unsigned int mstart, unsigned int mend, unsigned char t);

// utility functions
static unsigned int Mem_Detect(unsigned char mstart);
static unsigned char Mem_Check_Size(unsigned char size);
static void Mainloop(void);
static void Diag(unsigned char size);

// text GUI helpers
void Main_Menu(void);
void SetBorderColor(unsigned char c);
void Clear_Status(unsigned char b);
void Set_Status(char *format, unsigned char b);
void fatalframe(char * message);
void nicetextframexy(unsigned char x,unsigned char y,unsigned char w,unsigned char h,char * title);
//void niceframetitle(unsigned char x, unsigned char y, unsigned char w, char * title);
void solidframetitle(unsigned char x, unsigned char y, unsigned char w, char * title);
static void Banner(unsigned char bb);

// Log window
char *log(void);
void updatelog(void);
void PrintErrorLog(unsigned int err, unsigned char bank);

/*****************************************************************************/
/*               Variables shared with the assembly code                     */
/*****************************************************************************/
struct error err_list[16];

tword_t DPat; // current data pattern
unsigned int NErr; // total number of errors

/*****************************************************************************/
/*                                Test Data                                  */
/*****************************************************************************/

static const tword_t mats4_pat[]  = { (tword_t)0x0000, (tword_t)0x5555,
                                      (tword_t)0x3333 };
static const tword_t mats8_pat[]  = { (tword_t)0x0000, (tword_t)0x5555,
                                      (tword_t)0x0f0f, (tword_t)0x3333 };

#if TWORD_SIZE==2
static const tword_t mats16_pat[] = { 0x0000, 0x5555, 0x00ff, 0x0f0f, 0x3333 };
static const tword_t sso_pat[]    = { 0x01fe, 0x02fd, 0x04fb, 0x08f7,
				      0x10ef, 0x20df, 0x40bf, 0x807f };
#endif

static const struct test_t test[] = {
  //  { "ALL  ", NULL,  NULL, 0 },
  { "MATS4 ", MATS, mats4_pat,  sizeof(mats4_pat)/sizeof(tword_t) },
  { "MATS8 ", MATS, mats8_pat,  sizeof(mats8_pat)/sizeof(tword_t) },
#if TWORD_SIZE==2
  { "MATS16", MATS, mats16_pat, sizeof(mats16_pat)/sizeof(tword_t) },
  { "SSO   ", SSO,  sso_pat,    sizeof(sso_pat)/sizeof(tword_t) },
#endif
  { "AAD   ", AAD,  NULL,       0 }
};


/*****************************************************************************/
/*                              Local Variables                              */
/*****************************************************************************/

static unsigned int mstart, mend;  // detected expansion memory
static const char *rom_ver;               // detected rom version

static char log_data[LOG_LINES][LOG_LEN+1];
static unsigned char log_put;


/*****************************************************************************/
/*                                   main                                    */
/*****************************************************************************/

int main (void)
{
  unsigned char os;

  SetBorderColor(COL_MED_BLUE);
  
#if defined (__APPLE2ENH__)
  
  videomode(VIDEOMODE_80COL);
#if SCR_WIDTH==40
  // go back to 40 column with mousetext enabled
  putchar(0x11);
#endif
  // forces color on composite
  //  POKE(0xC021,PEEK(0xC021)&0x7F); 
  
  os=get_ostype();

  if( (os&APPLE_IIGS)!=APPLE_IIGS ) {
    fatalframe("This program requires an Apple IIgs");
    goto quit;
  }

  switch(os) {
  case APPLE_IIGS1:
    rom_ver="ROM01";
    mstart = 0x02;
    break;
  case APPLE_IIGS3:
    rom_ver="ROM3";
    mstart = 0x10;
    break;
  default:
    rom_ver="UNKOWN";
    mstart = 0x02;
    break;
  }    
#else // not Apple II
  os=0;
  mstart=0;
#if defined(__C128__)
  videomode(VIDEOMODE_80COL);
  fast();
  rom_ver="C128";
  #else
  rom_ver="C64";
#endif
#endif
  
  mend=Mem_Detect(mstart);
  if (mend==mstart) {
      fatalframe("No Memory Expansion Detected");
      goto quit;
  }

  Main_Menu();
#ifdef DIAG
  Diag(DIAG);
#else
  Mainloop();
#endif
  
 quit:
  clrscr();
  //  return EXIT_SUCCESS;
  return 0;
}

#ifdef DIAG
static void Diag(unsigned char size)
{
  unsigned char t, BadBanks;
  char in;
  t=0;

  gotoxy(CFG_X,4);
  if(size==5) printf("GGLABS " MEM_CARD "/512");
  else  printf(" GGLABS " MEM_CARD "/%d", size);
  gotoxy(CFG_X,5);
  printf("  Diagnostics");

  t=Mem_Check_Size(size);
  if(t) {
    sprintf(log(),"Check Size Failed s:%02X e:%02X",mstart,mend-1);
    Banner(size);
    SetBorderColor(COL_RED);
    updatelog();
    cgetc();
  }
  
  do {
    Clear_Status(1);
    BadBanks=0;
    sprintf(log(),"Started Test %s on bank %02X to %02X",test[t].name,mstart,mend-1);
    updatelog();
    BadBanks=Start_Test(mstart,mend,t);
    Banner(BadBanks);
    Test_Complete(BadBanks);

    in=cgetc();
    in=toupper(in);
  } while (in!='Q');
  
  Clear_Status(1);
  //  rebootafterexit();
}
#endif

#ifndef DIAG
static void Mainloop(void)
{
  unsigned char t, BadBanks;
  char in;
  t=0;
  
  do {
    gotoxy(CFG_X,4);
    printf("[T] Test: %s",test[t].name);
    gotoxy(CFG_X,6);
    printf("[" INV_ON MTEXT_ON "M" MTEXT_OFF INV_OFF "] Start");
#ifndef CART
    gotoxy(CFG_X,7);
    printf("[Q] Quit");
#endif
    
    in=cgetc();
    in=toupper(in);

    if(in==13) {
      Clear_Status(1);
      BadBanks=0;
      sprintf(log(),"Started Test %s on bank %02X to %02X",test[t].name,mstart,mend-1);
      updatelog();
      BadBanks=Start_Test(mstart,mend,t);
      Test_Complete(BadBanks);
    }
    
    if(in=='T') {
      t++;
      if(t>=(sizeof(test)/sizeof(struct test_t))) t=0;
      Clear_Status(1);
    }
  }
  #ifndef CART
  while (in!='Q');
  #else
  while (1);
  #endif
  
  Clear_Status(1);
  //  rebootafterexit();
}
#endif

void Test_Complete(unsigned char bb)
{
  Clear_Status(0);
  if(bb) {
    sprintf(log(),"Test Completed - %d Banks had errors",bb);
    Set_Status("***** FAIL *****",0);
  } else {
    sprintf(log(),"Test Completed - No Errors"); 
    Set_Status("***** PASS *****",0);
    SetBorderColor(COL_GREEN);
  }
  updatelog();
}

int Start_Test(unsigned int mstart, unsigned int mend, unsigned char t)
{
  unsigned int bank;
  unsigned int BB=0;

  for (bank=mstart;bank<mend;bank++) {
    Set_Status("Testing %02X",bank);
    NErr=0;
    test[t].run(bank,test[t].pat,test[t].pat_len);
    
    if(NErr) {
      BB++;
      SetBorderColor(COL_RED);
      PrintErrorLog(NErr,bank);
    }
  }

#if 0
  if(test[t].flags&TFLAG_AAD) {
	for (bank=mstart;bank<mend;bank++) {
	  unsigned int ad;
	  Bank_Set(bank);
	      //	      Set_Pat(test[t].pat[p]);
	      Set_Status("Writing %02X",bank);
	      AAD_Write();
	      /*    ad=0;
	      do {
		LPOKEW(bank,ad,(ad>>1)+0x11);
		ad+=2;
		} while (ad!=0);*/
	  }
	  
	  for (bank=mstart;bank<mend;bank++) {
	    unsigned int ad;
	    unsigned char r,e;
	      Bank_Set(bank);
	      //	      Set_Pat(test[t].pat[p]);
	      Set_Status("Reading %02X",bank);
	      NErr=0;
	      	      AAD_Read();
	      if(NErr) {
		BadBanks++;
		SetBorderColor(COL_RED);
		PrintErrorLog(NErr,bank);
		//	  cgetc();
	      }
	  }
      }
#endif
  
  return BB;
}

/*****************************************************************************/
/*                                Memory Tests                               */
/*****************************************************************************/

void Set_Pat(tword_t pat)
{
  DPat=pat;
  gotoxy (STATUS_X+11,STATUS_Y);
#if TWORD_SIZE==2
  printf("%04X",pat);
#else
  printf(" %02X ",pat);
#endif
}

void SSO(unsigned char bank, const tword_t *pat, unsigned char len) __reentrant
{
  unsigned char i;
  
  Bank_Set(bank);
  for(i=0;i<len;i++) {
    Set_Pat(pat[i]);
    Bank_Write();
#ifdef TEST
    if (bank==ERR_BANK) LPOKE(bank,ERR_ADDR,0x5a);
#endif
    Bank_Read();
  }
}

void MATS(unsigned char bank, const tword_t *pat, unsigned char len) __reentrant
{
  unsigned char i;
  
  Bank_Set(bank);
  for(i=0;i<len;i++) {
    Set_Pat(pat[i]);
    Bank_Write();
    Set_Pat(~pat[i]);
    Bank_Write();
    Bank_Read();
    Bank_Read();
    Set_Pat(pat[i]);
    Bank_Write();
    Bank_Read();
#ifdef TEST
    if(bank==ERR_BANK && i==1) {
      LPOKE(ERR_BANK,ERR_ADDR,0x54);
      LPOKE(ERR_BANK,ERR_ADDR+1,0x15);
    }
#endif
    Bank_Read();
  }  
}

void AAD(unsigned char bank, const tword_t *pat, unsigned char len) __reentrant
{
  (void)pat;
  (void)len;
  
  Bank_Set(bank);
  AAD_Write();
#ifdef TEST
  if(bank==ERR_BANK) LPOKE(bank,ERR_ADDR,0x54);
  //if(bank==ERR_BANK) LPOKE(bank,ERR_ADDR+0x30,0x9a);
  //    if(bank==ERR_BANK) LPOKE(bank,ERR_ADDR+0x60,0xf1);
  //  if(bank==ERR_BANK+1) LPOKE(bank,ERR_ADDR,0xAF);
#endif
  AAD_Read();
}

/*****************************************************************************/
/*                                Mem Autosize                               */
/*****************************************************************************/

static unsigned int Mem_Detect(unsigned char mstart)
{
  unsigned int b;
  unsigned char v;
  unsigned char r;
  
  for(b=MAX_BANK; b>mstart; b--) {
    v=b^0xff;
    LPOKE(b,0,v);
    //    printf("lpoke %02x:%04x %02x\n",b,0,v);
  }

  v=b^0xff;
  if(v==0xff) v=0x5a; //fix first bank georam detection
  LPOKE(b,0,v);
  //  printf("lpoke %02x:%04x %02x\n",b,0,v);
  
  
  for(b=mstart;b<=MAX_BANK;b++) {
    v=b^0xff;
    if(v==0xff) v=0x5a;
    r=LPEEK(b,0);
    //    printf("lpeek %02x %04x = %02x Exp:%02x\n",b,0,r,v);
    if(r!=v) { break; }
  }
#ifdef TEST
  return 0x20;
#endif
  //  cgetc();
  return b;
}

#ifdef DIAG
static unsigned char Mem_Check_Size(unsigned char size)
{
#if defined (__APPLE2ENH__)
  if(size==4) {
    if((mend-mstart)!=(size<<4))
      return 4;
  } else {
    if(mend!=0x80)
      return 8;
  }
#else
  if(size==5) {
    if(mend!=0x20)
      return 5;
  } else if(size==1) {
    if(mend!=0x40)
      return 1;
  } else {
    if(mend!=0x100)
      return 4;
  }
#endif
  return 0;
}
#endif

/*****************************************************************************/
/*                                Log Functions                              */
/*****************************************************************************/

char *log(void)
{
  char *temp;
  temp=log_data[log_put];
  log_put++;
  if(log_put>=LOG_LINES) log_put=0;
  return temp;
}

void updatelog(void)
{
  unsigned char i, j, line;

  line=log_put;
  for(i=0;i<LOG_LINES;i++) {
    for(j=strlen(log_data[line]);j<LOG_LEN;j++)
	  log_data[line][j]=' ';

    // truncate log lines to log window width
    log_data[line][SCR_WIDTH-LOG_X*2]=0;

    gotoxy(LOG_X,i+22-LOG_LINES);
    printf(log_data[line]);
    line++;
    if(line>=LOG_LINES) line=0;
  }
}

void PrintErrorLog(unsigned int err, unsigned char bank)
{
  unsigned char i;
  tword_t fbit;
  unsigned char fb;
  
  if(err>16) err=16;
  
  for (i=0;i<err;i++) {
    fbit=(err_list[i].expected^err_list[i].read1)|(err_list[i].expected^err_list[i].read2)|
         (err_list[i].expected^err_list[i].read3)|(err_list[i].expected^err_list[i].read4);

#if TWORD_SIZE==2
    //    fbit= (fbit&0xFF)|((fbit>>8)&0xFF);
    // the above does not work (seems a cc65 issue)
    fb= ((fbit>>8)&0xFF)|(fbit&0xFF);
#else
    fb=fbit;
#endif
    
#if SCR_WIDTH==40
    sprintf(log(),
#if TWORD_SIZE==2
	    "%02X/%04X %04X %04X %04X %04X %04X %02X",
#else
	    "%02X/%04X %02X %02X %02X %02X %02X %02X",
#endif	    
	    bank, err_list[i].addr, err_list[i].expected, err_list[i].read1, err_list[i].read2,
	    err_list[i].read3, err_list[i].read4, fb);
#else
    
    sprintf(log(),
#if TWORD_SIZE==2
	    "bank:%02X adr:%04X exp:%04X read1:%04X read2:%04X read3:%04X read4:%04X F:%02X",
#else
	    "bank:%02X adr:%04X exp:%02X read1:%02X read2:%02X read3:%02X read4:%02X F:%02X",
#endif	    	    
	    bank, err_list[i].addr, err_list[i].expected, err_list[i].read1, err_list[i].read2,
	    err_list[i].read3, err_list[i].read4, fb);
#endif
  }
  updatelog();
}

/*****************************************************************************/
/*                                Screen Utilities                           */
/*****************************************************************************/

void Main_Menu(void)
{
  clrscr();

#if SCR_WIDTH==40
  solidframetitle(0,0,SCR_WIDTH,"GMT V" VER " - http://gglabs.us");
  nicetextframexy(0,2,(SCR_WIDTH/2),17-LOG_LINES,"Test Config");
  nicetextframexy(SCR_WIDTH/2,2,(SCR_WIDTH/2),17-LOG_LINES,"System Info");  
  nicetextframexy(0,20-LOG_LINES,SCR_WIDTH,LOG_LINES+3,"Log");
#else
  nicetextframexy(0,0,SCR_WIDTH,24,"GMT V" VER " - http://gglabs.us");
  nicetextframexy(1,2,(SCR_WIDTH/2-1),17-LOG_LINES,"Test Config");
  nicetextframexy(SCR_WIDTH/2,2,(SCR_WIDTH/2-1),17-LOG_LINES,"System Info");
  nicetextframexy(1,20-LOG_LINES,SCR_WIDTH-2,LOG_LINES+3,"Log");
#endif
  
  gotoxy(SCR_WIDTH/2+2,4);
  printf(EXTRA_INFO ": %s",rom_ver);
  gotoxy(SCR_WIDTH/2+2,5);
  printf("Exp Mem: %dKB",(mend-mstart)<<BANK_SHIFT);
  gotoxy(SCR_WIDTH/2+2,6);
  printf("Start:%02X  End:%02X",mstart,mend-1);

#if SCR_WIDTH==40
  sprintf(log(),"GMT V%s - GGLABS Memory Test",VER);
  sprintf(log(),"A modern " MEM_TYPE " memory test   ");
  sprintf(log(),"Copyright (c) 2017-2022");
  sprintf(log(),"Gabriele Gorla" EXTRA_AUTHORS);
  sprintf(log(),"");  
  sprintf(log(),"This program is free software: you");
  sprintf(log(),"can redistribute it and/or modify it");
  sprintf(log(),"under the terms of the GNU GPL as");
  sprintf(log(),"published by the FSF, either V3, or");
  sprintf(log(),"(at your option) any later version.");
#else
  sprintf(log(),"GGLABS Memory Test V%s - http://gglabs.us",VER);
  sprintf(log(),"A modern " MEM_TYPE " memory test optimized for coverage and speed");
  sprintf(log(),"Partially based on test algorithms by A.J. van de Goor");
  sprintf(log(),"Copyright (c) 2017-2022 Gabriele Gorla" EXTRA_AUTHORS);
  sprintf(log(),"");  
  sprintf(log(),"This program is free software: you can redistribute it and/or modify");
  sprintf(log(),"it under the terms of the GNU General Public License as published by");
  sprintf(log(),"the Free Software Foundation, either version 3 of the License, or");
  sprintf(log(),"(at your option) any later version.");
#endif
  
  updatelog();  
}

#ifdef DIAG
static void Banner(unsigned char bb)
{
  sprintf(log(),""); 
  if(bb) {
    sprintf(log()," *** *** *** *   ");
    sprintf(log()," *   * *  *  *   ");
    sprintf(log()," **  ***  *  *   ");
    sprintf(log()," *   * *  *  *   ");
    sprintf(log()," *   * * *** *** ");  
  } else {
    sprintf(log()," *** *** *** *** ");
    sprintf(log()," * * * * *   *   ");
    sprintf(log()," *** *** *** *** ");
    sprintf(log()," *   * *   *   * ");
    sprintf(log()," *   * * *** *** ");
  }
  sprintf(log(),""); 
}
#endif

void Clear_Status(unsigned char b)
{
  if(b) SetBorderColor(COL_MED_BLUE);
  Set_Status("                ",0);
}

void Set_Status(char *format, unsigned char b)
{
  gotoxy(STATUS_X, STATUS_Y);
  printf(format,b);
}

void fatalframe(char *msg)
{
  clrscr();
  nicetextframexy(FATAL_X,5,40,8,"GGLABS MEMORY TEST V" VER );
  gotoxy(FATAL_X+(40-strlen(msg))/2,8);
  cputs(msg);
  gotoxy(FATAL_X+7,10);
  cputs("Press any key to continue");
  cgetc();
}

void nicetextframexy(unsigned char x,unsigned char y,unsigned char w,unsigned char h,char * title)
{
  textframexy(x,y,w,h,TEXTFRAME_TALL);
  //niceframetitle(x,y,w,title);
  solidframetitle(x+1,y,w-2,title);  
}

#if 0
void niceframetitle(unsigned char x, unsigned char y, unsigned char w, char * title)
{
  unsigned char i;
  gotoxy(x,y);
  /*
  printf(INV_ON MTEXT_ON "Z\\");
  for(i=0;i<(w-4);i++) printf("G");
  printf("\\_" INV_OFF MTEXT_OFF );
  gotoxy (x+ (w - strlen (title) -4) / 2, y);
  printf(INV_ON MTEXT_ON "\\" MTEXT_OFF " %s " MTEXT_ON "\\" MTEXT_OFF INV_OFF ,title);
  */
  printf(INV_ON);
  for(i=0;i<(w-4);i++) printf("=");
  printf(INV_OFF);
  gotoxy (x+ (w - strlen (title) -4) / 2, y);
  printf(INV_ON " %s " INV_OFF ,title);
}
#endif

void solidframetitle(unsigned char x, unsigned char y, unsigned char w, char * title)
{
  unsigned char i;
  gotoxy(x,y);
  printf(INV_ON MTEXT_ON);
  for(i=0;i<w;i++) putchar(' ');
  printf(INV_OFF MTEXT_OFF );
  gotoxy (x+ (w - strlen (title) -2 ) / 2, y);
  printf(INV_ON " %s " INV_OFF ,title);
}
