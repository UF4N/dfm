/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "global.h"
#include "color.h"
#include <malloc.h>
#include <string.h>

Display *GlobalDisplay;
Window GlobalWindow;
Window GlobalDetailWindow;
Window GlobalMoveWindow;
char *GlobalWindowName=NULL;
char *GlobalPath=NULL;
char *fontname=NULL;
int kindofwindow;
int isdesktop;
int hidden;
long timer;
Cursor midcur,waitcur;
int sr1,sg1,sb1;
int sr2,sg2,sb2;
int sr3,sg3,sb3;
int sr4,sg4,sb4;
unsigned long f1,f2,f3,f4;
GC gc;

void SetGC(GC a)
{
  gc=a;
}

GC GetGC()
{
  return(gc);
}

void SetCursors(Cursor a,Cursor b)
{
  midcur=a;
  waitcur=b;
}

Cursor GetMidCursor()
{
  return(midcur);
}

Cursor GetWaitCursor()
{
  return(waitcur);
}

char* GetFontName()
{
  if (fontname==NULL)
    return("-*-lucida-medium-r-*-*-12-*-*-*-*-*-*-*");
  else 
    return(fontname);
  /* return("-*-helvetica-medium-r-*-*-13-*-*-*-*-*-*-*"); */
}

void SetFontName(char* name)
{
  free(fontname);
  fontname=name;
}

void SetShowHidden(int i)
{
  hidden=i;
}

int GetShowHidden()
{
  return(hidden);
}

void SetTimer(long i)
{
  timer=i;
}

long GetTimer()
{
  return(timer);
}

void SetIsDesktop(int i)
{
  isdesktop=i;
}

int GetIsDesktop()
{
  return(isdesktop);
}

void SetKindOfWindow(int kind)
{
  kindofwindow=kind;
}

int GetKindOfWindow()
{
  return(kindofwindow);
}

Display *GetDisplay()
{
  return(GlobalDisplay);
}

void SetDisplay(Display *display)
{
  GlobalDisplay=display;
}

Window GetWindow()
{
  return(GlobalWindow);
}

void SetWindow(Window window)
{
  GlobalWindow=window;
}

Window GetDetailWindow()
{
  return(GlobalDetailWindow);
}

void SetDetailWindow(Window window)
{
  GlobalDetailWindow=window;
}

Window GetMoveWindow()
{
  return(GlobalMoveWindow);
}

void SetMoveWindow(Window window)
{
  GlobalMoveWindow=window;
}

char *GetWindowName()
{
  return(GlobalWindowName);
}

void SetWindowName(char *name)
{
  if (GlobalWindowName==NULL)
    free(GlobalWindowName);
  GlobalWindowName=malloc(strlen(name)+1);
  strcpy(GlobalWindowName,name);
}

int IsSubDesktop()
{
  if (strncmp("~/.dfmdesk/",GlobalPath,strlen("~/.dfmdesk/"))==0)
    return(True);
  else
    return(False);
}

char* GetPath()
{
  return(GlobalPath);
}

void SetPath(char *path)
{
  if (GlobalPath==NULL)
    free(GlobalPath);
  GlobalPath=malloc(strlen(path)+1);
  strcpy(GlobalPath,path);
}

void SetColors(int r1,int g1,int b1,
	       int r2,int g2,int b2,
	       int r3,int g3,int b3,
	       int r4,int g4,int b4)
{
  sr1=r1;sg1=g1;sb1=b1;
  sr2=r2;sg2=g2;sb2=b2;
  sr3=r3;sg3=g3;sb3=b3;
  sr4=r4;sg4=g4;sb4=b4;
  f1=GetBestColor(sr1*257,sg1*257,sb1*257);
  f2=GetBestColor(sr2*257,sg2*257,sb2*257);
  f3=GetBestColor(sr3*257,sg3*257,sb3*257);
  f4=GetBestColor(sr4*257,sg4*257,sb4*257);
}

void GetColorsRGB(int *r1,int *g1,int *b1,
		  int *r2,int *g2,int *b2,
		  int *r3,int *g3,int *b3,
		  int *r4,int *g4,int *b4)
{
  *r1=sr1;*g1=sg1;*b1=sb1;
  *r2=sr2;*g2=sg2;*b2=sb2;
  *r3=sr3;*g3=sg3;*b3=sb3;
  *r4=sr4;*g4=sg4;*b4=sb4;
}

unsigned long GetFGColor()
{
  if (GetIsDesktop()==True) {
    return(f1);
  } else {
    return(f3);
  }
}

unsigned long GetBGColor()
{
  if (GetIsDesktop()==True) {
    return(f2);
  } else {
    return(f4);
  }
}

