/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "iconmanager.h"
#include "filemanager.h"
#include <unistd.h>
#include "iwc.h"
#include "shell.h"
#include "extension.h"
#include "color.h"
#include "gui.h"
#include "pixmap.h"
#include "arcwindow.h"
#include "positiontext.h"
#include "windowname.h"
#include "global.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "menus.h"
#include "timer.h"
#include "createicon.h"
#include "positionarrange.h"
#include "windowposition.h"
#include <Xlib.h>
#include <keysym.h>
#include "freeze.h"
#include "includepixmaps/slider.h"
#include "includepixmaps/treeplus.xpm"
#include "includepixmaps/treeminus.xpm"
#include "includepixmaps/treelineH.xpm"
#include "includepixmaps/treelineV.xpm"

#include <extensions/shape.h>

struct item *directory=NULL;
struct tree *toptree=NULL;

int grid=0;

Window movexmain,movex;
Window moveymain,movey;
Window quader;
int moveonx=False,moveony=False;
int quaderon=False;

int sorttype;

Pixmap minus=0;
Pixmap minus_shape=0;
Pixmap plus=0;
Pixmap plus_shape=0;
Pixmap lineh=0;
Pixmap linev=0;

void CalcMoveWindowSize()
{
  unsigned int minx,miny;
  int ax,ay;
  int x,y;
  int i;
  unsigned int width,height;
  unsigned int border,depth;
  Window root;
  struct item *direntry;
  Pixmap pixelmap,pixelmap2;
  unsigned int dummy;
  if (GetIsDesktop()==False) {
    XGetGeometry(GetDisplay(),GetMoveWindow(),&root,&ax,&ay,
		 &width,&height,&border,&depth);
    minx=0;
    miny=0;
    direntry=directory;
    
    while (direntry!=NULL) {
      XGetGeometry(GetDisplay(),direntry->Iconwindow,&root,&x,&y,
		   &width,&height,&border,&depth);
      if (minx<x+width) minx=x+width;
      if (miny<y+height) miny=y+height;
      direntry=direntry->NextItem;
    }
    for (i=0;i<3;i++) {
      XGetGeometry(GetDisplay(),GetWindow(),&root,&x,&y,
		   &width,&height,&border,&depth);
      if (moveony==True) width=width-14;
      if (moveonx==True) height=height-14;
      if (miny>height|| ay!=0) { 
	if (moveony==False) {
	  moveymain=XCreateSimpleWindow(GetDisplay(),GetWindow(),
					width,0,14,height,0,
					0,0);
	  XSelectInput(GetDisplay(),moveymain,ButtonReleaseMask|ButtonPressMask
		       |PointerMotionMask);
	  movey=XCreateSimpleWindow(GetDisplay(),moveymain,
				    0,14,14,height-28,0,
				    0,0);
	  XSelectInput(GetDisplay(),movey,ButtonReleaseMask|ButtonPressMask
		       |PointerMotionMask);
          XLowerWindow(GetDisplay(),moveymain);
          XMapWindow(GetDisplay(),moveymain);
          XMapRaised(GetDisplay(),movey);
	  moveony=True;
	}
	XMoveResizeWindow(GetDisplay(),moveymain,width,0,14,height);
      } else {
	if (moveony==True) {
	  XDestroyWindow(GetDisplay(),moveymain);
	  moveony=False;
	}      
      }
      if (minx>width || ax!=0 ) { 
	if (moveonx==False) {
	  movexmain=XCreateSimpleWindow(GetDisplay(),GetWindow(),
					0,height,width,14,0,
					0,0
					);
	  XSelectInput(GetDisplay(),movexmain,ButtonReleaseMask|ButtonPressMask
		       |PointerMotionMask);
	  movex=XCreateSimpleWindow(GetDisplay(),movexmain,
				    0,14,width-28,14,0,
				    0,0);
	  XSelectInput(GetDisplay(),movex,ButtonReleaseMask|ButtonPressMask
		       |PointerMotionMask);
          XLowerWindow(GetDisplay(),movexmain);
          XMapWindow(GetDisplay(),movexmain);
          XMapRaised(GetDisplay(),movex);
	  moveonx=True;
	}
	XMoveResizeWindow(GetDisplay(),movexmain,0,height,width,14);
      } else {
	if (moveonx==True) {
	  XDestroyWindow(GetDisplay(),movexmain);
	  moveonx=False;
	}
      }
    }
    if (moveonx==True || moveony==True) {
      if (quaderon==False) {
	quader=XCreateSimpleWindow(GetDisplay(),GetWindow(),
				   width,height,14,14,0,0,0);
	XSelectInput(GetDisplay(),quader,ButtonReleaseMask|ButtonPressMask
		     |PointerMotionMask);
	DataPixmap(quader_xpm,&pixelmap,NULL);
	XSetWindowBackgroundPixmap(GetDisplay(),quader,pixelmap);
	XFreePixmap(GetDisplay(),pixelmap);
	XMapRaised(GetDisplay(),quader);
	quaderon=True;
      }
      XMoveResizeWindow(GetDisplay(),quader,width,height,14,14);
    } else {
      if (quaderon==True) {
	XDestroyWindow(GetDisplay(),quader);
	quaderon=False;
      }
    }
    if (width-ax>minx) minx=width-ax;
    if (height-ay>miny) miny=height-ay;
    XResizeWindow(GetDisplay(),GetMoveWindow(),minx,miny);
    if (moveony==True) {
      XMoveResizeWindow(GetDisplay(),movey,
			0,
			14+(int)((double)(height-28)*(double)(-ay)/(double)miny),
			14,
			1+(int)((double)(height-28)*(double)height/(double)miny)
			);
    }
    if (moveonx==True) {
      XMoveResizeWindow(GetDisplay(),movex,
			14+(int)((double)(width-28)*(double)(-ax)/(double)minx),
			0,
			1+(int)((double)(width-28)*(double)width
				/(double)minx),
			14
			);
    }
    if (moveony==True) {
      XGetGeometry(GetDisplay(),moveymain,&root,&x,&y,&width,&height,&dummy,&depth);
      pixelmap=XCreatePixmap(GetDisplay(),moveymain,width,height,depth);
      /*top*/
      DataPixmap(moveymaintop_n_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,0,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*middle*/
      DataPixmap(moveymainmiddle_xpm,&pixelmap2,NULL);
      for (i=14;i<height;i=i+14)
	XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,0,i);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*bottom*/
      DataPixmap(moveymainbottom_n_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,0,height-14);
      XFreePixmap(GetDisplay(),pixelmap2);

      XSetWindowBackgroundPixmap(GetDisplay(),moveymain,pixelmap);
      XFreePixmap(GetDisplay(),pixelmap);
      /*slider*/
      XGetGeometry(GetDisplay(),movey,&root,&x,&y,&width,&height,&dummy,&depth);
      pixelmap=XCreatePixmap(GetDisplay(),movey,width,height,depth);
      /*top*/
      DataPixmap(moveytop_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,3,0,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*middle*/
      DataPixmap(moveymiddle_xpm,&pixelmap2,NULL);
      for (i=3;i<height;i=i+14)
	XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,0,i);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*bottom*/
      DataPixmap(moveybottom_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,3,0,height-3);
      XFreePixmap(GetDisplay(),pixelmap2);

      XSetWindowBackgroundPixmap(GetDisplay(),movey,pixelmap);
      XFreePixmap(GetDisplay(),pixelmap);
      XClearWindow(GetDisplay(),moveymain);
      XClearWindow(GetDisplay(),movey);
      if (IsFreezed()==False)
        XRaiseWindow(GetDisplay(),moveymain);
    }
    if (moveonx==True) {
      XGetGeometry(GetDisplay(),movexmain,&root,&x,&y,&width,&height,&dummy,&depth);
      pixelmap=XCreatePixmap(GetDisplay(),movexmain,width,height,depth);
      /*top*/
      DataPixmap(movexmainleft_n_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,0,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*middle*/
      DataPixmap(movexmainmiddle_xpm,&pixelmap2,NULL);
      for (i=14;i<width;i=i+14)
	XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,i,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*bottom*/
      DataPixmap(movexmainright_n_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,width-14,0);
      XFreePixmap(GetDisplay(),pixelmap2);

      XSetWindowBackgroundPixmap(GetDisplay(),movexmain,pixelmap);
      XFreePixmap(GetDisplay(),pixelmap);
      /*slider*/
      XGetGeometry(GetDisplay(),movex,&root,&x,&y,&width,&height,&dummy,&depth);
      pixelmap=XCreatePixmap(GetDisplay(),movex,width,height,depth);
      /*top*/
      DataPixmap(movexleft_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,3,14,0,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*middle*/
      DataPixmap(movexmiddle_xpm,&pixelmap2,NULL);
      for (i=3;i<width;i=i+14)
	XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,14,14,i,0);
      XFreePixmap(GetDisplay(),pixelmap2);
      /*bottom*/
      DataPixmap(movexright_xpm,&pixelmap2,NULL);
      XCopyArea(GetDisplay(),pixelmap2,pixelmap,GetGC(),0,0,3,14,width-3,0);
      XFreePixmap(GetDisplay(),pixelmap2);

      XSetWindowBackgroundPixmap(GetDisplay(),movex,pixelmap);
      XFreePixmap(GetDisplay(),pixelmap);
      XClearWindow(GetDisplay(),movexmain);
      XClearWindow(GetDisplay(),movex);
      if (IsFreezed()==False)
        XRaiseWindow(GetDisplay(),movexmain);
    }
  }
}

void MakeWindowIcons()
{
  struct item *direntry;
  directory=GetDirectory(GetPath());
  SortIcons(0);
  direntry=directory;
  while (direntry!=NULL) {
    CreateIconWithDirentry(direntry,True,False);
    direntry=direntry->NextItem;
  }
  CalcMoveWindowSize();
}

Window LowerIcons()
{
  struct item *direntry;
  Window last;
  XWindowChanges wc;
  unsigned int changemask;
  direntry=directory;
  last=GetWindow();
  while (direntry!=NULL) {
    wc.sibling=last;
    wc.stack_mode=Above;
    changemask=CWStackMode|CWSibling;
    XConfigureWindow(GetDisplay(),direntry->Iconwindow,changemask,&wc);
    last=direntry->Iconwindow;
    direntry=direntry->NextItem;
  }
  return(last);
}

void CreateExpandButton(struct tree *drawtree)
{
    drawtree->Expandbutton=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),
				  1,1,1,1,0,
				  0,
				  GetBestColor(0,0,0)
				  );
    XSelectInput(GetDisplay(),drawtree->Expandbutton,ButtonReleaseMask|ButtonPressMask|PointerMotionMask);
    drawtree->Expandline=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),
				  1,1,1,1,0,
				  0,
				  GetBestColor(0,0,0)
				  );
}

void ArrangeButtons(struct tree *drawtree)
{
  int x,y;
  int xt,yt,ye;
  unsigned int width,height,height2;
  unsigned int border,depth;
  Window root;
  struct tree *linetree;

  if (drawtree!=NULL && drawtree->initalized==True) {
    XGetGeometry(GetDisplay(),drawtree->ThisItem->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    if (drawtree->abletoexpand==False) {
      XMoveResizeWindow(GetDisplay(),drawtree->Expandbutton,
			x-22,y+(1+height)/2,22,2);
      XSetWindowBackgroundPixmap(GetDisplay(),drawtree->Expandbutton,lineh);
      XMapRaised(GetDisplay(),drawtree->Expandbutton);
      XUnmapWindow(GetDisplay(),drawtree->Expandline);
    } else if (drawtree->expanded==True) {
      XMoveResizeWindow(GetDisplay(),drawtree->Expandbutton,
			x-22,y+(height-15)/2,22,15);
      XMapRaised(GetDisplay(),drawtree->Expandbutton);
      linetree=drawtree->subtree;
      ye=0;
      while(linetree!=NULL) {
	XGetGeometry(GetDisplay(),linetree->ThisItem->Iconwindow,&root,&xt,&yt,
		     &width,&height2,&border,&depth);
	if (yt>ye) ye=yt;
	linetree=linetree->nexttree;
      }
      XMoveResizeWindow(GetDisplay(),drawtree->Expandline,
			x-9,y+14+(height-15)/2,2,ye-y-7);	
      XSetWindowBackgroundPixmap(GetDisplay(),drawtree->Expandline,linev);
      XMapRaised(GetDisplay(),drawtree->Expandline);
      ArrangeButtons(drawtree->subtree);
    } else {
      XMoveResizeWindow(GetDisplay(),drawtree->Expandbutton,
			x-22,y+(height-15)/2,22,15);
      XMapRaised(GetDisplay(),drawtree->Expandbutton);
      XUnmapWindow(GetDisplay(),drawtree->Expandline);
    }
    ArrangeButtons(drawtree->nexttree);    
  }
}

struct tree *DeleteTreeRec(struct tree *deltree)
{
  struct tree *treeentry;
  treeentry=deltree->subtree;
  while (treeentry!=NULL) {
    treeentry=DeleteTreeRec(treeentry);
  }
  treeentry=deltree->nexttree;
  if (deltree->initalized==True) {
    XDestroyWindow(GetDisplay(),deltree->ThisItem->Iconwindow);
    XDestroyWindow(GetDisplay(),deltree->Expandbutton);
    XDestroyWindow(GetDisplay(),deltree->Expandline);
  }
  free(deltree->ThisItem->filename);
  free(deltree->ThisItem);
  free(deltree);
  return(treeentry);
}


void DeleteTree(struct tree *deltree)
{
  struct tree *treeentry;
  treeentry=deltree->subtree;
  while (treeentry!=NULL && treeentry->initalized==True) {
    deltree->ThisItem->NextItem=treeentry->ThisItem->NextItem;
    if (treeentry->nexttree==NULL)
      treeentry=treeentry->subtree;
    else
      treeentry=treeentry->nexttree;
  }
  treeentry=deltree->subtree;
  while (treeentry!=NULL) {
    treeentry=DeleteTreeRec(treeentry);
  }
}

void MinusTree(struct tree *deltree)
{
  XSetWindowBackgroundPixmap(GetDisplay(),deltree->Expandbutton,plus);
  XShapeCombineMask(GetDisplay(),deltree->Expandbutton, ShapeBounding, 0, 0, 
		    plus_shape, ShapeSet);
  XClearWindow(GetDisplay(),deltree->Expandbutton);
  FreezeWindow();
  DeleteTree(deltree);
  DoTree(deltree);
  PlaceIcons();
  ArrangeButtons(toptree);
  UnfreezeWindow();
}

void CreateTreeIcons(struct tree *exptree)
{
  struct tree *treeentry;
  struct item *saveitem;
  struct item *lastitem=NULL;
  int i=0;
  treeentry=exptree->subtree;
  saveitem=exptree->ThisItem->NextItem;  
  while (treeentry!=NULL) {
    CreateIconWithDirentry(treeentry->ThisItem,True,False);
    if (i==0) {
      i++;
      exptree->ThisItem->NextItem=treeentry->ThisItem;
      lastitem=treeentry->ThisItem;
    } else {
      lastitem->NextItem=treeentry->ThisItem;
      lastitem=treeentry->ThisItem;
    }
    treeentry=treeentry->nexttree;
  }
  lastitem->NextItem=saveitem;
  exptree->expanded=True;
}

void PlusTree(struct tree *exptree)
{
  struct tree *treeentry;
  struct tree *lastentry=NULL;
  int x,y,miny,maxy,heiy;
  unsigned int width,height;
  unsigned int border,depth;
  Window root;

  XSetWindowBackgroundPixmap(GetDisplay(),exptree->Expandbutton,minus);
  XShapeCombineMask(GetDisplay(),exptree->Expandbutton, ShapeBounding, 0, 0, 
		    minus_shape, ShapeSet);
  XClearWindow(GetDisplay(),exptree->Expandbutton);
  FreezeWindow();
  CreateTreeIcons(exptree);
  PlaceIcons();
  ArrangeButtons(toptree);
  treeentry=exptree->subtree;
  while (treeentry!=NULL) {
    lastentry=treeentry;
    treeentry=treeentry->nexttree;
  }
  if (lastentry!=NULL) { /*true*/
    XGetGeometry(GetDisplay(),exptree->ThisItem->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    miny=y;
    XGetGeometry(GetDisplay(),lastentry->ThisItem->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    maxy=y+height+20;
    XGetGeometry(GetDisplay(),GetWindow(),&root,&x,&y,
		 &width,&height,&border,&depth);
    heiy=(int)height;
    XGetGeometry(GetDisplay(),GetMoveWindow(),&root,&x,&y,
		 &width,&height,&border,&depth);
    if (maxy>-y+heiy) {
      if (maxy-miny>heiy)
	XMoveResizeWindow(GetDisplay(),GetMoveWindow(),x,-miny,width,height);
      else
	XMoveResizeWindow(GetDisplay(),GetMoveWindow(),x,-maxy+heiy,width,height);	
    }
    CalcMoveWindowSize();
  }
  if (moveony==True)
    XRaiseWindow(GetDisplay(),moveymain);
  if (moveonx==True)
    XRaiseWindow(GetDisplay(),movexmain);
  UnfreezeWindow();
  XFlush(GetDisplay());
  treeentry=exptree->subtree;
  while (treeentry!=NULL) {
    DoTree(treeentry);
    CreateExpandButton(treeentry);
    XGetGeometry(GetDisplay(),treeentry->ThisItem->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    if (treeentry->abletoexpand==True) {
      XSetWindowBackgroundPixmap(GetDisplay(),treeentry->Expandbutton,plus);
      XShapeCombineMask(GetDisplay(),treeentry->Expandbutton, ShapeBounding, 0, 0, 
			plus_shape, ShapeSet);
      XMoveResizeWindow(GetDisplay(),treeentry->Expandbutton,
			x-22,y+(height-15)/2,22,15);
      XMapRaised(GetDisplay(),treeentry->Expandbutton);
    } else {
      XMoveResizeWindow(GetDisplay(),treeentry->Expandbutton,
			x-22,y+(1+height)/2,22,2);
      XSetWindowBackgroundPixmap(GetDisplay(),treeentry->Expandbutton,lineh);
      XMapRaised(GetDisplay(),treeentry->Expandbutton);
    }
    treeentry=treeentry->nexttree;
  }
}

void MakeWindowTree()
{
  if (plus==0) {
    DataPixmap(treeplus_xpm,&plus,&plus_shape);
    DataPixmap(treeminus_xpm,&minus,&minus_shape);
    DataPixmap(treelineH_xpm,&lineh,NULL);
    DataPixmap(treelineV_xpm,&linev,NULL);
  }
  toptree=GetFirstTree();
  directory=toptree->ThisItem;
  CreateIconWithDirentry(directory,True,False);
  CreateExpandButton(toptree);
  if (toptree->abletoexpand==True) 
    PlusTree(toptree);
  PlaceIcons();
  ArrangeButtons(toptree);
}

void UpdateWindowTreeRec(struct tree *newtree,struct tree *oldtree)
{
  struct tree *treeentry;
  struct tree *treeentry2;
  if (newtree->abletoexpand==True && oldtree->expanded==True) {
    XSetWindowBackgroundPixmap(GetDisplay(),newtree->Expandbutton,minus);
    XShapeCombineMask(GetDisplay(),newtree->Expandbutton, ShapeBounding, 0, 0, 
		      minus_shape, ShapeSet);
    CreateTreeIcons(newtree);
    treeentry=newtree->subtree;
    treeentry2=oldtree->subtree;
    while (treeentry!=NULL) {
      DoTree(treeentry);
      CreateExpandButton(treeentry);
      if (treeentry->abletoexpand==True) {
	XSetWindowBackgroundPixmap(GetDisplay(),treeentry->Expandbutton,plus);
	XShapeCombineMask(GetDisplay(),treeentry->Expandbutton, ShapeBounding, 0, 0, 
			  plus_shape, ShapeSet);
	XMapWindow(GetDisplay(),treeentry->Expandbutton);
      }
      while (treeentry2!=NULL && 
	     strcmp(treeentry->ThisItem->filename,treeentry2->ThisItem->filename)>0)
	treeentry2=treeentry2->nexttree;
      
      if (treeentry2!=NULL &&
	  strcmp(treeentry->ThisItem->filename,treeentry2->ThisItem->filename)==0)
	UpdateWindowTreeRec(treeentry,treeentry2);
      treeentry=treeentry->nexttree;
    }      
  }
}

void UpdateWindowTree()
{
  struct tree *newtree;
  struct item *newdir;

  FreezeWindow();  
  newtree=GetFirstTree();
  newdir=newtree->ThisItem;
  CreateIconWithDirentry(newdir,True,False);
  CreateExpandButton(newtree);
  UpdateWindowTreeRec(newtree,toptree);
  DeleteTree(toptree);
  XDestroyWindow(GetDisplay(),toptree->ThisItem->Iconwindow);
  XDestroyWindow(GetDisplay(),toptree->Expandbutton);
  XDestroyWindow(GetDisplay(),toptree->Expandline);
  free(toptree->ThisItem->filename);
  free(toptree->ThisItem);
  free(toptree);
  toptree=newtree;
  directory=newdir;
  PlaceIcons();
  ArrangeButtons(toptree);
  UnfreezeWindow();  
}

struct item *GetDirentryFromWindow(Window win)
{
  struct item *direntry;
  direntry=directory;
  while (direntry!=NULL && direntry->Iconwindow!=win)
    direntry=direntry->NextItem;
  return(direntry);
}

struct tree *GetTreeentryFromWindow(struct tree *pushtree,Window win)
{
  struct tree *dummytree;
  if (pushtree!=NULL && pushtree->initalized==True) {
    if (pushtree->Expandbutton==win)
      return(pushtree);
    else {
      if (pushtree->expanded==True) {
	dummytree=GetTreeentryFromWindow(pushtree->subtree,win);
	if (dummytree!=NULL)
	  return(dummytree);
      }
      return(GetTreeentryFromWindow(pushtree->nexttree,win));
    }
  } else
    return(NULL);
}
  
void UnsetMarks()
{
  struct item *direntry;
  direntry=directory;
  while (direntry!=NULL) {
    if (direntry->mark==True) {
      direntry->mark=False;
      CreateIconWithDirentry(direntry,False,False);
    }
    direntry=direntry->NextItem;
  }
}

void SetAllMarks()
{
  struct item *direntry;
  if (GetKindOfWindow()!=STRUCTURE) {
    direntry=directory;
    while (direntry!=NULL) {
      if (direntry->mark==False) {
	direntry->mark=True;
	CreateIconWithDirentry(direntry,False,False);
      }
      direntry=direntry->NextItem;
    }
  }
}

void RubberBand(int x,int y)
{
  struct item *direntry;
  int iconx,icony;
  unsigned int width,height;
  int movewinposx,movewinposy;
  unsigned int border,depth;
  Window root;
  XEvent event;
  int sx=x,sy=y;
  int x1,y1,sx1,sy1;
  Pixmap pixmap;
  int buttonup=0;
  Window x1y1x2y1,x1y1x1y2,x1y2x2y2,x2y1x2y2;
  XSetWindowAttributes xswa;
  unsigned long valuemask;
  Window last;
  XWindowChanges wc;
  unsigned int changemask;
  if (GetIsDesktop()==True) {
    XGetGeometry(GetDisplay(),XDefaultRootWindow(GetDisplay()),&root,
		 &movewinposx,&movewinposy,&width,&height,&border,&depth);
    pixmap=XCreatePixmap(GetDisplay(),XDefaultRootWindow(GetDisplay()),8,8,depth);  
    XGetGeometry(GetDisplay(),GetMoveWindow(),&root,
		 &movewinposx,&movewinposy,&width,&height,&border,&depth);
    XGetGeometry(GetDisplay(),GetWindow(),&root,
		 &iconx,&icony,&width,&height,&border,&depth);
  } else {
    XGetGeometry(GetDisplay(),GetMoveWindow(),&root,
	       &movewinposx,&movewinposy,&width,&height,&border,&depth);
  XGetGeometry(GetDisplay(),GetWindow(),&root,
	       &iconx,&icony,&width,&height,&border,&depth);
  pixmap=XCreatePixmap(GetDisplay(),GetMoveWindow(),8,8,depth);  
  }
  XSetBackground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));
  XSetForeground(GetDisplay(),GetGC(),GetBestColor(65535,65535,65535));

  XFillRectangle(GetDisplay(),pixmap,GetGC(),0,0,8,8);
  XSetForeground(GetDisplay(),GetGC(),GetBestColor(0,0,0));
  XDrawRectangle(GetDisplay(),pixmap,GetGC(),0,0,4,4);
  XDrawRectangle(GetDisplay(),pixmap,GetGC(),4,4,4,4);
  if (GetIsDesktop()==True) {
    xswa.override_redirect=True;
    xswa.event_mask=ButtonReleaseMask|ButtonPressMask|PointerMotionMask;
    valuemask=CWOverrideRedirect|CWEventMask;
    x1y1x2y1=XCreateWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0,1,1,
		      0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
    x1y1x1y2=XCreateWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0,1,1,
		      0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
    x1y2x2y2=XCreateWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0,1,1,
		      0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
    x2y1x2y2=XCreateWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0,1,1,
		      0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
   } else {
    x1y1x2y1=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),0,0,1,1,0,0,0);
    x1y1x1y2=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),0,0,1,1,0,0,0);
    x1y2x2y2=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),0,0,1,1,0,0,0);
    x2y1x2y2=XCreateSimpleWindow(GetDisplay(),GetMoveWindow(),0,0,1,1,0,0,0);
  }
  XSetWindowBackgroundPixmap(GetDisplay(),x1y1x2y1,pixmap);
  XSetWindowBackgroundPixmap(GetDisplay(),x1y1x1y2,pixmap);
  XSetWindowBackgroundPixmap(GetDisplay(),x1y2x2y2,pixmap);
  XSetWindowBackgroundPixmap(GetDisplay(),x2y1x2y2,pixmap);
  XMapRaised(GetDisplay(),x1y1x2y1);
  XMapRaised(GetDisplay(),x1y1x1y2);
  XMapRaised(GetDisplay(),x2y1x2y2);
  XMapRaised(GetDisplay(),x1y2x2y2);
  if (GetIsDesktop()==True) {
    XLowerWindow(GetDisplay(),GetWindow()); 
    LowerIcons();
    last=GetWindow();
    direntry=directory;
    while (direntry!=NULL) {
      last=direntry->Iconwindow;
      direntry=direntry->NextItem;
    }
    wc.sibling=last;
    wc.stack_mode=Above;
    changemask=CWStackMode|CWSibling;
    XConfigureWindow(GetDisplay(),x1y1x2y1,changemask,&wc);
    XConfigureWindow(GetDisplay(),x1y1x1y2,changemask,&wc);
    XConfigureWindow(GetDisplay(),x2y1x2y2,changemask,&wc);
    XConfigureWindow(GetDisplay(),x1y2x2y2,changemask,&wc);
  }
  do {
    if (x<sx) { x1=x; sx1=sx;} else { x1=sx; sx1=x;}
    if (y<sy) { y1=y; sy1=sy;} else { y1=sy; sy1=y;}
    XMoveResizeWindow(GetDisplay(),x1y1x2y1,x1,y1,sx1-x1+1,1);
    XMoveResizeWindow(GetDisplay(),x1y1x1y2,x1,y1,1,sy1-y1+1);
    XMoveResizeWindow(GetDisplay(),x2y1x2y2,sx1,y1,1,sy1-y1+1);
    XMoveResizeWindow(GetDisplay(),x1y2x2y2,x1,sy1,sx1-x1+1,1);
    XNextEvent(GetDisplay(),&event);
    while(XEventsQueued(GetDisplay(),QueuedAfterReading)>2 && event.type==MotionNotify)
      XNextEvent(GetDisplay(),&event);
    switch (event.type) {
    case MotionNotify:
      sx=event.xmotion.x;
      sy=event.xmotion.y;
      if (sx < -movewinposx) sx= -movewinposx;
      if (sy < -movewinposy) sy= -movewinposy;
      if (moveony==True) {
	if (sx > -movewinposx+width-15) 
	  sx= -movewinposx+width-15;
      } else {
	if (sx > -movewinposx+width-1) 
	  sx= -movewinposx+width-1;
      }
      if (moveonx==True) {
	if (sy > -movewinposy+height-15) 
	  sy= -movewinposy+height-15;
      } else {
	if (sy > -movewinposy+height-1) 
	  sy= -movewinposy+height-1;
      }
     break;
    case ButtonRelease:
      buttonup=1;
      break;
    }
  } while (buttonup==0);
  XDestroyWindow(GetDisplay(),x1y1x2y1);
  XDestroyWindow(GetDisplay(),x1y1x1y2);
  XDestroyWindow(GetDisplay(),x2y1x2y2);
  XDestroyWindow(GetDisplay(),x1y2x2y2);
  XFreePixmap(GetDisplay(),pixmap);
  if (sx<x){
    border=x;
    x=sx;
    sx=border;
  }
  if (sy<y){
    border=y;
    y=sy;
    sy=border;
  }
  if (GetKindOfWindow()!=STRUCTURE) {
    direntry=directory;
    while (direntry!=NULL) {
      XGetGeometry(GetDisplay(),direntry->Iconwindow,&root,
		   &iconx,&icony,&width,&height,&border,&depth);
      if ((int)(sx-iconx)>=0 &&
	  (int)(iconx+width-x)>=0 && 
	  (int)(sy-icony)>=0 && 
	  (int)(icony+height-y)>=0 ) {
	direntry->mark=True;
	CreateIconWithDirentry(direntry,False,False);
      }
      direntry=direntry->NextItem;
    }
  }
}

int GetMarkedNumber()
{
  struct item *direntry;
  int number=0;
  direntry=directory;
  while (direntry!=NULL) {
    if (direntry->mark==True) {
      number++;
    }
    direntry=direntry->NextItem;
  }
  return number;
}

void GetMarkedNames(char** files)
{
  struct item *direntry;
  char *text;
  char *text2;
  char *text3;
  direntry=directory;
  *files=malloc(1);
  (*files)[0]=0;
  while (direntry!=NULL) {
    if (direntry->mark==True) {
      text2=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
      strcpy(text2,GetPath());
      strcat(text2,direntry->filename);
      text3=GenerateSlashName(text2);
      text=malloc(strlen(*files)+strlen(text3)+2);
      strcpy(text,*files);
      strcat(text," ");
      strcat(text,text3);
      free(text3);
      free(text2);
      free(*files);
      *files=text;
    }
    direntry=direntry->NextItem;
  }
}

void IconMotion(struct item *direntry,int x,int y,int rootx,int rooty)
{
  struct item *direntry2;
  char *newpos;
  char *pos;
  char *icon;
  char *start;
  char *text;
  char *files;
  Window NeuesIcon;
  Window other;
  XEvent event;
  int ondesk;
  int xw,yw;
  int xr,yr;
  int buttonup=0;
  unsigned int keys;
  XWindowAttributes attr;

  NeuesIcon=direntry->Iconwindow;
  direntry->mark=False;
  if (GetMarkedNumber()>0)
    CreateIconWithDirentry(direntry,True,True);
  else
    CreateIconWithDirentry(direntry,True,False);
  direntry->mark=True;
  other=direntry->Iconwindow;
  direntry->Iconwindow=NeuesIcon;
  NeuesIcon=other;
  XReparentWindow(GetDisplay(),NeuesIcon,XDefaultRootWindow(GetDisplay()),
		  rootx-x,rooty-y);
  do {
    XNextEvent(GetDisplay(),&event);
    while(XEventsQueued(GetDisplay(),QueuedAfterReading)>2 && event.type==MotionNotify)
      XNextEvent(GetDisplay(),&event);
    switch (event.type) {
    case MotionNotify:
      XMoveWindow(GetDisplay(),NeuesIcon,
		  event.xmotion.x_root-x,event.xmotion.y_root-y);
      break;
    case ButtonRelease:
      buttonup=1;
      break;
    }
  } while (buttonup==0);
  XDestroyWindow(GetDisplay(),NeuesIcon);
  XFlush(GetDisplay());
  XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),&other,
		&NeuesIcon,&xr,&yr,&xw,&yw,&keys);
  ondesk=False;
  if (GetIsDesktop()==True) {
    direntry2=directory;
    while (direntry2!=NULL) {
      if (direntry2->Iconwindow==NeuesIcon)
	ondesk=True;
      direntry2=direntry2->NextItem;
    }
  }
  if (NeuesIcon==GetGreatestParent(GetWindow()) || 
      ondesk==True || 
      (GetIsDesktop()==True &&
       NeuesIcon==None)) {
    if (GetIsDesktop()==False || (ondesk==False && (GetIsDesktop()==False || NeuesIcon!=None)))
      XQueryPointer(GetDisplay(),GetMoveWindow(),&other,&NeuesIcon,
		    &xr,&yr,&xw,&yw,&keys);
    if (NeuesIcon==None) {
      if (GetKindOfWindow()==NORMAL) {
	text=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
	strcpy(text,GetPath());
	strcat(text,direntry->filename);
	GetFileInfoFromDesktop(text,&pos,&icon,&start);
	if (pos==NULL || pos[0]!='a') {
	  if (GetIsDesktop()==True && grid>0) {
	    XGetWindowAttributes(GetDisplay(),direntry->Iconwindow,&attr);
	    xr=(int)(0.5+(double)(xw-x+attr.width/2)/(double)grid)*grid;
	    yr=(int)(0.5+(double)(yw-y+attr.height/2)/(double)grid)*grid;
	    SetPositionToText(&newpos,xr-attr.width/2,yr-attr.height/2,' ');
	    SendFileInfoToDesktop(text,newpos,NULL,NULL);
	    free(newpos);
	    XMoveWindow(GetDisplay(),direntry->Iconwindow,
			xr-attr.width/2,yr-attr.height/2);	    
	  } else {
	    SetPositionToText(&newpos,xw-x,yw-y,' ');
	    SendFileInfoToDesktop(text,newpos,NULL,NULL);
	    free(newpos);
	    XMoveWindow(GetDisplay(),direntry->Iconwindow,
			xw-x,yw-y);
	  }
	}
	free(pos);
	free(icon);
	free(start);
	free(text);
      }
    } else 
      if (NeuesIcon!=direntry->Iconwindow) {
	direntry2=GetDirentryFromWindow(NeuesIcon);
	if (direntry2!=NULL) {
	  if (direntry2->filename[strlen(direntry2->filename)-1]!='/') {
	    text=malloc(strlen(direntry2->filename)+strlen(GetPath())+1);
	    strcpy(text,GetPath());
	    strcat(text,direntry2->filename);
	    GetFileInfoFromDesktop(text,&pos,&icon,&start);
	    if (start==NULL || icon==NULL) {
	      free(icon);
	      free(start);
	      GetExtensionParameter(direntry2->filename,text,&icon,&start);
	    }
	    GetMarkedNames(&files);
	    ExecuteShellWithFollowLink(text,start,files,0);
	    free(files);
	    free(text);
	    free(pos);
	    free(icon);
	    free(start);
	  } else {
	    GetMarkedNames(&files);
	    text=strdup(direntry2->filename);
	    GoalWasFolder(GetMoveWindow(),text,files,keys);
	    free(text);
	    free(files);
	  }
	}
      }
  } else {
    GetMarkedNames(&files);
    if (GetKindOfWindow()==NORMAL)
      SendFiles(event,x,y,files);
    else
      SendFiles(event,0,0,files);      
    free(files);
  }
  CalcMoveWindowSize();
}

void NewIcon(char *filename,char *orginal,int x,int y,int iconnr)
{
  char *pos;
  char *icon;
  char *start;
  char *text,*text2,*text3;
  struct item *direntry;
  struct item *new;
  int i=1,j=1;

  if (iconnr==1) 
    SetNextPosition(x,y);
  do {
    j++;
    if (filename[j-2]=='/')
      i=j-1;
  } while (filename[j]!=0);
  text=malloc(j-i+1);
  strncpy(text,filename+i,j-i);
  text[j-i]=0;
  GetFileInfoFromDesktop(orginal,&pos,&icon,&start);
  if (start==NULL || icon==NULL) {
    free(icon);
    free(start);
    GetExtensionParameter(text,filename,&icon,&start);
  }
  text2=malloc(strlen(text)+strlen(GetPath())+1);
  strcpy(text2,GetPath());
  strcat(text2,text);
  EraseFileInfoInDesktop(text2);
  SendFileInfoToDesktop(text2,NULL,icon,start);
  direntry=directory;
  if (direntry==NULL) {
    directory=malloc(sizeof(struct item));
    new=directory;
  } else {
    while (direntry->NextItem!=NULL)
      direntry=direntry->NextItem;
    new=malloc(sizeof(struct item));
    direntry->NextItem=new;
  }
  new->filename=malloc(strlen(text)+1);
  strcpy(new->filename,text);
  if (strncmp(text2,"~/",strlen("~/"))==0) {
    text3=malloc(strlen((char*)getenv("HOME"))+strlen(text2)+1);
    strcpy(text3,(char*)getenv("HOME"));
    strcat(text3,text2+1);
    free(text2);
    text2=malloc(strlen(text3)+1);
    strcpy(text2,text3);
    free(text3);
  }
  stat(text2,&(new->info));
  new->type=0;
  if ((new->info.st_mode & 16384) == 16384) {
    text2[strlen(text2)-1]=0;
    new->type++;
  }
  lstat(text2,&(new->info));
  new->mark=False;
  if ((new->info.st_mode & 8192) == 8192)
    new->type=new->type+2;
  new->NextItem=NULL;
  CreateIconWithDirentry(new,True,False);    
  free(text2);
  free(pos);
  free(icon);
  free(start);
  free(text);
  /*if (GetKindOfWindow()==DETAIL) {
    SortIcons(sorttype);
    PlaceIcons();
  }*/
  CalcMoveWindowSize();
}

void PlaceIcons()
{
  int x,y;
  unsigned int width,height;
  unsigned int border,depth;
  Window root;
  struct item *direntry;
  XGetGeometry(GetDisplay(),GetWindow(),&root,&x,&y,
	       &width,&height,&border,&depth);
  if (GetKindOfWindow()==NORMAL)
    XMoveResizeWindow(GetDisplay(),GetMoveWindow(),0,0,width,height);
  ResetPosition();
  direntry=directory;
  while (direntry!=NULL) {
    XGetGeometry(GetDisplay(),direntry->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    PositionArrange(direntry,width,height);
    direntry=direntry->NextItem;
  }
  CalcMoveWindowSize();
}

void EraseIcon(char *filename)
{
  struct item *direntry;
  struct item *last;
  char *text;
  char *text2;
  int i=1,j=1;
  ResetPositionHistory();
  text=malloc(strlen(filename)+2);
  strcpy(text,filename);
  if (text[strlen(text)-1]=='/')
    text[strlen(text)-1]=0;
  else
    strcat(text,"/");
  EraseFileInfoInDesktop(text);
  EraseFileInfoInDesktop(filename);
  free(text);
  do {
    j++;
    if (filename[j-2]=='/')
      i=j-1;
  } while (filename[j]!=0);
  text=malloc(j-i+1);
  strncpy(text,filename+i,j-i);
  text[j-i]=0;
  text2=malloc(strlen(text)+2);
  strcpy(text2,text);
  if (text2[strlen(text2)-1]=='/')
    text2[strlen(text2)-1]=0;
  else
    strcat(text2,"/");
  direntry=directory;
  if (direntry!=NULL) {
    if (strcmp(direntry->filename,text)==0 ||
	strcmp(direntry->filename,text2)==0) {
      directory=direntry->NextItem;
      free(direntry->filename);
      XDestroyWindow(GetDisplay(),direntry->Iconwindow);
      free(direntry);
    } else {
      do {
	last=direntry;
	direntry=direntry->NextItem;
      } while (direntry!=NULL &&
	       strcmp(direntry->filename,text)!=0 &&
	       strcmp(direntry->filename,text2)!=0);
      if (direntry!=NULL) {
	last->NextItem=direntry->NextItem;
	free(direntry->filename);
	XDestroyWindow(GetDisplay(),direntry->Iconwindow);
	free(direntry);
      }
    }
  }
  free(text);
  free(text2);
  /*if (GetKindOfWindow()==DETAIL) {
    SortIcons(sorttype);
    PlaceIcons();
  }*/
  CalcMoveWindowSize();
}

void MoveTheMoveWindow(XEvent evnt)
{
  int z;
  int goalx,goaly;
  int xw,yw;
  int xr,yr;
  int buttonup=0;
  unsigned int keys;
  Window child;
  int x,y,add,addp;
  unsigned int width,height;
  int ax,ay;
  unsigned int nwidth,nheight;
  unsigned int minx,miny;
  unsigned int border,depth;
  Window root;
  Pixmap pixelmap;

  XGetGeometry(GetDisplay(),GetWindow(),&root,&x,&y,
	       &width,&height,&border,&depth);
  if (moveony==True) width=width-14;
  if (moveonx==True) height=height-14;
  XGetGeometry(GetDisplay(),GetMoveWindow(),&root,&ax,&ay,
	       &minx,&miny,&border,&depth);
  XQueryPointer(GetDisplay(),GetMoveWindow(),&root,&child,
		&xr,&yr,&xw,&yw,&keys);
  if (evnt.xbutton.button==2) {
    do {
      XNextEvent(GetDisplay(),&evnt);
     while(XEventsQueued(GetDisplay(),QueuedAfterReading)>2 && evnt.type==MotionNotify)
      XNextEvent(GetDisplay(),&evnt);
     switch (evnt.type) {
      case MotionNotify:
	goalx=evnt.xmotion.x_root-xr+ax;
	goaly=evnt.xmotion.y_root-yr+ay;
	if (goalx>0) goalx=0;
	if (goaly>0) goaly=0;
	if (goalx<(int)width-(int)minx) goalx=(int)width-(int)minx;
	if (goaly<(int)height-(int)miny) goaly=(int)height-(int)miny;
	XMoveWindow(GetDisplay(),GetMoveWindow(),goalx,goaly);
	if (GetKindOfWindow()==DETAIL)
	  XMoveWindow(GetDisplay(),GetDetailWindow(),20+goalx,0);
	/*          */
	if (moveony==True) {
	  XMoveResizeWindow(GetDisplay(),movey,0,
	      14+(int)((double)(height-28)*(double)(-goaly)/(double)miny),
	      14,
	      1+(int)((double)(height-28)*(double)height/(double)miny)
			    );
	}
	if (moveonx==True) {
	  XMoveResizeWindow(GetDisplay(),movex,
	      14+(int)((double)(width-28)*(double)(-goalx)/(double)minx),
	      0,
	      1+(int)((double)(width-28)*(double)width/(double)minx),
	      14
			    );
	}
	/*         */

	break;
      case ButtonRelease:
	buttonup=1;
	break;
      }
    } while (buttonup==0);
  } else if (evnt.xbutton.window==movex || evnt.xbutton.window==movexmain) {
    XGetGeometry(GetDisplay(),movex,&root,&x,&y,
		 &nwidth,&nheight,&border,&depth);
    if (evnt.xbutton.window==movex) {
      do {
	XNextEvent(GetDisplay(),&evnt);
	while(XEventsQueued(GetDisplay(),QueuedAfterReading)>2 && evnt.type==MotionNotify)
	  XNextEvent(GetDisplay(),&evnt);
	switch (evnt.type) {
	case MotionNotify:
	  goalx=evnt.xmotion.x_root-xr+x;
	  if (goalx<14) goalx=14;
	  z=(int)(-0.5-(double)(goalx-14)*(double)minx/(double)(width-28));
	  if (goalx>width-nwidth-14) {
	    goalx=width-nwidth-14;
	    z=width-minx;
	  }
	  XMoveWindow(GetDisplay(),movex,goalx,0);
	  XMoveWindow(GetDisplay(),GetMoveWindow(),z,ay);
	  if (GetKindOfWindow()==DETAIL)
	    XMoveWindow(GetDisplay(),GetDetailWindow(),20+z,0);
	  break;
	case ButtonRelease:
	  buttonup=1;
	  break;
	}
      } while (buttonup==0);
    } else {
      if (evnt.xbutton.x<14) {
	DataPixmap(movexmainleft_p_xpm,&pixelmap,NULL);
	XCopyArea(GetDisplay(),pixelmap,movexmain,GetGC(),0,0,14,14,0,0);
	XFreePixmap(GetDisplay(),pixelmap);
	addp=1;
      } else if (evnt.xbutton.x<x) {
	addp=width;
      } else if (evnt.xbutton.x>width-14) {
	DataPixmap(movexmainright_p_xpm,&pixelmap,NULL);
	XCopyArea(GetDisplay(),pixelmap,movexmain,GetGC(),0,0,14,14,width-14,0);
	XFreePixmap(GetDisplay(),pixelmap);
	addp=-1;
      } else if (evnt.xbutton.x>x+nwidth) {
	addp=-width;
      } else 
	addp=0;
      do {
	add=addp;
	if (ax+add>0)
	  add=-ax;
	else if (ax+add<(int)width-(int)minx)
	  add=-ax+(int)width-(int)minx;
	XMoveWindow(GetDisplay(),movex,
		    14+(int)((double)(width-28)*(double)(-(ax+add))/(double)minx),0);
	XMoveWindow(GetDisplay(),GetMoveWindow(),ax+add,ay);
	if (GetKindOfWindow()==DETAIL)
	  XMoveWindow(GetDisplay(),GetDetailWindow(),20+ax+add,0);
	XFlush(GetDisplay());
	XSync(GetDisplay(),0);
	XGetGeometry(GetDisplay(),GetMoveWindow(),&root,&ax,&ay,
		     &minx,&miny,&border,&depth);
	while (XEventsQueued(GetDisplay(),QueuedAfterReading)!=0 && evnt.type!=ButtonRelease)
	  XNextEvent(GetDisplay(),&evnt);
      } while(evnt.type!=ButtonRelease && (addp==1 || addp==-1));
      XClearWindow(GetDisplay(),movexmain);
    } 
  } else if (evnt.xbutton.window==movey || evnt.xbutton.window==moveymain) {
    XGetGeometry(GetDisplay(),movey,&root,&x,&y,
		 &nwidth,&nheight,&border,&depth);
    if (evnt.xbutton.window==movey) {
      do {
	XNextEvent(GetDisplay(),&evnt);
	while(XEventsQueued(GetDisplay(),QueuedAfterReading)>2 && evnt.type==MotionNotify)
	  XNextEvent(GetDisplay(),&evnt);
	switch (evnt.type) {
	case MotionNotify:
	  goaly=evnt.xmotion.y_root-yr+y;
	  if (goaly<14) goaly=14;
	  z=(int)(-0.5-(double)(goaly-14)*(double)miny/(double)(height-28));
	  if (goaly>height-nheight-14) { 
	    goaly=height-nheight-14;
	    z=height-miny;
	  }
	  XMoveWindow(GetDisplay(),movey,0,goaly);
	  XMoveWindow(GetDisplay(),GetMoveWindow(),ax,z);
	  break;
	case ButtonRelease:
	  buttonup=1;
	  break;
	}
      } while (buttonup==0);
    } else {
      if (evnt.xbutton.y<14) {
	DataPixmap(moveymaintop_p_xpm,&pixelmap,NULL);
	XCopyArea(GetDisplay(),pixelmap,moveymain,GetGC(),0,0,14,14,0,0);
	XFreePixmap(GetDisplay(),pixelmap);
	addp=1;
      } else if (evnt.xbutton.y<y) {
	addp=height;
      }else if (evnt.xbutton.y>height-14) {
	DataPixmap(moveymainbottom_p_xpm,&pixelmap,NULL);
	XCopyArea(GetDisplay(),pixelmap,moveymain,GetGC(),0,0,14,14,0,height-14);
	XFreePixmap(GetDisplay(),pixelmap);
	addp=-1;
      } else if (evnt.xbutton.y>y+nheight) {
	addp=-height;
      } else 
	addp=0;
      do {
	add=addp;
	if (ay+add>0)
	  add=-ay;
	else if (ay+add<(int)height-(int)miny)
	  add=-ay+(int)height-(int)miny;
	XMoveWindow(GetDisplay(),movey,0,
		    14+(int)((double)(height-28)*(double)(-(ay+add))/(double)miny));
	XMoveWindow(GetDisplay(),GetMoveWindow(),ax,ay+add);
	XFlush(GetDisplay());
	XSync(GetDisplay(),0);
	XGetGeometry(GetDisplay(),GetMoveWindow(),&root,&ax,&ay,
		     &minx,&miny,&border,&depth);
	while (XEventsQueued(GetDisplay(),QueuedAfterReading)!=0 && evnt.type!=ButtonRelease)
	  XNextEvent(GetDisplay(),&evnt);
      } while(evnt.type!=ButtonRelease && (addp==1 || addp==-1));
      XClearWindow(GetDisplay(),moveymain);
    } 
  }
  CalcMoveWindowSize();
}

int SortHelp(const void *a,const void *b)
{
  int adot,bdot,i;
  Window root;
  int ax,bx,topa,bottoma,topb,bottomb;
  unsigned int width,height,border,depth;
  int x,y;
  if (sorttype==ARRANGE) {
    XGetGeometry(GetDisplay(),((struct item*)a)->Iconwindow,&root,&x,&y,&width,&height,&border,&depth);
    ax=x;
    topa=y;
    bottoma=y+height;
    XGetGeometry(GetDisplay(),((struct item*)b)->Iconwindow,&root,&x,&y,&width,&height,&border,&depth);
    bx=x;
    topb=y;
    bottomb=y+height;
    if (topa>bottomb)
      return(1);
    else if (topb>bottoma)
      return(-1);
    else
      return(ax-bx);
  } else if (sorttype==NAME)
    return(strcmp(((struct item*)a)->filename,((struct item*)b)->filename));
  else if ((((struct item*)a)->type==0 || ((struct item*)a)->type==2) &&
      (((struct item*)b)->type==1 || ((struct item*)b)->type==3))
    return(1);
  else if ((((struct item*)a)->type==1 || ((struct item*)a)->type==3) &&
	   (((struct item*)b)->type==0 || ((struct item*)b)->type==2))
    return(-1);
  else {     
    adot=strlen(((struct item*)a)->filename)-1;i=0;
    while (((struct item*)a)->filename[i]!=0) {
      if (((struct item*)a)->filename[i]=='.' && i!=0)
	adot=i;
      i++;
    }
    bdot=strlen(((struct item*)b)->filename)-1;i=0;
    while (((struct item*)b)->filename[i]!=0) {
      if (((struct item*)b)->filename[i]=='.' && i!=0)
	bdot=i;
      i++;
    }
    if (sorttype==EXTENSION && 
	(strcmp((((struct item*)a)->filename)+adot+1,
		(((struct item*)b)->filename)+bdot+1)<0))
      return(-1);
    else if (sorttype==EXTENSION && 	     
	     (strcmp((((struct item*)a)->filename)+adot+1,
		     (((struct item*)b)->filename)+bdot+1)>0))
      return(1);
    else if (sorttype==DATEASC && ((struct item*)a)->info.st_mtime <
	     ((struct item*)b)->info.st_mtime)
      return(-1);
    else if (sorttype==DATEASC && ((struct item*)a)->info.st_mtime >
	     ((struct item*)b)->info.st_mtime)
      return(1);
    else if (sorttype==DATEDESC && ((struct item*)a)->info.st_mtime <
	     ((struct item*)b)->info.st_mtime)
      return(1);
    else if (sorttype==DATEDESC && ((struct item*)a)->info.st_mtime >
	     ((struct item*)b)->info.st_mtime)
      return(-1);
    else if (sorttype==SIZEASC && ((struct item*)a)->info.st_size <
	     ((struct item*)b)->info.st_size)
      return(-1);
    else if (sorttype==SIZEASC && ((struct item*)a)->info.st_size >
	     ((struct item*)b)->info.st_size)
      return(1);
    else if (sorttype==SIZEDESC && ((struct item*)a)->info.st_size <
	     ((struct item*)b)->info.st_size)
      return(1);
    else if (sorttype==SIZEDESC && ((struct item*)a)->info.st_size >
	     ((struct item*)b)->info.st_size)
      return(-1);
    else return(strcmp(((struct item*)a)->filename,
		       ((struct item*)b)->filename)); /*TYPE*/
  }
}

void SortIcons(int type)
{
  int i,j;
  struct item *direntry;
  struct item *direntry2;
  struct item *direntry3;
  struct item *newdir;

  ResetPositionHistory();
  if (GetKindOfWindow()!=STRUCTURE) {
    sorttype=type;
    i=0;
    direntry=directory;
    while (direntry!=NULL) {
      i++;
      direntry=direntry->NextItem;
    }
    if (i>0) {
      newdir=malloc(i*sizeof(struct item));
      i=0;
      direntry=directory;
      while (direntry!=NULL) {
	newdir[i]=*direntry;
	i++;
	direntry=direntry->NextItem;
      }
      qsort(newdir,i,sizeof(struct item),SortHelp);
      direntry2=malloc(sizeof(struct item));
      *direntry2=newdir[0];
      direntry=direntry2;
      for (j=1;j<i;j++) { 
	direntry3=direntry;
	direntry=malloc(sizeof(struct item));
	*direntry=newdir[j];
	direntry3->NextItem=direntry;
      }
      direntry->NextItem=NULL;
      free(newdir);
      newdir=direntry2;
      direntry=directory;
      while (direntry!=NULL) {
	direntry2=direntry->NextItem;
	free(direntry);
	direntry=direntry2;
      }
      directory=newdir;
    }
  }
}

void UpdateWindow()
{
  static char *backgroundnameold=NULL;
  char *backgroundname=NULL;
  char *positiona=NULL;
  char *positions=NULL;
  char *text;
  int r,g,b;
  int sx,sy;
  unsigned long bg;
  Pixmap backgroundpixmap;
  XWindowAttributes rootatr;

  text=GenerateWindowName(GetPath(),NORMAL);
  GetFileInfoFromDesktop(text,&positiona,&backgroundname,&positions);
  if (backgroundname==NULL && GetIsDesktop()==False) {
    backgroundname=GetDefaultBackground();
    /*SendFileInfoToDesktop(text,NULL,backgroundname,NULL);  */
  }
  free(text);
  if (backgroundname!=NULL && (backgroundnameold==NULL || 
			       GetIsDesktop()==True ||
			       strcmp(backgroundnameold,backgroundname)!=0)){
    if (LoadPixmap(backgroundname,&backgroundpixmap,NULL)==0) {
      if (GetIsDesktop()==True) 
	XSetWindowBackgroundPixmap(GetDisplay(),XDefaultRootWindow(GetDisplay()),backgroundpixmap);
      else
	XSetWindowBackgroundPixmap(GetDisplay(),GetMoveWindow(),backgroundpixmap);
      XFreePixmap(GetDisplay(),backgroundpixmap);
    } else {
      bg=GetBestColor(65535,65535,65535);
      if (backgroundname[0]=='#') {
	/*********************************/
	sscanf(backgroundname+2,"%u %u %u",&r,&g,&b);
	bg=GetBestColor(257*r,257*g,257*b);
	/*********************************/
      } 
      if (GetIsDesktop()==True) 
	XSetWindowBackground(GetDisplay(),XDefaultRootWindow(GetDisplay()),bg);	  
      else
	XSetWindowBackground(GetDisplay(),GetMoveWindow(),bg);
    }
    free(backgroundnameold);
    backgroundnameold=malloc(strlen(backgroundname)+1);
    strcpy(backgroundnameold,backgroundname);
    
  }  
  if (GetIsDesktop()==True) {
    GetPositionFromText(positiona,&sx,&sy,&grid);
    XGetWindowAttributes(GetDisplay(),XDefaultRootWindow(GetDisplay()),&rootatr);
    sx=rootatr.width;
    sy=rootatr.height;
    if (positions[0]=='c'){
      sx--;
      sy--;
    } else if (positions[0]=='a')
      sx--;
    else if (positions[0]!='b')
      sy--;
    XResizeWindow(GetDisplay(),GetWindow(),sx,sy);
    XResizeWindow(GetDisplay(),GetMoveWindow(),sx,sy);
    XClearWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()));
  } else 
    XClearWindow(GetDisplay(),GetMoveWindow());
  if (positiona!=NULL && positiona[0]=='a')
    SetShowHidden(False);
  else
    SetShowHidden(True);
  free(positiona);
  free(backgroundname);
  free(positions);
}

void SetPositionOnLastIcon()
{
  int x,y;
  unsigned int width,height;
  unsigned int border,depth;
  Window root;
  int ymax;
  struct item *direntry;
  
  ymax=5;
  direntry=directory;
  while (direntry!=NULL) {
    XGetGeometry(GetDisplay(),direntry->Iconwindow,&root,&x,&y,
		 &width,&height,&border,&depth);
    if (ymax<height+y) 
      ymax=height+y+10;
    direntry=direntry->NextItem;
  }
  SetNextPosition(5,ymax);
}

void UpdateIcons(int fast)
{
  struct item *savedir;
  struct item *olddirentry;
  struct item *newdirentry;
  struct item *eraseentry;
  int resorttype;
  
  StopAlarm();
  UpdateTimer();
  if (fast==False) {
    EraseIconPixmaps();
    UpdateWindow();
    UpdateFontName();
    UpdateColors();
    if (GetIsDesktop()==True)
      ResetPosition();
  }
  if (GetKindOfWindow()!=STRUCTURE) {
    if (GetKindOfWindow()==NORMAL && GetIsDesktop()==False)
      SetPositionOnLastIcon(); 
    resorttype=sorttype;
    SortIcons(NAME);
    savedir=directory;
    directory=GetDirectory(GetPath());
    SortIcons(NAME);
    olddirentry=savedir;
    newdirentry=directory;
    ResetPositionHistory();
    while (newdirentry!=NULL || olddirentry!=NULL) {
      if (newdirentry!=NULL && 
	  olddirentry!=NULL &&
	  strcmp(newdirentry->filename,olddirentry->filename)==0) {
	newdirentry->Iconwindow=olddirentry->Iconwindow;
	if (fast==False)
	  CreateIconWithDirentry(newdirentry,False,False);
	else
	  newdirentry->mark=olddirentry->mark;
	olddirentry=olddirentry->NextItem;
	newdirentry=newdirentry->NextItem;
      }
      if (newdirentry!=NULL && 
	  olddirentry!=NULL &&
	  strcmp(newdirentry->filename,olddirentry->filename)>0) {
	XDestroyWindow(GetDisplay(),olddirentry->Iconwindow);      
	olddirentry=olddirentry->NextItem;
      }
      if (newdirentry!=NULL && 
	  olddirentry!=NULL &&
	  strcmp(newdirentry->filename,olddirentry->filename)<0) {
	CreateIconWithDirentry(newdirentry,True,False);
	newdirentry=newdirentry->NextItem;
      }
      if (newdirentry==NULL && olddirentry!=NULL) {
	XDestroyWindow(GetDisplay(),olddirentry->Iconwindow);      
	olddirentry=olddirentry->NextItem;
      }      
      if (newdirentry!=NULL && olddirentry==NULL) {
	CreateIconWithDirentry(newdirentry,True,False);
	newdirentry=newdirentry->NextItem;
      }      
    }
    while(savedir!=NULL) {
      eraseentry=savedir->NextItem;
      free(savedir->filename);
      free(savedir);
      savedir=eraseentry;
    }
    sorttype=resorttype;
    if (GetKindOfWindow()==DETAIL) {
      SortIcons(resorttype);
      PlaceIcons();
    }
  } else {
    /*if (fast==False) {*/
    UpdateWindowTree();
    /*MinusTree(toptree);
      PlusTree(toptree);*/
    /*}*/
  }
  CalcMoveWindowSize();
  ReactivateAlarm();
}

void RunAndWaitAtPosition(char *text,char *start)
{
  char *windowname;
  char *positions;
  char *positiona;
  char *background;
  XEvent event;

  if (strcmp(start,"dfm !0! -detail")==0)
    windowname=GenerateWindowName(text,DETAIL);
  else if (strcmp(start,"dfm !0! -structure")==0)
    windowname=GenerateWindowName(text,STRUCTURE);
  else
    windowname=GenerateWindowName(text,NORMAL);
  GetFileInfoFromDesktop(windowname,&positiona,&background,
			 &positions);
  if (positiona!=NULL && positiona[0]=='a') { /* the hidden info must not be changed */
    free(positions);
    free(positiona);
    free(background);
    GetFileInfoFromDesktop(GetWindowName(),&positiona,&background,
			   &positions);
    positiona[0]='a';
    SendFileInfoToDesktop(windowname,positiona,NULL,
			   positions);
  } else {
    free(positions);
    free(positiona);
    free(background);
    GetFileInfoFromDesktop(GetWindowName(),&positiona,&background,
			   &positions);
    positiona[0]=' ';
    SendFileInfoToDesktop(windowname,positiona,NULL,
			   positions);
  }
  ExecuteShellWithFollowLink(text,start,NULL,0);
  SetDestroy(5);
  do {
    XNextEvent(GetDisplay(),&event);
  } while (event.type!=VisibilityNotify);
  StopDestroy();
  free(windowname);
  free(positions);
  free(positiona);
  free(background);
}

void HandleKeyPress(XEvent evnt)
{
  char keys[10];  
  KeySym mykey;
  unsigned int key;
  int size;
  Window dummy1,dummy2;
  int dummy3,dummy4,dummy5,dummy6;
  int i;
  char *text;

  size=XLookupString((XKeyPressedEvent*)(&evnt),keys,10,&mykey,0);
  if (size==1) {
    if (mykey==XK_Escape) {
      XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());
      UpdateIcons(False);
      XUndefineCursor(GetDisplay(),GetWindow());
    } else {
      XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),
		    &dummy1,&dummy2,&dummy3,&dummy4,&dummy5,&dummy6,&key);
      if (key==ControlMask) {
	if (mykey==XK_a) {
	  XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());
	  SetAllMarks();
	  XUndefineCursor(GetDisplay(),GetWindow());
	} else if (mykey==XK_y) {
	  XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());
	  SortIcons(sorttype);
	  PlaceIcons();
	  XUndefineCursor(GetDisplay(),GetWindow());
	} else if (mykey==XK_q) {
	  if (GetIsDesktop()==False) {
	    XCloseDisplay(GetDisplay());
	    exit(0);
	  }
	} else if (mykey==XK_f) {
	  NewFolder();
	} else if (mykey==XK_s) {
	  ExecuteShellcommand(GetPath(),"dfm !0! -structure",NULL,0);
	} else if (mykey==XK_d) {
	  if (GetKindOfWindow()==NORMAL && GetIsDesktop()==False) {
	    RunAndWaitAtPosition(GetPath(),"dfm !0! -detail");
	    XCloseDisplay(GetDisplay());
	    exit(0);
	  } else
	    ExecuteShellcommand(GetPath(),"dfm !0! -detail",NULL,0);
	} else if (mykey==XK_n) {
	  if (GetKindOfWindow()==DETAIL) {
	    RunAndWaitAtPosition(GetPath(),"dfm !0!");
	    XCloseDisplay(GetDisplay());
	    exit(0);
	  } else
	    ExecuteShellcommand(GetPath(),"dfm !0!",NULL,0);
	} else if (mykey==XK_o) {
	  text=malloc(strlen("dfm -setdir  &")+strlen(GetPath())+1);
	  strcpy(text,"dfm -setdir ");
	  strcat(text,GetPath());
	  strcat(text," &");
	  for(i=0;i<strlen(text)-2;i++) 
	    if (text[i]==' ' && text[i+1]=='~' && text[i+2]=='/')
	      text[i+1]='.';
	  usershellsystem(text);
	  free(text);
	}
      }
    }
  } else { 
    if (mykey==XK_Home) {
      printf("first\n");
    } else if (mykey==XK_End) {
      printf("end\n");
    } else if (mykey==XK_Up) {
      printf("up\n");
    } else if (mykey==XK_Down) {
      printf("down\n");
    } else if (mykey==XK_Left) {
      printf("left\n");
    } else if (mykey==XK_Right) {
      printf("right\n");
    } else if (mykey==XK_Prior) {
      printf("prior\n");
    } else if (mykey==XK_Next) {
      printf("next\n");
    } 
  }
}

int HandleTheEvent(XEvent evnt)
{
  char *pos;
  char *icon;
  char *start;
  char *text;
  char *text2;
  static Time t;
  static int x=0,y=0,press=0;
  static struct item *direntryold;
  struct item *direntry;
  struct tree *treeentry;
  int done=0;
  Window dummy1,dummy2;
  int dummy3,dummy4,dummy5,dummy6;
  unsigned int keys;
  switch (evnt.type) {
  case KeyPress:
    HandleKeyPress(evnt);
    break;
  case ConfigureNotify:
    if (SaveNewWindowPosition()==True)
      CalcMoveWindowSize();
    break;
  case MotionNotify:
    if (press==1 && evnt.xmotion.window!=GetMoveWindow()) {
      direntry=GetDirentryFromWindow(evnt.xmotion.window);
      if (direntry!=NULL && direntry->mark==True ) {
	if ((x-evnt.xmotion.x_root)*(x-evnt.xmotion.x_root)>4 ||
	    (y-evnt.xmotion.y_root)*(y-evnt.xmotion.y_root)>4) {
	  if (direntry!=directory || GetKindOfWindow()!=STRUCTURE) {
	    IconMotion(direntry,
		       evnt.xmotion.x+x-evnt.xmotion.x_root,
		       evnt.xmotion.y+y-evnt.xmotion.y_root,
		       evnt.xmotion.x_root,evnt.xmotion.y_root);
	  }
	  press=0;
	}
      } else 
	press=0;
    } else 
      if (press==1) {
	RubberBand(evnt.xmotion.x,evnt.xmotion.y);
	press=0;
      }
    break;
  case ButtonRelease:
    press=0;
    break;
  case ButtonPress:
    x=evnt.xbutton.x_root;
    y=evnt.xbutton.y_root;
    if (evnt.xbutton.window==movexmain || 
	evnt.xbutton.window==moveymain ||
	evnt.xbutton.window==movex ||
	evnt.xbutton.window==movey)
      MoveTheMoveWindow(evnt);
    else if (evnt.xbutton.button==3) {
      XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());
      if (evnt.xbutton.window==GetMoveWindow()) {
	ArcWindow(evnt.xbutton.window);
	text=GenerateSlashName(GetPath());
	MenuLaunch(evnt.xbutton.time,text,True);
	free(text);
	DearcWindow(evnt.xbutton.window);
      } else {
	direntry=GetDirentryFromWindow(evnt.xbutton.window);
	if (direntry!=NULL) {
	  ArcWindow(evnt.xbutton.window);	  
	  if (direntry->mark==True) {
	    GetMarkedNames(&text);
	  } else {
	    text2=malloc(strlen(GetPath())+strlen(direntry->filename)+1);
	    strcpy(text2,GetPath());
	    strcat(text2,direntry->filename); 
	    text=GenerateSlashName(text2);
	    free(text2);
	  }
	  MenuLaunch(evnt.xbutton.time,text,False);
	  DearcWindow(evnt.xbutton.window);
	  free(text);
	} 
      }
      XUndefineCursor(GetDisplay(),GetWindow());
    } else if (evnt.xbutton.button==2)
      MoveTheMoveWindow(evnt);
    else {
      press=1;
      XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),
		    &dummy1,&dummy2,&dummy3,&dummy4,&dummy5,&dummy6,&keys);
      if (evnt.xbutton.window!=GetMoveWindow()) {
	direntry=GetDirentryFromWindow(evnt.xbutton.window);
	if (direntry!=NULL) { 
	  if (direntry->mark==False) {
	    t=evnt.xbutton.time;
	    direntryold=direntry;
	    if ((keys&ControlMask)!=ControlMask || GetKindOfWindow()==STRUCTURE)
	      UnsetMarks();
	    direntry->mark=True;
	    CreateIconWithDirentry(direntry,False,False);
	  } else {
	    if (evnt.xbutton.time-t<500 && direntry==direntryold) {
	      XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());
	      text=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
	      strcpy(text,GetPath());
	      strcat(text,direntry->filename);
	      GetFileInfoFromDesktop(text,&pos,&icon,&start);
	      if (start==NULL || icon==NULL) {
		free(start);
		free(icon);
		GetExtensionParameter(direntry->filename,text,&icon,&start);
	      }
	      if (GetKindOfWindow()==STRUCTURE && direntry==directory) {
		ExecuteShellcommand(GetPath(),"dfm !0!",NULL,0);
	      } else {
		if ((keys&ShiftMask)==ShiftMask && 
		    IsSubDesktop()==False && 
		    GetKindOfWindow()!=STRUCTURE &&
		    strncmp(start,"dfm !0!",strlen("dfm !0!"))==0)
		  RunAndWaitAtPosition(text,start);
		else
		  ExecuteShellWithFollowLink(text,start,NULL,0);
	      }
	      free(text);
	      free(pos);
	      free(icon);
	      free(start);
	      if ((keys&ControlMask)!=ControlMask)
		UnsetMarks();
	      XUndefineCursor(GetDisplay(),GetWindow());
	      if ((keys&ShiftMask)==ShiftMask && GetIsDesktop()==False) {
		XCloseDisplay(GetDisplay());
		exit(0);
	      }
	    } else {
	      t=evnt.xbutton.time;
	      direntryold=direntry;
	      if ((keys&ControlMask)==ControlMask) {
		direntry->mark=False;
		CreateIconWithDirentry(direntry,False,False);
	      }
	    }
	  }
	} else {
	  treeentry=GetTreeentryFromWindow(toptree,evnt.xbutton.window);
	  if (treeentry!=NULL) { 
	    if (treeentry->abletoexpand==True) {
	      XDefineCursor(GetDisplay(),GetWindow(),GetWaitCursor());	    
	      if (treeentry->expanded==False)
		PlusTree(treeentry);
	      else
		MinusTree(treeentry);
	      XUndefineCursor(GetDisplay(),GetWindow());
	    }
	  }
	  press=0;
	}
      } else
	if ((keys&ControlMask)!=ControlMask)
	  UnsetMarks();
    }
    break;
  default:
    break;
  }
  return(done);
}
