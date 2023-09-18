/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <Xlib.h>
#include "detailtitle.h"
#include "global.h"
#include "color.h"

void CreateDetailTitle()
{
    Pixmap backgroundpixmap;
  Font font;
  Window detailwin;
  
    detailwin=XCreateSimpleWindow(GetDisplay(),GetWindow(),20,0,732,20,
				  0,0,0);
    font=XLoadFont(GetDisplay(),"8x13");
    XSetFont(GetDisplay(),GetGC(),font);
    backgroundpixmap=XCreatePixmap(GetDisplay(),detailwin,
				   732,20,XDefaultDepth(GetDisplay(),
							XDefaultScreen(GetDisplay())));
    XSetForeground(GetDisplay(),GetGC(),GetBestColor(0xcf3c,0xcf3c,0xcf3c));
    XFillRectangle(GetDisplay(),backgroundpixmap,GetGC(),0,0,732,20);
    XSetBackground(GetDisplay(),GetGC(),GetBestColor(0xcf3c,0xcf3c,0xcf3c));
    XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),0,12," Size",strlen(" Size"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),0,0,0,19);
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),116,12,"Name",strlen("Name"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),92,0,92,19);
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),304,12,"Owner",strlen("Owner"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),300,0,300,19);
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),376,12,"Group",strlen("Group"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),372,0,372,19);
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),448,12,"Time",strlen("Time"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),444,0,444,19);
    XDrawImageString(GetDisplay(),backgroundpixmap,GetGC(),648,12,"Modeline",strlen("Modeline"));
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),644,0,644,19);    
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),731,0,731,19);    
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),0,19,731,19);    
    XDrawLine(GetDisplay(),backgroundpixmap,GetGC(),0,0,731,0);    
    XSetWindowBackgroundPixmap(GetDisplay(),detailwin,backgroundpixmap);
    XFreePixmap(GetDisplay(),backgroundpixmap);
    XMapRaised(GetDisplay(),detailwin);
    SetDetailWindow(detailwin);
}
