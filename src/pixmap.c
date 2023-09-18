/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "pixmap.h"
#include "gif.h"
#include "global.h"
#include "color.h"
#include <malloc.h>
#include <xpm.h>

GC pgc=0,pgcs=0;

#define COLORARRAY 70

void EvaluatePixels(Pixmap *p,Pixmap *s,unsigned long pixel,int npoints,XPoint *points)
{
  if (npoints>0) {
    if (pixel>0xff000000) {
      if (s!=0)
        XDrawPoints(GetDisplay(),*s,pgcs,points,npoints,CoordModeOrigin);
    } else {
      XSetForeground(GetDisplay(),pgc,pixel);
      XDrawPoints(GetDisplay(),*p,pgc,points,npoints,CoordModeOrigin);      
    }
  }
}


void MakePixmapFromXpmImage(XpmImage *I,Pixmap *p,Pixmap *s)
{
  XColor color;
  unsigned long i,x,y;
  unsigned long *pixel;
  int shape;
  XPoint **pointmatrix;
  int *npoints;  

  pixel=malloc(sizeof(unsigned long)*(I->ncolors));
  pointmatrix=malloc(sizeof(XPoint*)*(I->ncolors));
  npoints=malloc(sizeof(int)*I->ncolors);
  shape=False;
  for (i=0;i<(I->ncolors);i++) {
    npoints[i]=0;
    pointmatrix[i]=malloc(sizeof(XPoint)*COLORARRAY);
    if (XParseColor(GetDisplay(),XDefaultColormap(GetDisplay(),XDefaultScreen(GetDisplay())),
		    (I->colorTable)[i].c_color,&color)==0) {
      pixel[i]=0xffffffff;
      shape=True;
    } else
      pixel[i]=GetBestColor(color.red,color.green,color.blue);
  }
  
  *p=XCreatePixmap(GetDisplay(),XDefaultRootWindow(GetDisplay()),I->width,I->height,
		   XDefaultDepth(GetDisplay(),0));
  if (pgc==0)
    pgc=XCreateGC(GetDisplay(),*p,0,0);
  if (s!=NULL && shape==True) {
    *s=XCreatePixmap(GetDisplay(),XDefaultRootWindow(GetDisplay()),I->width,I->height,1);
    if (pgcs==0)
      pgcs=XCreateGC(GetDisplay(),*s,0,0);
    XSetForeground(GetDisplay(),pgcs,1);
    XFillRectangle(GetDisplay(),*s,pgcs,0,0,I->width,I->height);
    XSetForeground(GetDisplay(),pgcs,0);
  } else
    if (s!=NULL)
      *s=0;
  i=0;
  for (y=0;y<(I->height);y++)
    for (x=0;x<(I->width);x++) {
      pointmatrix[I->data[i]][npoints[I->data[i]]].x=x;
      pointmatrix[I->data[i]][npoints[I->data[i]]].y=y;
      npoints[I->data[i]]++;
      if (npoints[I->data[i]]==COLORARRAY) {
	EvaluatePixels(p,s,pixel[I->data[i]],npoints[I->data[i]],pointmatrix[I->data[i]]);
	npoints[I->data[i]]=0;
      }
      i++;
    }
  for (i=0;i<(I->ncolors);i++) {
    EvaluatePixels(p,s,pixel[i],npoints[i],pointmatrix[i]);
    free(pointmatrix[i]);
  }

  free(npoints);
  free(pointmatrix);
  free(pixel);
  /*
    attr.valuemask=XpmExactColors|XpmCloseness;
    attr.exactColors=False;
    attr.closeness=65536;    
    XpmCreatePixmapFromXpmImage(GetDisplay(),XDefaultRootWindow(GetDisplay()),
    I,p,s,&attr);
    */
}

int LoadPixmap(char *filename,Pixmap *p,Pixmap *s)
{
  XpmImage I;
  int ret;
  if (strcmp(".gif",filename+strlen(filename)-4)==0) {
    ret=ReadGifToXpmImage(filename,&I);
  } else
    ret=XpmReadFileToXpmImage(filename,&I,NULL);
  if (ret==0){
    MakePixmapFromXpmImage(&I,p,s);
    XpmFreeXpmImage(&I);
  } else {
    *p=0;
    if (s!=NULL)
      *s=0;
  }
  return (ret);
}

int DataPixmap(char **data,Pixmap *p,Pixmap *s)
{
  XpmImage I;
  int ret;

  ret=XpmCreateXpmImageFromData(data,&I,NULL);
  if (ret==0){
    MakePixmapFromXpmImage(&I,p,s);
    XpmFreeXpmImage(&I);
  } else {
    *p=0;
    if (s!=NULL)
      *s=0;
  }
  return (ret);
}
