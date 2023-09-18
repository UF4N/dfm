/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "global.h"
#include "windowposition.h"
#include "positiontext.h"
#include "iwc.h"
#include <stdlib.h>

void SetPosition(Window win,Window movewin,int ax,int ay,int sx,int sy) 
{
  int x=0,y=0;
  unsigned int width=0,height=0;
  unsigned int border,depth;
  int gax,gay;
  int gsx,gsy;
  div_t p;
  XEvent ev;
  Window root; 

  XGetGeometry(GetDisplay(),XDefaultRootWindow(GetDisplay()),&root,&x,&y,&width,&height,&border,&depth);
  if ((ax+sx/2)<0 ||
      (ax+sx/2)>width ||
      (ay+sy/2)<0 ||
      (ay+sy/2)>height) {
    ax=ax+sx/2;
    ay=ay+sy/2;
    p=div(ax,width);
    if (ax<0)
      ax=ax-(p.quot-1)*width;
    else
      ax=p.rem;
    p=div(ay,height);
    if (ay<0)
      ay=ay-(p.quot-1)*height;
    else
      ay=p.rem;
    ax=ax-sx/2;
    ay=ay-sy/2;
  }
  do {} while (GetGreatestParent(win)==win);
  if (movewin!=0) {
    do {
      XNextEvent(GetDisplay(),&ev);  
    } while(ev.type!=MapNotify);
    XMoveResizeWindow(GetDisplay(),win,ax,ay,sx,sy+1);
  } else
    XMoveResizeWindow(GetDisplay(),win,ax,ay,sx,sy);
  if (movewin!=0) {
    do {
      XNextEvent(GetDisplay(),&ev);  
    } while(ev.type!=ConfigureNotify);
  } else {
    do {
      XFlush(GetDisplay());
      XSync(GetDisplay(),False);
      XGetGeometry(GetDisplay(),GetGreatestParent(win),&root,&x,&y,&width,&height,&border,&depth);
    } while (ax==x && ay==y && sx==width && sy==height);
    do {
      XFlush(GetDisplay());
      XSync(GetDisplay(),False);
      gax=x;gay=y;gsx=width;gsy=height;
      XGetGeometry(GetDisplay(),GetGreatestParent(win),
		   &root,&x,&y,&width,&height,&border,&depth);
    } while (gax!=x || gay!=y || gsx!=width || gsy!=height);
    system("");
  }
  XFlush(GetDisplay());
  XSync(GetDisplay(),False);
  XTranslateCoordinates(GetDisplay(),win,XDefaultRootWindow(GetDisplay()),
			0,0,&x,&y,&root);
  gax=ax-(x-ax);
  gay=ay-(y-ay);
  XMoveResizeWindow(GetDisplay(),win,gax,gay,sx,sy);
  if (movewin!=0)
    XMoveResizeWindow(GetDisplay(),movewin,0,0,sx,sy);
}

int SaveNewWindowPosition()
{
  char *pos1;
  char *pos2;
  char *pos3;
  char *dummy1,*dummy2;
  XWindowAttributes rootatr;
  int x1,y1;
  unsigned int width1,height1;
  int x=0,y=0;
  unsigned int width=0,height=0;
  unsigned int border,depth;
  Window root;
  XEvent ev;
  do {
    if (XEventsQueued(GetDisplay(),QueuedAfterReading)>0) {
      XPeekEvent(GetDisplay(),&ev);
      if (ev.type==ConfigureNotify || ev.type==NoExpose)
        XNextEvent(GetDisplay(),&ev);
    }
  } while(XEventsQueued(GetDisplay(),QueuedAfterReading)>0 && (ev.type==ConfigureNotify || ev.type==NoExpose));
  do {
    XSync(GetDisplay(),False);
    x1=x;y1=y;width1=width;height1=height;
    XGetGeometry(GetDisplay(),GetGreatestParent(GetWindow()),
    	         &root,&x,&y,&width,&height,&border,&depth);
  } while (x!=x1 || y!=y1 || width!=width1 || height!=height1);
  XGetGeometry(GetDisplay(),GetWindow(),
	       &root,&x,&y,&width,&height,&border,&depth);
  XTranslateCoordinates(GetDisplay(),GetWindow(),XDefaultRootWindow(GetDisplay()),
			0,0,&x,&y,&root);
  XGetWindowAttributes(GetDisplay(),GetWindow(),&rootatr);
  if (rootatr.map_state!=IsUnmapped) {
    GetFileInfoFromDesktop(GetWindowName(),&pos3,&dummy1,&dummy2);
    if (pos3==NULL)
      SetPositionToText(&pos1,x,y,' ');
    else
      SetPositionToText(&pos1,x,y,pos3[0]);
    SetPositionToText(&pos2,width,height,' ');
    SendFileInfoToDesktop(GetWindowName(),pos1,NULL,pos2);
    free(pos1);
    free(pos2);
    free(pos3);
    free(dummy1);
    free(dummy2);
    return True; /*calcmovewindowsize nessesary*/
  } else
    return False;
}

