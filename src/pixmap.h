/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef PIXMAP_H
#define PIXMAP_H

#include <Xlib.h>
#include <Xutil.h>

int LoadPixmap(char *filename,Pixmap *p,Pixmap *s);

int DataPixmap(char **data,Pixmap *p,Pixmap *s);

#endif
