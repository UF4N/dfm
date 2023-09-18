/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef WINDOWPOSITION_H
#define WINDOWPOSITION_H

#include <Xlib.h>

void SetPosition(Window win,Window movewin,int ax,int ay,int sx,int sy);

int SaveNewWindowPosition();

#endif
