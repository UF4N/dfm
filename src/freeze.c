/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "freeze.h"
#include "global.h"
#include <Xlib.h>

Window freezewin=0;

void FreezeWindow()
{
  int x,y;
  unsigned int width,height;
  unsigned int border,depth;
  Window root;
  if (freezewin==0 && GetIsDesktop()==False) {
    XGetGeometry(GetDisplay(),GetWindow(),&root,&x,&y,
	       &width,&height,&border,&depth);
    freezewin=XCreateSimpleWindow(GetDisplay(),GetWindow(),0,0,width,height,0,0,0);
    XSetWindowBackgroundPixmap(GetDisplay(),freezewin,0);
    XMapRaised(GetDisplay(),freezewin);
  }
}

void UnfreezeWindow()
{
  if (freezewin!=0) {
    XDestroyWindow(GetDisplay(),freezewin);
    freezewin=0;
  }
}

int IsFreezed()
{
  if (freezewin==0)
    return False;
  else
    return True;
}

void CleanupWindow()
{
  FreezeWindow();
  UnfreezeWindow();
}
