/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "loadicon.h"
#include "createicon.h"
#include "positionarrange.h"
#include "filemanager.h"
#include <extensions/shape.h>
#include <Xlib.h>
#include <Xutil.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "global.h"
#include "color.h"
#include "includepixmaps/noicon.xpm"
#include "includepixmaps/link.xpm"
#include "includepixmaps/littlefolder.xpm"
#include "includepixmaps/littlelinkfolder.xpm"
#include "pixmap.h"
#include "iwc.h"
#include "extension.h"
#include "positiontext.h"
#include "iconmanager.h"
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include "shell.h"

void GetXVIconName(char *filename,char **icon)
{
  char *xvpic;
  char *text;
  int character=0;
  FILE *xvfile;
  int i;
  if (strlen(filename)!=0) {         /*happens by structure and the topfolder*/
    xvpic=malloc(strlen(GetPath())+strlen(".xvpics/")+
		 strlen(filename)+1);
    strcpy(xvpic,GetPath());
    strcat(xvpic,".xvpics/");
    strcat(xvpic,filename);
    if (xvpic[0]=='~' && xvpic[1]=='/')
      xvpic[0]='.';
    if ((xvfile=fopen(xvpic, "r")) != NULL) {
      for (i=0;i<strlen(xvpic)+100;i++)
	character=fgetc(xvfile);
      fclose(xvfile);
      if (character!=-1) {
	free(*icon);
	*icon=malloc(strlen(xvpic)+1);
	strcpy(*icon,xvpic);
      } else {
	text=malloc(strlen("\\rm ")+strlen(xvpic)+1);
	strcpy(text,"\\rm ");
	strcat(text,xvpic);
	usershellsystem(text);
	free(text);
      }    
    }
    free(xvpic);
  }
}

void CreateIconWithDirentry(struct item *direntry,int newicon,int plusmore)
{
  Window root;
  mode_t um;
  struct passwd *username;
  struct group *groupname;
  char *pos;
  char *icon;
  char *start;
  char *text=NULL,*text2;
  char *fullname;
  static char *ourfont=NULL;
  static Pixmap littlefolder=0;
  static Pixmap littlefolder_shape=0;
  static Pixmap littlelinkfolder=0;
  static Pixmap saveback=0;
  static Pixmap linkpix=0;
  static int activ=0;
  static GC gch=0,gc=0;
  static XFontStruct *fontst;
  Pixmap backgroundpixmap=0,shapepixmap=0;
  Pixmap backgroundpixmap2,shapepixmap2;
  XSetWindowAttributes xswa;
  unsigned long valuemask;
  int x,y;
  int i;
  int isxvpic;
  unsigned int width,height,dummy,depth,twidth,nwidth;
  int xposicon,xpostext;
  if (activ==0) {
    gch=XCreateGC(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0);
    if (GetKindOfWindow()==DETAIL) {
      fontst=XLoadQueryFont(GetDisplay(),"8x13");
      XSetFont(GetDisplay(),gch,fontst->fid);
    }
    DataPixmap(link_xpm,&linkpix,NULL);
    DataPixmap(noicon_xpm,&saveback,NULL);
    activ++;
  }
  if (GetKindOfWindow()!=DETAIL && 
      (ourfont==NULL || 
      strcmp(GetFontName(),ourfont)!=0)) {
    if (ourfont!=NULL) {
      XFreeFont(GetDisplay(),fontst);
      free(ourfont);
    }
    fontst=XLoadQueryFont(GetDisplay(),GetFontName());
    if (fontst==NULL)
      fontst=XLoadQueryFont(GetDisplay(),"8x13");
    XSetFont(GetDisplay(),gch,fontst->fid);
    ourfont=malloc(strlen(GetFontName())+1);
    strcpy(ourfont,GetFontName());
  }
  fullname=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
  strcpy(fullname,GetPath());
  strcat(fullname,direntry->filename);
  GetFileInfoFromDesktop(fullname,&pos,&icon,&start);
  if (icon==NULL || start==NULL) {
    free(icon);
    free(start);
    GetExtensionParameter(direntry->filename,fullname,&icon,&start);  
    GetXVIconName(direntry->filename,&icon);
  }
  xswa.override_redirect=True;
  valuemask=CWOverrideRedirect;  
  if (newicon==True) {
    if (GetIsDesktop()==True) {
      direntry->Iconwindow=XCreateWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0,1,1,
					 0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
      XLowerWindow(GetDisplay(),direntry->Iconwindow);
    } else {
      direntry->Iconwindow=XCreateWindow(GetDisplay(),GetMoveWindow(),-100,-100,1,1,
					 0,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
    }
  }  
  if (GetKindOfWindow()==NORMAL) {
    if (backgroundpixmap==0) 
      loadicon(&backgroundpixmap,&shapepixmap,icon,&isxvpic);
    if (backgroundpixmap==0) {
      free(icon);
      icon=NULL;
      GetExtensionParameter(direntry->filename,fullname,&icon,&text);
      free(text);
      loadicon(&backgroundpixmap,&shapepixmap,icon,&isxvpic);
    }
    if (backgroundpixmap==0) {
      backgroundpixmap=saveback;
      isxvpic=False;
    }
  }
  if (GetKindOfWindow()==NORMAL) {
    /*NORMAL*/
    if (direntry->mark==False) {
      XSetBackground(GetDisplay(),gch,GetBGColor());
      XSetForeground(GetDisplay(),gch,GetFGColor());
    } else {
      XSetBackground(GetDisplay(),gch,GetFGColor());
      XSetForeground(GetDisplay(),gch,GetBGColor());
    }      
    if (strcmp(direntry->filename,".linktorootdir/")==0) {
      text=malloc(strlen("/")+1);
      strcpy(text,"/");
    } else {
      if (strcmp(direntry->filename,".linktohomedir/")==0) {
	text=malloc(strlen("~/")+1);
	strcpy(text,"~/");
      } else {
	text=malloc(strlen(direntry->filename)+1);
	strcpy(text,direntry->filename);
      }     
    } 
    XGetGeometry(GetDisplay(),backgroundpixmap,&root,&x,&y,&width,&height,
		 &dummy,&depth);
    twidth=XTextWidth(fontst,text,strlen(text));
    if (twidth<width) {
      xpostext=(width-twidth)/2;
      xposicon=0;
      nwidth=width;
    } else {
      xposicon=(twidth-width)/2;
      xpostext=0;
      nwidth=twidth;
    }
    if (direntry->type>=2 || GetIsDesktop()==True)
      nwidth=nwidth+11;
    if (plusmore==True)
      nwidth=nwidth+8+XTextWidth(fontst,"+ more",strlen("+ more"));
    backgroundpixmap2=XCreatePixmap(GetDisplay(),direntry->Iconwindow,
				    nwidth,height+2+fontst->ascent+fontst->descent,depth);
    if (direntry->mark==True) {
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,xposicon,0,width,height);
      XSetFunction(GetDisplay(),gch,GXxor);
    }
    XCopyArea(GetDisplay(),backgroundpixmap,backgroundpixmap2,gch,0,0,width,height,
	      xposicon,0);
    XSetFunction(GetDisplay(),gch,GXcopy);
    if (direntry->type>=2) 
    XCopyArea(GetDisplay(),linkpix,backgroundpixmap2,gch,0,0,11,11,
	      xpostext+twidth,height-5);
    if (isxvpic==True)
      XFreePixmap(GetDisplay(),backgroundpixmap);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,xpostext,height+2+fontst->ascent,
		     text,strlen(text));
    if (plusmore==True)
      XDrawImageString(GetDisplay(),backgroundpixmap2,gch,
		       xpostext+twidth+8,height+2+fontst->ascent,
		       "+ more",strlen("+ more"));
    XSetWindowBackgroundPixmap(GetDisplay(),direntry->Iconwindow,backgroundpixmap2);
    shapepixmap2=XCreatePixmap(GetDisplay(),direntry->Iconwindow,
			       nwidth,height+2+fontst->ascent+fontst->descent,1);
    if (activ==1) {
      gc=XCreateGC(GetDisplay(),shapepixmap2,0,0);
      activ++;
    }
    XSetForeground(GetDisplay(),gc,0);
    XFillRectangle(GetDisplay(),shapepixmap2,gc,0,0,
		   nwidth,height+2+fontst->ascent+fontst->descent);
    XSetBackground(GetDisplay(),gc,0);
    XSetForeground(GetDisplay(),gc,1);
    if (shapepixmap==0)
      XFillRectangle(GetDisplay(),shapepixmap2,gc,xposicon,0,width,height);
    else
      XCopyArea(GetDisplay(),shapepixmap,shapepixmap2,gc,0,0,width,height,
		xposicon,0);
    XFillRectangle(GetDisplay(),shapepixmap2,gc,xpostext,height+2,twidth,fontst->ascent+fontst->descent);
    if (plusmore==True)
      XFillRectangle(GetDisplay(),shapepixmap2,gc,xpostext+twidth+8,height+2,
		     XTextWidth(fontst,"+ more",strlen("+ more")),fontst->ascent+fontst->descent);
    if (direntry->type>=2)
      XFillRectangle(GetDisplay(),shapepixmap2,gc,xpostext+twidth,height-5,11,11);
    XShapeCombineMask(GetDisplay(), direntry->Iconwindow, ShapeBounding, 0, 0, 
		      shapepixmap2, ShapeSet);
    
    XFreePixmap(GetDisplay(),backgroundpixmap2);
    XFreePixmap(GetDisplay(),shapepixmap2);
    
    if (pos==NULL) {
      PositionArrange(direntry,nwidth,height+2+fontst->ascent+fontst->descent);
      SendFileInfoToDesktop(fullname,NULL,icon,start);
    } else {
      GetPositionFromText(pos,&x,&y,NULL);
      XMoveWindow(GetDisplay(),direntry->Iconwindow,x,y);
    }
    if (GetIsDesktop()==True)
      XSelectInput(GetDisplay(),direntry->Iconwindow,PointerMotionMask|
		   EnterWindowMask|ButtonReleaseMask|ButtonPressMask|ExposureMask);
    else
      XSelectInput(GetDisplay(),direntry->Iconwindow,PointerMotionMask|
		   ButtonReleaseMask|ButtonPressMask);
    XResizeWindow(GetDisplay(),direntry->Iconwindow,nwidth,height+2+fontst->ascent+fontst->descent);
    free(text);
  } else if (GetKindOfWindow()==DETAIL) {
    backgroundpixmap2=XCreatePixmap(GetDisplay(),direntry->Iconwindow,
				    732,15,XDefaultDepth(GetDisplay(),
							    XDefaultScreen(GetDisplay())));
    if (direntry->mark==False) {
      XSetForeground(GetDisplay(),gch,GetBGColor());
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,0,0,732,15);
      XSetBackground(GetDisplay(),gch,GetBGColor());
      XSetForeground(GetDisplay(),gch,GetFGColor());
    } else {
      XSetForeground(GetDisplay(),gch,GetFGColor());
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,0,0,732,15);
      XSetBackground(GetDisplay(),gch,GetFGColor());
      XSetForeground(GetDisplay(),gch,GetBGColor());
    }      
    text=malloc(13);
    sprintf(text,"%11ld",direntry->info.st_size);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,0,12,text,strlen(text));
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,0,0,0,14);
    free(text);
    text=malloc(strlen(direntry->filename)+1);
    strcpy(text,direntry->filename);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,96,12,text,strlen(text));
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,92,0,92,14);
    free(text);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,296,12,"                   ",19);
    username=getpwuid(direntry->info.st_uid);
    if (username!=NULL)
      XDrawImageString(GetDisplay(),backgroundpixmap2,gch,304,12,username->pw_name,strlen(username->pw_name));
    else {
      text=malloc(13);
      sprintf(text,"%d",(int)direntry->info.st_uid);
      XDrawImageString(GetDisplay(),backgroundpixmap2,gch,304,12,text,strlen(text));
      free(text);
    }
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,300,0,300,14);
    groupname=getgrgid(direntry->info.st_gid);
    if (groupname!=NULL) 
      XDrawImageString(GetDisplay(),backgroundpixmap2,gch,376,12,groupname->gr_name,strlen(groupname->gr_name));
    else {
      text=malloc(13);
      sprintf(text,"%d",(int)direntry->info.st_gid);
      XDrawImageString(GetDisplay(),backgroundpixmap2,gch,376,12,text,strlen(text));
      free(text);
    }
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,372,0,372,14);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,448,12,ctime(&(direntry->info.st_mtime)),24);
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,444,0,444,14);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,640,12,"          ",10);
    text=malloc(11);
    um=direntry->info.st_mode & S_IFMT;
    if (direntry->type>=2) strcpy(text,"l");
    else if (um == S_IFSOCK) strcpy(text,"s");
    else if (um == S_IFREG) strcpy(text,"-");
    else if (um == S_IFDIR) strcpy(text,"d");
    else if (um == S_IFBLK) strcpy(text,"b");
    else if (um == S_IFCHR) strcpy(text,"c");
    else if (um == S_IFCHR) strcpy(text,"f");
    um=direntry->info.st_mode;
    if ((um & S_IRUSR)==0) strcat(text,"-");
    else strcat(text,"r");
    if ((um & S_IWUSR)==0) strcat(text,"-");
    else strcat(text,"w");
    if ((um & S_IXUSR)==0) strcat(text,"-");
    else {
      if ((um & S_ISUID)==0) strcat(text,"x");
      else strcat(text,"s");
    }
    if ((um & S_IRGRP)==0) strcat(text,"-");
    else strcat(text,"r");
    if ((um & S_IWGRP)==0) strcat(text,"-");
    else strcat(text,"w");
    if ((um & S_IXGRP)==0) strcat(text,"-");
    else {
      if ((um & S_ISGID)==0) strcat(text,"x");
      else strcat(text,"s");
    }
    if ((um & S_IROTH)==0) strcat(text,"-");
    else strcat(text,"r");
    if ((um & S_IWOTH)==0) strcat(text,"-");
    else strcat(text,"w");
    if ((um & S_IXOTH)==0) strcat(text,"-");
    else strcat(text,"x");
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,648,12,text,10);
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,644,0,644,14);    
    XDrawLine(GetDisplay(),backgroundpixmap2,gch,731,0,731,14);    
    XSetWindowBackgroundPixmap(GetDisplay(),direntry->Iconwindow,backgroundpixmap2);
    XFreePixmap(GetDisplay(),backgroundpixmap2);
    SendFileInfoToDesktop(fullname,NULL,icon,start);
    XSelectInput(GetDisplay(),direntry->Iconwindow,PointerMotionMask|
		 ButtonReleaseMask|ButtonPressMask);
    if (newicon==True)
      PositionArrange(direntry,732,15);      
    XResizeWindow(GetDisplay(),direntry->Iconwindow,732,15);
    free(text);
  } else {
    if (littlefolder==0) {
      DataPixmap(littlefolder_xpm,&littlefolder,&littlefolder_shape);
      DataPixmap(littlelinkfolder_xpm,&littlelinkfolder,NULL);
    }
    text=malloc(strlen(GetPath())+strlen(direntry->filename)+1);
    strcpy(text,GetPath());
    strcat(text,direntry->filename);
    if (strcmp("/",text)==0) { 
      text2=malloc(2);
      strcpy(text2,"/");
    } else if (strcmp("~/",text)==0) { 
      text2=malloc(3);
      strcpy(text2,"~/");
    } else {
      text2=malloc(strlen(GetPath())+strlen(direntry->filename)+1);
      i=0;
      do {
	if (text[i]=='/')
	  strcpy(text2,text+i+1);
	i++;
      } while (text[i]!='/' || text[i+1]!=0);
      text2[strlen(text2)-1]=0;
    }
    nwidth=XTextWidth(fontst,text2,strlen(text2));
    backgroundpixmap2=XCreatePixmap(GetDisplay(),direntry->Iconwindow,
				    18+nwidth,2+fontst->ascent+fontst->descent,XDefaultDepth(GetDisplay(),
							    XDefaultScreen(GetDisplay())));
    XSetFunction(GetDisplay(),gch,GXcopy);
    shapepixmap2=XCreatePixmap(GetDisplay(),direntry->Iconwindow,
			       18+nwidth,2+fontst->ascent+fontst->descent,1);
    if (activ==1) {
      gc=XCreateGC(GetDisplay(),shapepixmap2,0,0);
      activ++;
    }
    XSetForeground(GetDisplay(),gc,0);
    XFillRectangle(GetDisplay(),shapepixmap2,gc,0,0,
		   18+nwidth,2+fontst->ascent+fontst->descent);
    XSetForeground(GetDisplay(),gc,1);
    XFillRectangle(GetDisplay(),shapepixmap2,gc,18,0,
		   nwidth,2+fontst->ascent+fontst->descent);
    XCopyArea(GetDisplay(),littlefolder_shape,shapepixmap2,gc,
		0,0,15,13,1,(2+fontst->ascent+fontst->descent)/2-7);
    if (direntry->mark==False) {
      XSetForeground(GetDisplay(),gch,GetBGColor());
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,18,0,nwidth,2+fontst->ascent+fontst->descent);
      XSetBackground(GetDisplay(),gch,GetBGColor());
      XSetForeground(GetDisplay(),gch,GetFGColor());
    } else {
      XSetForeground(GetDisplay(),gch,GetFGColor());
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,18,0,nwidth,2+fontst->ascent+fontst->descent);
      XSetBackground(GetDisplay(),gch,GetFGColor());
      XSetForeground(GetDisplay(),gch,GetBGColor());
    }      
    if (direntry->mark==True) {
      XFillRectangle(GetDisplay(),backgroundpixmap2,gch,0,0,18+nwidth,2+fontst->ascent+fontst->descent);
      XSetFunction(GetDisplay(),gch,GXxor);
    }
    if (direntry->type==3)
      XCopyArea(GetDisplay(),littlelinkfolder,backgroundpixmap2,gch,
		0,0,15,13,1,(2+fontst->ascent+fontst->descent)/2-7);
    else
      XCopyArea(GetDisplay(),littlefolder,backgroundpixmap2,gch,
		0,0,15,13,1,(2+fontst->ascent+fontst->descent)/2-7);
    XSetFunction(GetDisplay(),gch,GXcopy);
    XDrawImageString(GetDisplay(),backgroundpixmap2,gch,18,1+fontst->ascent,text2,strlen(text2));
    XSetWindowBackgroundPixmap(GetDisplay(),direntry->Iconwindow,backgroundpixmap2);
    XShapeCombineMask(GetDisplay(), direntry->Iconwindow, ShapeBounding, 0, 0, 
		      shapepixmap2, ShapeSet);
    XFreePixmap(GetDisplay(),backgroundpixmap2);
    XFreePixmap(GetDisplay(),shapepixmap2);
    SendFileInfoToDesktop(fullname,NULL,icon,start);
    XSelectInput(GetDisplay(),direntry->Iconwindow,PointerMotionMask|
		 ButtonReleaseMask|ButtonPressMask);
    XResizeWindow(GetDisplay(),direntry->Iconwindow,18+nwidth,2+fontst->ascent+fontst->descent);
    free(text2);
    free(text);
  }
  if (GetIsDesktop()==False)
    XMapRaised(GetDisplay(),direntry->Iconwindow);
  else
    XMapWindow(GetDisplay(),direntry->Iconwindow);
  if (newicon==False)
    XClearWindow(GetDisplay(),direntry->Iconwindow);
  free(fullname);
  free(pos);
  free(icon);
  free(start);
}

