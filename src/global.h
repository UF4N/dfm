/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <Xlib.h>

#define NORMAL 0
#define DETAIL 1
#define STRUCTURE 2

void SetTimer(long i);
long GetTimer();

void SetGC(GC a);
GC GetGC();

void SetCursors(Cursor a,Cursor b);
Cursor GetMidCursor();
Cursor GetWaitCursor();

char* GetFontName();
void SetFontName(char* name);

void SetShowHidden(int i);
int GetShowHidden();

void SetIsDesktop(int i);
int GetIsDesktop();

void SetKindOfWindow(int kind);
int GetKindOfWindow();

void SetDisplay(Display *display);
Display *GetDisplay();

Window GetDetailWindow();
void SetDetailWindow(Window window);

Window GetWindow();
void SetWindow(Window window);

Window GetMoveWindow();
void SetMoveWindow(Window window);

char *GetWindowName();
void SetWindowName(char *name);

int IsSubDesktop();

char* GetPath();
void SetPath(char *path);

void SetColors(int r1,int g1,int b1,
	       int r2,int g2,int b2,
	       int r3,int g3,int b3,
	       int r4,int g4,int b4);

void GetColorsRGB(int *r1,int *g1,int *b1,
		  int *r2,int *g2,int *b2,
		  int *r3,int *g3,int *b3,
		  int *r4,int *g4,int *b4);

unsigned long GetFGColor();

unsigned long GetBGColor();


#endif

