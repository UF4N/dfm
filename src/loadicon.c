/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "loadicon.h"
#include "xvpic.h"
#include "iwc.h"
#include "pixmap.h"
#include "global.h"
#include <stdio.h>


void loadicon(Pixmap *backgroundpixmap,Pixmap *shapepixmap,char *iconfile,int *isxvpic)
{
  FILE *ficon;
  int a,b;

  *backgroundpixmap=0;
  *shapepixmap=0;
  *isxvpic=False;  
  GetIconPixmap(iconfile,backgroundpixmap,shapepixmap);
  if (*backgroundpixmap==0 && (ficon=fopen(iconfile,"r"))!=NULL) {
    a=fgetc(ficon);
    b=fgetc(ficon);
    if (a==80 && b==55) {
      *isxvpic=True;
      LoadXVPIC(iconfile,backgroundpixmap);
    } else {
      LoadPixmap(iconfile,backgroundpixmap,shapepixmap);
      if (*backgroundpixmap!=None) 
	SetIconPixmap(iconfile,*backgroundpixmap,*shapepixmap);
    }
    fclose(ficon);
  }
}




