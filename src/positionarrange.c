/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "positionarrange.h"
#include <malloc.h>
#include <string.h>
#include "global.h"
#include "positiontext.h"
#include "iwc.h"

struct item **itemarray=NULL;
int anzahlarray=0;
int *oldx=NULL;
int *oldy=NULL;
int savex=5;
int savey=5;
int maxheight=0;

int maindesktoparrangetype=0;
int mainax=5;
int mainay=5;
int mainbx=5;
int mainby=5;

void SetNextPosition(int x,int y)
{
    anzahlarray=0;
    savex=x;
    savey=y;
    maxheight=0;
}

void ResetPositionHistory()
{
  anzahlarray=0;
}

void ResetPosition()
{
  XWindowAttributes attr;
  char *position,*b,*c;
  if (GetIsDesktop()==True) {
    XGetWindowAttributes(GetDisplay(),GetMoveWindow(),&attr);
    GetFileInfoFromDesktop(GetWindowName(),&position,&b,&c);
    GetPositionFromText(position,&mainax,&mainay,NULL);
    GetPositionFromText(c,&mainbx,&mainby,NULL);
    if (position[0]=='a') {
      maindesktoparrangetype=1;
      savex=mainax;
      savey=mainay;      
    } else if (position[0]=='b') {
      maindesktoparrangetype=2;
      savex=mainax;
      savey=attr.height-mainby;      
    } else if (position[0]=='c') {
      maindesktoparrangetype=3;
      savex=attr.width-mainbx;      
      savey=mainay;
    } else {
      maindesktoparrangetype=0;
      savex=mainax;
      savey=mainay;
    }
    free(position);free(b);free(c);
  } else { 
    savex=5;
    if (GetKindOfWindow()==STRUCTURE)
      savey=0;
    else
      savey=5;
  }
  maxheight=0;
  anzahlarray=0;
}

void PositionArrange(struct item *direntry,int sx,int sy)
{
  XWindowAttributes attr;
  int x,y,i;
  char *text,*text2;
  char *pos,*a,*b,*c;
  text=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
  strcpy(text,GetPath());
  strcat(text,direntry->filename);
  GetFileInfoFromDesktop(text,&a,&b,&c);
  if (a==NULL || a[0]!='a' || GetKindOfWindow()!=NORMAL) {
    XGetWindowAttributes(GetDisplay(),GetMoveWindow(),&attr);
    if (GetKindOfWindow()==NORMAL) {
      if (GetIsDesktop()==True) {
	if (maindesktoparrangetype==0) {
	anzahlarray++;
	itemarray=realloc(itemarray,sizeof(struct item *) * anzahlarray);
	oldx=realloc(oldx,sizeof(int) * anzahlarray);
	oldy=realloc(oldy,sizeof(int) * anzahlarray);
	if (savex+sx+14 > attr.width-mainbx && savex != mainax) {
	  savex=5;
	  savey=savey+maxheight+10;
	  maxheight=0;
	  anzahlarray=1;
	  itemarray=realloc(itemarray,sizeof(struct item *));
	  oldx=realloc(oldx,sizeof(int));
	  oldy=realloc(oldy,sizeof(int));
	}
	if (sy>maxheight) {
	  for (i=0;i<anzahlarray-1;i++) {
	    oldy[i]=oldy[i]+sy-maxheight;
	    XMoveWindow(GetDisplay(),itemarray[i]->Iconwindow,oldx[i],oldy[i]);
	    text2=malloc(strlen(itemarray[i]->filename)+strlen(GetPath())+1);
	    strcpy(text2,GetPath());
	    strcat(text2,itemarray[i]->filename);
	    SetPositionToText(&pos,oldx[i],oldy[i],' ');
	    SendFileInfoToDesktop(text2,pos,NULL,NULL);
	    free(pos);
	    free(text2);
	  }
	  maxheight=sy;
	}
	x=savex;
	y=savey+maxheight-sy;
	savex=savex+sx+10;
	itemarray[anzahlarray-1]=direntry;
	oldx[anzahlarray-1]=x;
	oldy[anzahlarray-1]=y;
        /**********************alt
	  if (savex+sx+14 > attr.width-mainbx && savex != mainax) {
	    savex=mainax;
	    savey=savey+maxheight+10;
	    maxheight=0;
	  }
	  if (sy>maxheight)
	    maxheight=sy;
	  x=savex;
	  y=savey;
	  savex=savex+sx+10;
	alt ******************/
	} else if (maindesktoparrangetype==2) {
	  if (savex+sx+14 > attr.width-mainbx && savex != mainax) {
	    savex=mainax;
	    savey=savey-maxheight-10;
	    maxheight=0;
	  }
	  if (sy>maxheight)
	    maxheight=sy;
	  x=savex;
	  y=savey-sy;
	  savex=savex+sx+10;
	} else if (maindesktoparrangetype==1) {
	  anzahlarray++;
	  itemarray=realloc(itemarray,sizeof(struct item *) * anzahlarray);
	  oldx=realloc(oldx,sizeof(int) * anzahlarray);
	  oldy=realloc(oldy,sizeof(int) * anzahlarray);
	  if (savey+sy+14 > attr.height-mainby && savey != mainay) {
	    savey=mainay;
	    savex=savex+maxheight+10;
	    maxheight=0;
	    anzahlarray=1;
	    itemarray=realloc(itemarray,sizeof(struct item *));
	    oldx=realloc(oldx,sizeof(int));
	    oldy=realloc(oldy,sizeof(int));
	  }
	  if (sx>maxheight) {
	    for (i=0;i<anzahlarray-1;i++) {
	      oldx[i]=oldx[i]+(sx-maxheight)/2;
	      XMoveWindow(GetDisplay(),itemarray[i]->Iconwindow,oldx[i],oldy[i]);
	      text2=malloc(strlen(itemarray[i]->filename)+strlen(GetPath())+1);
	      strcpy(text2,GetPath());
	      strcat(text2,itemarray[i]->filename);
	      SetPositionToText(&pos,oldx[i],oldy[i],' ');
	      SendFileInfoToDesktop(text2,pos,NULL,NULL);
	      free(pos);
	      free(text2);
	    }
	    maxheight=sx;
	  }
	  x=savex+(maxheight-sx)/2;
	  y=savey;
	  savey=savey+sy+10;
	  itemarray[anzahlarray-1]=direntry;
	  oldx[anzahlarray-1]=x;
	  oldy[anzahlarray-1]=y;
	} else {
	  /*************/
	  anzahlarray++;
	  itemarray=realloc(itemarray,sizeof(struct item *) * anzahlarray);
	  oldx=realloc(oldx,sizeof(int) * anzahlarray);
	  oldy=realloc(oldy,sizeof(int) * anzahlarray);
	  if (savey+sy+14 > attr.height-mainby && savey != mainay) {
	    savey=mainay;
	    savex=savex-maxheight-10;
	    maxheight=0;
	    anzahlarray=1;
	    itemarray=realloc(itemarray,sizeof(struct item *));
	    oldx=realloc(oldx,sizeof(int));
	    oldy=realloc(oldy,sizeof(int));
	  }
	  if (sx>maxheight) {
	    for (i=0;i<anzahlarray-1;i++) {
	      oldx[i]=oldx[i]-(sx-maxheight)/2;
	      XMoveWindow(GetDisplay(),itemarray[i]->Iconwindow,oldx[i],oldy[i]);
	      text2=malloc(strlen(itemarray[i]->filename)+strlen(GetPath())+1);
	      strcpy(text2,GetPath());
	      strcat(text2,itemarray[i]->filename);
	      SetPositionToText(&pos,oldx[i],oldy[i],' ');
	      SendFileInfoToDesktop(text2,pos,NULL,NULL);
	      free(pos);
	      free(text2);
	    }
	    maxheight=sx;
	  }
	  x=savex-sx-(maxheight-sx)/2;
	  y=savey;
	  savey=savey+sy+10;
	  itemarray[anzahlarray-1]=direntry;
	  oldx[anzahlarray-1]=x;
	  oldy[anzahlarray-1]=y;
	  /*************/
	}
	  
      } else {
	anzahlarray++;
	itemarray=realloc(itemarray,sizeof(struct item *) * anzahlarray);
	oldx=realloc(oldx,sizeof(int) * anzahlarray);
	oldy=realloc(oldy,sizeof(int) * anzahlarray);
	if (savex+sx+14 > attr.width && savex != 5) {
	  savex=5;
	  savey=savey+maxheight+10;
	  maxheight=0;
	  anzahlarray=1;
	  itemarray=realloc(itemarray,sizeof(struct item *));
	  oldx=realloc(oldx,sizeof(int));
	  oldy=realloc(oldy,sizeof(int));
	}
	if (sy>maxheight) {
	  for (i=0;i<anzahlarray-1;i++) {
	    oldy[i]=oldy[i]+sy-maxheight;
	    XMoveWindow(GetDisplay(),itemarray[i]->Iconwindow,oldx[i],oldy[i]);
	    text2=malloc(strlen(itemarray[i]->filename)+strlen(GetPath())+1);
	    strcpy(text2,GetPath());
	    strcat(text2,itemarray[i]->filename);
	    SetPositionToText(&pos,oldx[i],oldy[i],' ');
	    SendFileInfoToDesktop(text2,pos,NULL,NULL);
	    free(pos);
	    free(text2);
	  }
	  maxheight=sy;
	}
	x=savex;
	y=savey+maxheight-sy;
	savex=savex+sx+10;
	itemarray[anzahlarray-1]=direntry;
	oldx[anzahlarray-1]=x;
	oldy[anzahlarray-1]=y;
      }
    } else if (GetKindOfWindow()==DETAIL){
      savex=20;
      savey=savey+sy;
      x=savex;
      y=savey;
    } else {
      savex=22;
      for (i=0;i<strlen(direntry->filename);i++) {
	if (direntry->filename[i]=='/') 
	  savex=savex+15;
      }
      savey=savey+sy+2;
      x=savex;
      y=savey;
    }
    if (GetKindOfWindow()==NORMAL) {
      SetPositionToText(&pos,x,y,' ');
      SendFileInfoToDesktop(text,pos,NULL,NULL);
      free(pos);
    } 
    XMoveWindow(GetDisplay(),direntry->Iconwindow,x,y);
  }
  free(a);free(b);free(c);
  free(text);
}  
