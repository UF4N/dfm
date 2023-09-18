/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef LOADICON_H
#define LOADICON_H

#include <Xlib.h>
#include <Xutil.h>

void loadicon(Pixmap *backgroundpixmap,Pixmap *shapepixmap,char *iconfile,int *isxvpic);

#endif
