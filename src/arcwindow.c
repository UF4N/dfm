/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "arcwindow.h"
#include "global.h"
#include "freeze.h"
#include "color.h"
#include "iconmanager.h"

void ArcWindow(Window w)
{
  int x,y,x2,y2;
  unsigned int width,height;
  unsigned depth;
  Window root;
  unsigned int dummy;
  XGetGeometry(GetDisplay(),w,&root,&x,&y,&width,&height,&dummy,&depth);
  if (GetIsDesktop()==True) {
    if (w==GetMoveWindow()) {
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
       XDrawArc(GetDisplay(),XDefaultRootWindow(GetDisplay()),GetGC(),0,0,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
       XDrawArc(GetDisplay(),XDefaultRootWindow(GetDisplay()),GetGC(),1,1,width-2,height-2,0,360*64);
     } else {
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
       XDrawArc(GetDisplay(),w,GetGC(),0,0,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
       XDrawArc(GetDisplay(),w,GetGC(),1,1,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
       XDrawArc(GetDisplay(),XDefaultRootWindow(GetDisplay()),GetGC(),x,y,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
       XDrawArc(GetDisplay(),XDefaultRootWindow(GetDisplay()),GetGC(),x+1,y+1,width-2,height-2,0,360*64);
    }
  } else {
    XSetSubwindowMode(GetDisplay(),GetGC(),IncludeInferiors);
    if (w==GetMoveWindow()) {
       XGetGeometry(GetDisplay(),GetWindow(),&root,&x2,&y2,&width,&height,&dummy,&depth);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
       XDrawArc(GetDisplay(),GetWindow(),GetGC(),0,0,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
       XDrawArc(GetDisplay(),GetWindow(),GetGC(),1,1,width-2,height-2,0,360*64);
     } else {
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
       XDrawArc(GetDisplay(),GetMoveWindow(),GetGC(),x,y,width-2,height-2,0,360*64);
       XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
       XDrawArc(GetDisplay(),GetMoveWindow(),GetGC(),x+1,y+1,width-2,height-2,0,360*64); 
     }
     XSetSubwindowMode(GetDisplay(),GetGC(),ClipByChildren);
  }
}

void DearcWindow(Window w)
{
  struct item *direntry;
  if (GetIsDesktop()==False)
    CleanupWindow();
  else {
    XClearWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()));
    if (w!=GetMoveWindow()) {
      direntry=GetDirentryFromWindow(w);
      if (direntry!=NULL) 
        XClearWindow(GetDisplay(),w);
    }
  }
}   
