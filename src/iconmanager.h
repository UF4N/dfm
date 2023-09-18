/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <Xlib.h>
#include <Xutil.h>
#include <cursorfont.h>

#define TYPE      0
#define NAME      1
#define EXTENSION 2
#define DATEASC   3
#define DATEDESC  4
#define SIZEASC   5
#define SIZEDESC  6
#define ARRANGE   7


struct item *GetDirentryFromWindow(Window win);

void NewIcon(char *filename,char *orginal,int x,int y,int iconnr);

void EraseIcon(char *filename);

void MakeWindowIcons();

Window LowerIcons();

void MakeWindowTree();

void SortIcons(int type);

void PlaceIcons();

void UpdateWindow();

void UpdateIcons(int fast);

int HandleTheEvent(XEvent evnt);

void UnsetMarks();

void SetAllMarks();

#endif

