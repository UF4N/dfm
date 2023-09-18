/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <Xlib.h>
#include <Xutil.h>
#include <malloc.h>
#include "global.h"
#include "color.h"

void InitColors()
{
  Colormap cmap;
  XColor pixs[MAXCOLORS];
  XColor pix;
  unsigned long a,b;
  if (XDefaultVisual(GetDisplay(),XDefaultScreen(GetDisplay()))->class==PseudoColor) {
    cmap=XDefaultColormap(GetDisplay(),XDefaultScreen(GetDisplay()));
    a=0;
    do {
      b|=(((a>>0) & 1)^((a>>3) & 1 )^((a>>6) & 1)) << 0 ;
      b =(((a>>0) & 1)^((a>>3) & 1 ))              << 1 ;
      b|=(((a>>0) & 1))                            << 2 ;
      b|=(((a>>1) & 1)^((a>>4) & 1 )^((a>>7) & 1)) << 3 ;
      b|=(((a>>1) & 1)^((a>>4) & 1 ))              << 4 ;
      b|=(((a>>1) & 1))                            << 5 ;
      b|=(((a>>2) & 1)^((a>>5) & 1 )^((a>>8) & 1)) << 6 ;
      b|=(((a>>2) & 1)^((a>>5) & 1 ))              << 7 ;
      b|=(((a>>2) & 1))                            << 8 ;
      pix.red=9362*(448&b)/64;
      pix.green=9362*(56&b)/8;
      pix.blue=9362*(7&b);
      if(XAllocColor(GetDisplay(),cmap,&pix)==0)
	a=255;
      a++;
    } while (pix.pixel<MAXCOLORS && a!=256);
    if (a<256)
      XFreeColors(GetDisplay(),cmap,&pix.pixel,1,0);
    for (a=0;a<MAXCOLORS;a++) 
      pixs[a].pixel=a;
    XQueryColors(GetDisplay(),cmap,pixs,MAXCOLORS);
    for (a=0;a<MAXCOLORS;a++)
      XAllocColor(GetDisplay(),cmap,&pixs[a]);
  }
}

unsigned long GetBestColor(int r,int g,int b)
{    
  Colormap cmap;
  XColor backcolor;
  static XColor *pixcolors=NULL;
  long difference,localdifference;
  int i;
  unsigned long bg=0;
  backcolor.red=r;
  backcolor.green=g;
  backcolor.blue=b;
  cmap=XDefaultColormap(GetDisplay(),XDefaultScreen(GetDisplay()));
  
  if (XDefaultVisual(GetDisplay(),XDefaultScreen(GetDisplay()))->class!=PseudoColor && 
      XAllocColor(GetDisplay(),cmap,&backcolor)!=0)
    bg=backcolor.pixel;
  else {
    if (pixcolors==NULL) {
      pixcolors=malloc(MAXCOLORS*sizeof(XColor));
      for (i=0;i<MAXCOLORS;i++) 
	pixcolors[i].pixel=(unsigned long)i;
      XQueryColors(GetDisplay(),cmap,pixcolors,MAXCOLORS);
    }
    difference=65536*3;
    for (i=0;i<MAXCOLORS;i++) {
      localdifference=labs((long)pixcolors[i].red-(long)backcolor.red)+
		      labs((long)pixcolors[i].green-(long)backcolor.green)+
		      labs((long)pixcolors[i].blue-(long)backcolor.blue);      
      if (localdifference<difference) {
	difference=localdifference;
	bg=(unsigned long)i;
      }
    }
  }
  return bg;
}
