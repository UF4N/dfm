/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "color.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "global.h"
#include "xvpic.h"

unsigned long* GetXVColors()
{
  unsigned long* xvp;
  int a;
  xvp=malloc(256*sizeof(unsigned long));
  for (a=0;a<256;a++)
    xvp[a]=GetBestColor(9362*(224&a)/32,9362*(28&a)/4,21845*(3&a));
  return xvp;
}

void LoadXVPIC(char *iconfile,Pixmap *backgroundpixmap)
{
  FILE *ficon;
  int a,b,width,height,x,y;
  char line[90];
  static unsigned long *xvpixels;
  static int buildcolormap=True;

  if (*backgroundpixmap==0 && (ficon=fopen(iconfile,"r"))!=NULL) {
      a=fgetc(ficon);
      b=fgetc(ficon);
      do {
        fscanf(ficon,"%s",line);
      } while(strncmp("#END_OF_COMMENTS",line,strlen("#END_OF_COMMENTS"))!=0);
      fscanf(ficon,"%d %d %d",&width,&height,&a);
      fgetc(ficon);
      *backgroundpixmap=XCreatePixmap(GetDisplay(),GetWindow(),width,height,
				      XDefaultDepth(GetDisplay(),0));
      if (buildcolormap==True) {
        xvpixels=GetXVColors();
	buildcolormap=False;
      }
      for (y=0;y<height;y++)
	for (x=0;x<width;x++) {
	  a=fgetc(ficon); 
	  XSetForeground(GetDisplay(),GetGC(),xvpixels[a]);
	  XDrawPoint(GetDisplay(),*backgroundpixmap,GetGC(),x,y);
	}
	fclose(ficon);
  }
}
