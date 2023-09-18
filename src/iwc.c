/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <Xlib.h>
#include <Xatom.h>
#include <Xutil.h>
#include <ClientWin.h>
#include <malloc.h>
#include <stdio.h>
#include "iwc.h"
#include <string.h>
#include "global.h"
#include "namedemultiplexer.h"
#include "iconmanager.h"
#include "filemanager.h"
#include "windowname.h"
#include "extension.h"
#include "shell.h"
#include <stdlib.h>
#ifdef GZIP
#include <zlib.h>
#endif

Window win=0;
Window deskwin=0;

int IsDesktopActive()  
{
  Window *desktopwindow;
  desktopwindow=GetWindowNamed(".dfmdesk --- dfm:~/.dfmdesk/");
  if (desktopwindow==NULL)
    return(False);
  else {
    deskwin=*desktopwindow;
    return(True);
  }
}

Window *GetWindowNamedRecursive(char *windowname,Window search)
{
  int i;
  Window root,parent;
  Window *children=NULL;
  Window *NextRecWin;
  unsigned int nchildren;
  char *childname;

  XQueryTree(GetDisplay(),search,&root,&parent,
		    &children,&nchildren);
  i=0;
  while (i<nchildren) {
    XFree(children);
    children=NULL;
    XQueryTree(GetDisplay(),search,&root,&parent,
	       &children,&nchildren);
    XFetchName(GetDisplay(),children[i],&childname);
    if (childname!=NULL) {
      if (strcmp(windowname,childname)==0) {
	win=children[i];
	i=nchildren;
      }
      XFree(childname);
    } else {
      NextRecWin=GetWindowNamedRecursive(windowname,children[i]);
      if (NextRecWin!=NULL) {
	i=nchildren;
      } 
    }      
    i++;
  }
  XFree(children);
  if (i==nchildren)
    return(NULL);
  else
    return(&win);
}

Window* GetWindowNamed(char *windowname)
{
  Window* win;

  XGrabServer(GetDisplay());
  win=GetWindowNamedRecursive(windowname,XDefaultRootWindow(GetDisplay()));
  XUngrabServer(GetDisplay());
  return(win);
}

Window GetGreatestParent(Window w)
{
  Window last;
  Window this;
  Window root;
  Status status;
  Window *children;
  unsigned int nchildren;
  this=w;
  do {
    last=this;
    status=XQueryTree(GetDisplay(),last,&root,&this,&children,&nchildren);
    if (status) XFree(children);
  } while (this!=root);
  return(last);
}

void SendFiles(XEvent evnt,int x,int y,char *filenames)
{
  Atom DndProtocol,DndSelection;
  Atom OldDndProtocol,OldDndSelection;
  XEvent xcl;
  XEvent offix;
  char *offixnames,*offixtemp,*offixsend;
  int i,j,old,counter,ex;
  char *text;

  Window new;
  Window other;
  int xw,yw;
  int xr,yr;
  unsigned int keys;

  XStoreBytes(GetDisplay(),filenames,strlen(filenames));
  xcl.type=ClientMessage;
  xcl.xclient.display=GetDisplay();
  xcl.xclient.window=GetMoveWindow();
  xcl.xclient.message_type=XInternAtom(GetDisplay(),"_DFMMESSAGE",False);
  xcl.xclient.format=16;
  xcl.xclient.data.s[0]=SEND;
  xcl.xclient.data.s[1]=(short)x;
  xcl.xclient.data.s[2]=(short)y;
  xcl.xclient.data.s[3]=(short)evnt.xbutton.x_root;
  xcl.xclient.data.s[4]=(short)evnt.xbutton.y_root;

  XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),&other,
		&new,&xr,&yr,&xw,&yw,&keys);

  if (new!=None) {
    XSendEvent(GetDisplay(),PointerWindow,False,NoEventMask,&xcl);
  } else {
    XSendEvent(GetDisplay(),deskwin,False,NoEventMask,&xcl);
  }

  /*offix begin*/
  if (new!=None) {
    new=XmuClientWindow(GetDisplay(),new);
    
    DndProtocol=XInternAtom(GetDisplay(),"_DND_PROTOCOL",False);
    DndSelection=XInternAtom(GetDisplay(),"_DND_SELECTION",False);
    OldDndProtocol=XInternAtom(GetDisplay(),"DndProtocol",False);
    OldDndSelection=XInternAtom(GetDisplay(),"DndSelection",False);
    
    offixnames=strdup(filenames);
    counter=0;
    i=0;
    while (offixnames[i]!=0) {
      if (offixnames[i]==' ')
	counter++;
      if (offixnames[i]=='\\')
	i++;
      i++;
    }
    offixtemp=malloc(strlen(filenames)+counter*strlen(getenv("HOME"))+1);
    offixtemp[0]=0;
    ex=False;
    old=1;
    i=1;
    do {
      if (offixnames[i]==0)
	ex=True;
      if (offixnames[i]==' ' || offixnames[i]==0) {
	offixnames[i]=0;
	text=ExpandTilde(offixnames+old);
	strcat(offixtemp,text);
	strcat(offixtemp," ");
	free(text);
	old=i+1;
      }
      if (offixnames[i]=='\\')
	i++;
      i++;
    } while (ex==False);
    
    offixsend=DeSlashName(offixtemp);
    j=0;
    i=0;
    while (offixtemp[i]!=0) {
      if (offixtemp[i]==' ')
	offixsend[j]=0;
      if (offixtemp[i]=='\\')
	i++;
      i++;j++;
    }
    free(offixtemp);
    free(offixnames);
    
    offixnames=DeSlashName(filenames+1);
    
    if (counter==1)
      j--;
    XChangeProperty(GetDisplay(),XDefaultRootWindow(GetDisplay()),DndSelection,XA_STRING,8,
		    PropModeReplace,(unsigned char*)offixsend,j+1); 
    XChangeProperty(GetDisplay(),XDefaultRootWindow(GetDisplay()),OldDndSelection,XA_STRING,8,
		    PropModeReplace,(unsigned char*)offixsend,j+1); 
    free(offixsend);
    offix.type=ClientMessage;
    offix.xclient.display=GetDisplay();
    offix.xclient.window=new;
    offix.xclient.message_type=DndProtocol;
    offix.xclient.format=32;
    if (counter==1)
      offix.xclient.data.l[0]=DndFile;
    else
      offix.xclient.data.l[0]=DndFiles;
    offix.xclient.data.l[1]=(long)evnt.xbutton.state;
    offix.xclient.data.l[2]=(long)GetMoveWindow();
    offix.xclient.data.l[3]=(long)evnt.xbutton.x_root+65536*evnt.xbutton.y_root;
    offix.xclient.data.l[4]=1;
    
    XSendEvent(GetDisplay(),new,True,NoEventMask,&offix);
    
    offix.xclient.message_type=OldDndProtocol;
    
    XSendEvent(GetDisplay(),new,True,NoEventMask,&offix);
  }
  /*offix end*/
  XFlush(GetDisplay());
}

void ManageAutofocus(XEvent *event)
{
  Window new;
  Window other;
  int xw,yw;
  int xr,yr;
  unsigned int keys;
  event->xcrossing.window=XDefaultRootWindow(GetDisplay());
  XSendEvent(GetDisplay(),XDefaultRootWindow(GetDisplay()),False,
	     EnterWindowMask,event);
  XFlush(GetDisplay());
  XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),&other,
		&new,&xr,&yr,&xw,&yw,&keys);
  if (GetWindow()!=new) {
    if (GetDirentryFromWindow(new)==NULL) {
      event->xcrossing.window=new;
      XSendEvent(GetDisplay(),new,False,EnterWindowMask,event);    
      XFlush(GetDisplay());
    }      
  }
}  

void SendFontName(char *name)
{
  char* text; 
  Atom namedatom;

  text=malloc(strlen("_DFMICONFONT")+1);
  strcpy(text,"_DFMICONFONT");
  namedatom=XInternAtom(GetDisplay(),text,False);
  XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		  (unsigned char*)name,strlen(name));
  free(text);
}

void UpdateFontName()
{
  char* text;
  char* back;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  text=malloc(strlen("_DFMICONFONT")+1);
  strcpy(text,"_DFMICONFONT");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    SetFontName(NULL);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      back=malloc(length+1);
      strncpy(back,(char*)propret,length);
      (back)[length]=0;
      XFree(propret);
      SetFontName(back);
    } else
      SetFontName(NULL);
  }
  free(text);
}

void SendColors(int r1,int g1,int b1,
		int r2,int g2,int b2,
		int r3,int g3,int b3,
		int r4,int g4,int b4)
{
  char* text; 
  Atom namedatom;
  char a[12*4];
  sprintf(a,"%d %d %d %d %d %d %d %d %d %d %d %d",
	  r1,g1,b1,
	  r2,g2,b2,
	  r3,g3,b3,
	  r4,g4,b4);
  text=malloc(strlen("_DFMCOLORS")+1);
  strcpy(text,"_DFMCOLORS");
  namedatom=XInternAtom(GetDisplay(),text,False);
  XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		  (unsigned char*)a,strlen(a));
  free(text);
}

void UpdateColors()
{
  char* text;
  char* backs;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int r1,g1,b1;
  int r2,g2,b2;
  int r3,g3,b3;
  int r4,g4,b4;
  text=malloc(strlen("_DFMCOLORS")+1);
  strcpy(text,"_DFMCOLORS");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    SetColors(0,0,0,
	      255,255,255,
	      0,0,0,
	      255,255,255);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0) {
      backs=malloc(length+1);
      strncpy(backs,(char*)propret,length);
      (backs)[length]=0;
      sscanf(backs,"%d %d %d %d %d %d %d %d %d %d %d %d",
	     &r1,&g1,&b1,
	     &r2,&g2,&b2,
	     &r3,&g3,&b3,
	     &r4,&g4,&b4);
      SetColors(r1,g1,b1,
		r2,g2,b2,
		r3,g3,b3,
		r4,g4,b4);
      XFree(propret);
      free(backs);
    } else
      SetColors(0,0,0,
		255,255,255,
		0,0,0,
		255,255,255);
  }
  free(text);
}

void SetWindowCache(int status)
{
  char* text; 
  Atom namedatom;
  char a[10];
  text=malloc(strlen("_DFMCACHE")+1);
  strcpy(text,"_DFMCACHE");
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (status!=0) {
    sprintf(a,"%d",status);
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)a,strlen(a));
  } else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

int GetWindowCache()
{
  char* text;
  char* backs;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int back=0;
  text=malloc(strlen("_DFMCACHE")+1);
  strcpy(text,"_DFMCACHE");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    return(0);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      backs=malloc(length+1);
      strncpy(backs,(char*)propret,length);
      (backs)[length]=0;
      if (strcmp("True",backs)==0)
	back=180;
      else
	sscanf(backs,"%d",&back);
      XFree(propret);
      free(backs);
    }
  }
  free(text);
  return(back);  
}

void SetMainBackground(int status)
{
  char* text; 
  Atom namedatom;
  text=malloc(strlen("_DFMMAINBACK")+1);
  strcpy(text,"_DFMMAINBACK");
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (status==False) {
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)"False",strlen("False"));
  } else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

int GetMainBackground()
{
  char* text;
  char* backs;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int back=True;
  text=malloc(strlen("_DFMMAINBACK")+1);
  strcpy(text,"_DFMMAINBACK");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    return(True);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      backs=malloc(length+1);
      strncpy(backs,(char*)propret,length);
      (backs)[length]=0;
      if (strcmp("False",backs)==0)
	back=False;
      XFree(propret);
      free(backs);
    }
  }
  free(text);
  return(back);  
}

void SendTimer(long timer)
{
  char* text; 
  Atom namedatom;
  char a[10];
  text=malloc(strlen("_DFMTIMER")+1);
  strcpy(text,"_DFMTIMER");
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (timer!=40) {
    sprintf(a,"%d",(int)timer);
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)a,strlen(a));
  } else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

void UpdateTimer()
{
  char* text;
  char* backs;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int back=40;
  text=malloc(strlen("_DFMTIMER")+1);
  strcpy(text,"_DFMTIMER");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    SetTimer(40);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      backs=malloc(length+1);
      strncpy(backs,(char*)propret,length);
      (backs)[length]=0;
      sscanf(backs,"%d",&back);
      XFree(propret);
      free(backs);
    }
  }
  free(text);
  SetTimer((long)back);  
}

void SetDefaultBackground(char* defback)
{
  char* text; 
  Atom namedatom;
  text=malloc(strlen("_DFMDEFBACK")+1);
  strcpy(text,"_DFMDEFBACK");
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (defback!=NULL && defback[0]!=0) {
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)defback,strlen(defback));
  } else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

char* GetDefaultBackground()
{
  char* text;
  char* backs=NULL;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  text=malloc(strlen("_DFMDEFBACK")+1);
  strcpy(text,"_DFMDEFBACK");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    return(NULL);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      backs=malloc(length+1);
      strncpy(backs,(char*)propret,length);
      (backs)[length]=0;
      XFree(propret);
    }
  }
  free(text);  
  return(backs);
}

void LoadFileInfoToDesktop()
{
  int i,test;
  char *text,*text2;
#ifdef GZIP
  gzFile pfLoad;
#else
  FILE *pfLoad;
#endif
  Atom namedatom;
  deskwin=GetWindow();
  SendFileInfoToDesktop(".dfmdesk --- dfm:~/.dfmdesk/"," 5 5 0",
			NULL," 5 5");
#ifdef GZIP
  if ((pfLoad = gzopen(".dfminfo", "r")) != NULL) {
#else
  if ((pfLoad = fopen(".dfminfo", "r")) != NULL) {
#endif
    text=malloc(1000);
    text2=malloc(1000);
    do {
      i=-1;
      do {
	i++;
#ifdef GZIP
        test=gzread(pfLoad, &text[i], 1);
#else
        test=fgetc(pfLoad);
        text[i]=(char)test;
        if (test!=EOF) test=1;
#endif
      } while (test==1 && text[i]!=';' && text[i]!=10);
      text[i]=0;
      if (test==1) {
	i=-1;
	do {
	i++;
#ifdef GZIP
        test=gzread(pfLoad, &text2[i], 1);
#else
        test=fgetc(pfLoad);
        text2[i]=(char)test;
        if (test!=EOF) test=1;
#endif
      } while (test==1 && text2[i]!=';' && text2[i]!=10);
	text2[i]=0;
	namedatom=XInternAtom(GetDisplay(),text,False);
	XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,
			PropModeReplace,(unsigned char*)text2,strlen(text2)+1);
      }
    } while (test==1);
    free(text);
    free(text2);
#ifdef GZIP
    gzclose(pfLoad);
#else
    fclose(pfLoad);
#endif
  }
}

#ifdef GZIP
void gzprintf(gzFile pfSave,char *text,char *textN)
{
   char t[1000];
   sprintf(t,"%s;%s\n",text,textN);
   gzwrite(pfSave,t,strlen(t));
}
#else
void gzprintf(FILE *pfSave,char *text,char *textN)
{
   fprintf(pfSave,"%s;%s\n",text,textN);
}
#endif


void SaveFileInfoFromDesktop()
{
  FILE *dir;
  int num,i,j;
  struct item *dirsave;
  struct item *direntry;
  Atom *properties;
  char *name;
  char *path,*ownpath,*windowname;
  char *text,*textN;
  Atom dummyatom,namedatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
#ifdef GZIP
  gzFile pfSave;
  if ((pfSave = gzopen(".dfminfo", "w")) != NULL) {
#else
  FILE *pfSave;
  if ((pfSave = fopen(".dfminfo", "w")) != NULL) {
#endif
    properties=XListProperties(GetDisplay(),deskwin,&num);
    for (i=0;i<num;i++) {
      
      name=XGetAtomName(GetDisplay(),properties[i]);
      
      if (strncmp(name,"_DFMICONFONT",strlen("_DFMICONFONT"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMTIMER",strlen("_DFMTIMER"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMCOLORS",strlen("_DFMCOLORS"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMMAINBACK",strlen("_DFMMAINBACK"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMDEFBACK",strlen("_DFMDEFBACK"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMCACHE",strlen("_DFMCACHE"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  text=malloc(length+1);
	  strncpy(text,(char*)propret,length);
	  text[length]=0;
	  XFree(propret);
	  gzprintf(pfSave,name,text);
	  free(text);
	}
      } else if (strncmp(name,"_DFMSAVEW_",strlen("_DFMSAVEW_"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  path=malloc(length+1);
	  strncpy(path,(char*)propret,length);
	  path[length]=0;
	  XFree(propret);
	  ownpath=malloc(strlen(path)+1);
	  strcpy(ownpath,path);
	  if (ownpath[0]=='~')
	    ownpath[0]='.';
	  if ((dir=fopen(ownpath,"r"))!=NULL) {
	    fclose(dir);
	    
	    gzprintf(pfSave,name,path);
	    
	    for (j=NORMAL;j<=STRUCTURE;j++) {
	      windowname=GenerateWindowName(path,j);
	      text=malloc(strlen(windowname)+12);
	      
	      strcpy(text,"_DFM_");
	      strcat(text,windowname);
	      strcat(text,"_POS");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      strcpy(text,"_DFM_");
	      strcat(text,windowname);
	      strcat(text,"_ICON");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      strcpy(text,"_DFM_");
	      strcat(text,windowname);
	      strcat(text,"_START");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      free(text);
	      free(windowname);
	    }
	  }
	  free(path);free(ownpath);
	}
      } else if (strncmp(name,"_DFMSAVEI_",strlen("_DFMSAVEI_"))==0) {
	XGetWindowProperty(GetDisplay(),deskwin,properties[i],0,8192,False,XA_STRING,
			   &dummyatom,&dummyformat,&length,&dummy,&propret);
	if (length!=0){
	  path=malloc(length+1);
	  strncpy(path,(char*)propret,length);
	  path[length]=0;
	  XFree(propret);
	  ownpath=malloc(strlen(path)+1);
	  strcpy(ownpath,path);
	  if (ownpath[0]=='~')
	    ownpath[0]='.';
	  if ((dir=fopen(ownpath,"r"))!=NULL) {
	    fclose(dir);
	    
	    gzprintf(pfSave,name,path);
	    
	    dirsave=GetDirectory(path);
	    
	    direntry=dirsave;
	    
	    while(direntry!=NULL) {
	      /*tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt*/	      
	      text=malloc(strlen(path)+strlen(direntry->filename)+12);
	      strcpy(text,"_DFM_");
	      strcat(text,path);
	      strcat(text,direntry->filename);
	      strcat(text,"_POS");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      strcpy(text,"_DFM_");
	      strcat(text,path);
	      strcat(text,direntry->filename);
	      strcat(text,"_ICON");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      strcpy(text,"_DFM_");
	      strcat(text,path);
	      strcat(text,direntry->filename);
	      strcat(text,"_START");
	      if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
		XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
				   &dummyatom,&dummyformat,&length,&dummy,&propret);
		if (length!=0){
		  textN=malloc(length+1);
		  strncpy(textN,(char*)propret,length);
		  textN[length]=0;
		  XFree(propret);
		  gzprintf(pfSave,text,textN);
		  free(textN);
		}      
	      }
	      
	      
	      free(text);
	      /*tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt*/	      
	      direntry=direntry->NextItem;
	    }
	    
	    while(dirsave!=NULL) {
	      direntry=dirsave->NextItem;
	      free(dirsave->filename);
	      free(dirsave);
	      dirsave=direntry;
	    }
	  }
	  free(path);free(ownpath);
	}
      }
      
      XFree(name);
    }
    XFree(properties);
#ifdef GZIP
    gzclose(pfSave);
#else
    fclose(pfSave);
#endif
  }
}

int GetIconSaveStatus(char *path)
{
  char* text;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int back=False;
  text=malloc(strlen("_DFMSAVEI_")+strlen(path)+1);
  strcpy(text,"_DFMSAVEI_");
  strcat(text,path);
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    return(False);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      back=True;
      XFree(propret);
    }
  }
  free(text);
  return(back);
}

void SetIconSaveStatus(char* path,int status)
{
  char* text;
  Atom namedatom;
  text=malloc(strlen("_DFMSAVEI_")+strlen(path)+1);
  strcpy(text,"_DFMSAVEI_");
  strcat(text,path);
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (status==True)
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)path,strlen(path));
  else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

int GetWindowSaveStatus(char* path)
{
  char* text;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  int back=False;
  text=malloc(strlen("_DFMSAVEW_")+strlen(path)+1);
  strcpy(text,"_DFMSAVEW_");
  strcat(text,path);
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0)
    return(False);
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      back=True;
      XFree(propret);
    }
  }
  free(text);
  return(back);
}

void SetWindowSaveStatus(char* path,int status)
{
  char* text; 
  Atom namedatom;

  text=malloc(strlen("_DFMSAVEW_")+strlen(path)+1);
  strcpy(text,"_DFMSAVEW_");
  strcat(text,path);
  namedatom=XInternAtom(GetDisplay(),text,False);
  if (status==True)
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)path,strlen(path));
  else
    XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
}

void GetIconPixmap(char* iconname,Pixmap *iconpixmap,Pixmap *shapepixmap)
{
  char* text;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  Window goal;

  if (GetIsDesktop()==True)
    goal=GetMoveWindow();
  else
    goal=GetWindow();
  text=malloc(strlen("_DFMICON_")+strlen(iconname)+1);
  strcpy(text,"_DFMICON_");
  strcat(text,iconname);
  *iconpixmap=None;
  *shapepixmap=None;
  if ((namedatom=XInternAtom(GetDisplay(),text,True))!=0) {
    XGetWindowProperty(GetDisplay(),goal,namedatom,0,2,False,XA_PIXMAP,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0) {
     *iconpixmap=((Pixmap*)propret)[0];
    *shapepixmap=((Pixmap*)propret)[1];
      XFree(propret);
    }
  }
  free(text);
}

void SetIconPixmap(char* iconname,Pixmap iconpixmap,Pixmap shapepixmap)
{
  char* text; 
  Atom namedatom;
  Pixmap p[2];
  Window goal;

  if (GetIsDesktop()==True)
    goal=GetMoveWindow();
  else
    goal=GetWindow();
  p[0]=iconpixmap;
  p[1]=shapepixmap;
  text=malloc(strlen("_DFMICON_")+strlen(iconname)+1);
  strcpy(text,"_DFMICON_");
  strcat(text,iconname);
  namedatom=XInternAtom(GetDisplay(),text,False);
    XChangeProperty(GetDisplay(),goal,namedatom,XA_PIXMAP,32,PropModeReplace,
		    (unsigned char*)p,2);
  free(text);
}

void EraseIconPixmaps()
{
  Atom *properties;
  char *name;
  int num,i;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  Window goal;
  if (GetIsDesktop()==True)
    goal=GetMoveWindow();
  else
    goal=GetWindow();
  properties=XListProperties(GetDisplay(),goal,&num);
  for (i=0;i<num;i++) {
    name=XGetAtomName(GetDisplay(),properties[i]);
    if (strncmp(name,"_DFMICON_",strlen("_DFMICON_"))==0) {
      XGetWindowProperty(GetDisplay(),goal,properties[i],0,2,False,XA_PIXMAP,
			 &dummyatom,&dummyformat,&length,&dummy,&propret);
      XDeleteProperty(GetDisplay(),goal,properties[i]);
      if (length!=0){
	XFreePixmap(GetDisplay(),((Pixmap*)propret)[0]);
	if (((Pixmap*)propret)[1]!=None)
	  XFreePixmap(GetDisplay(),((Pixmap*)propret)[1]);
	XFree(propret);
      }
    }
    XFree(name);
  }
  XFree(properties);
}

void GetFileInfoFromDesktop(char *filenameorg,char **position,char **icon,char **start)
{
  char *text,*filename;
  Atom namedatom;
  Atom dummyatom;
  int dummyformat;
  unsigned char *propret;
  unsigned long length;
  unsigned long dummy;
  filename=malloc(strlen(filenameorg)+1);
  if (filenameorg[0]==' ')
    strcpy(filename,filenameorg+1);
  else
    strcpy(filename,filenameorg);    

  GetLinkNameWithoutLast(&filename);
  text=malloc(strlen(filename)+12);
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_POS");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    *position=NULL;
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      *position=malloc(length+1);
      strncpy(*position,(char*)propret,length);
      (*position)[length]=0;
      XFree(propret);
    } else
      *position=NULL;      
  }
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_ICON");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) 
    *icon=NULL;
  else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      *icon=malloc(length+1);
      strncpy(*icon,(char*)propret,length);
      (*icon)[length]=0;
      XFree(propret);
    } else
      *icon=NULL;      
  }
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_START");
  if ((namedatom=XInternAtom(GetDisplay(),text,True))==0) {
    *start=NULL;
  } else {
    XGetWindowProperty(GetDisplay(),deskwin,namedatom,0,8192,False,XA_STRING,
		       &dummyatom,&dummyformat,&length,&dummy,&propret);
    if (length!=0){
      *start=malloc(length+1);
      strncpy(*start,(char*)propret,length);
      (*start)[length]=0;
      XFree(propret);
    } else
      *start=NULL;      
  }
  free(text);
  free(filename);
}

void SendFileInfoToDesktop(char *filenameorg,char *position,char *icon,
			   char *start)
{
  char *text,*filename;
  Atom namedatom;
  filename=malloc(strlen(filenameorg)+1);
  strcpy(filename,filenameorg);
  GetLinkNameWithoutLast(&filename);

  text=malloc(strlen(filename)+12);
  if (position!=NULL) {
    strcpy(text,"_DFM_");
    strcat(text,filename);
    strcat(text,"_POS");
    namedatom=XInternAtom(GetDisplay(),text,False);
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)position,strlen(position));
  }
  if (icon!=NULL) {
    strcpy(text,"_DFM_");
    strcat(text,filename);
    strcat(text,"_ICON");
    namedatom=XInternAtom(GetDisplay(),text,False);
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)icon,strlen(icon));
  }
  if (start!=NULL) {
    strcpy(text,"_DFM_");
    strcat(text,filename);
    strcat(text,"_START");
    namedatom=XInternAtom(GetDisplay(),text,False);
    XChangeProperty(GetDisplay(),deskwin,namedatom,XA_STRING,8,PropModeReplace,
		    (unsigned char*)start,strlen(start));  
  }
  free(text);
  free(filename);
}

void EraseFileInfoInDesktop(char *filenameorg)
{
  char *text,*filename;
  Atom namedatom;

  filename=malloc(strlen(filenameorg)+1);
  strcpy(filename,filenameorg);
  GetLinkNameWithoutLast(&filename);

  text=malloc(strlen(filename)+12);
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_POS");
  namedatom=XInternAtom(GetDisplay(),text,False);
  XDeleteProperty(GetDisplay(),deskwin,namedatom);
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_ICON");
  namedatom=XInternAtom(GetDisplay(),text,False);
  XDeleteProperty(GetDisplay(),deskwin,namedatom);
  strcpy(text,"_DFM_");
  strcat(text,filename);
  strcat(text,"_START");
  namedatom=XInternAtom(GetDisplay(),text,False);
  XDeleteProperty(GetDisplay(),deskwin,namedatom);
  free(text);
  free(filename);
}

void EraseMultipleInfo(char *filenames)
{
  int i=1;
  int j;
  char *text;
  do {
    j=i;
    do {
      j++;
    } while (filenames[j]!=' ' && filenames[j]!=0);
    text=malloc(j-i+1);
    strncpy(text,filenames+i,j-i);
    text[j-i]=0;
    EraseFileInfoInDesktop(text);
    free(text);
    i=j+1;
  } while (filenames[j]!=0);
}

void SendUpdate(char *foldername) 
{
  char *text,*textp,*textl;
  Window *upwin;
  int i,segment;

  /* there is a new function GenerateWindowName, change this*/
  textl=malloc(strlen(GetPath())+strlen(foldername)+1);
  strcpy(textl,GetPath());
  strcat(textl,foldername);
  GetLinkName(&textl);

  textp=malloc(strlen(foldername)+1);
  strcpy(textp,foldername);
  textp[strlen(textp)-1]=0;
  segment=0;
  for (i=0;i<strlen(textp);i++)
    if (textp[i]=='/')
      segment=i+1;

  /*normal view*/
  text=malloc(strlen(textp+segment)+strlen(" --- dfm:")+
	      strlen(textl)+1);
  strcpy(text,textp+segment);
  strcat(text," --- dfm:");
  strcat(text,textl);
  upwin=GetWindowNamed(text);
  if (upwin!=NULL)
    SendWindow(*upwin,UPDATE_LIGHT);
  free(text);
  /*detail view*/
  text=malloc(strlen(textp+segment)+strlen(" -d- dfm:")+
	      strlen(textl)+1);
  strcpy(text,textp+segment);
  strcat(text," -d- dfm:");
  strcat(text,textl);
  upwin=GetWindowNamed(text);
  if (upwin!=NULL)
    SendWindow(*upwin,UPDATE_LIGHT);
  free(text);

  free(textp);
  free(textl);
}

void SendWindow(Window w,int message)
{
  XEvent xcl;
  xcl.type=ClientMessage;
  xcl.xclient.window=GetMoveWindow();
  xcl.xclient.message_type=XInternAtom(GetDisplay(),"_DFMMESSAGE",False);
  xcl.xclient.format=16;
  xcl.xclient.data.s[0]=message;
  XSendEvent(GetDisplay(),w,False,NoEventMask,&xcl);
  XFlush(GetDisplay());
}

void GoalWasFolder(Window Quellwindow,char *foldername,
		     char *filenames,unsigned int key)
{
  int status;
  if ((key&ShiftMask)==ShiftMask) {
    if ((key&ControlMask)==ControlMask)
      CopyCall(0,0,filenames,foldername,REAL,False);
    else
      LinkCall(0,0,filenames,foldername);
  } else {
    if ((key&ControlMask)!=ControlMask) {
      status=CopyCall(0,0,filenames,foldername,NORMAL,True);
      if (status==NO_ERROR) {
	EraseCall(filenames,False,True);
	SendWindow(Quellwindow,UPDATE_LIGHT);
      } else if (status==REPORT_ERROR) {
	EraseCall(filenames,True,True);
	SendWindow(Quellwindow,UPDATE_LIGHT);
      } else
	SendWindow(Quellwindow,UPDATE_LIGHT);
    } else
      CopyCall(0,0,filenames,foldername,NORMAL,False);      
  }
  SendUpdate(foldername);
}

void WorkIncomingCall(XEvent evnt)
{
  struct item *direntry;
  char *pos;
  char *icon;
  char *start;
  char *text;
  Window root,child;
  int dummy1,dummy2,x,y;
  unsigned int key;
  int n;
  int status;
  char *convertfiles;
  char *files;

  convertfiles=XFetchBytes(GetDisplay(),&n);
  files=malloc(n+1);
  strncpy(files,convertfiles,n);
  if (n!=0) 
    XFree(convertfiles);
  files[n]=0;
  if (evnt.xclient.data.s[0]==SEND) {
    if (GetIsDesktop()==True)
      XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),&root,&child,
		    &dummy1,&dummy2,&x,&y,&key);
    else
      XQueryPointer(GetDisplay(),GetMoveWindow(),&root,&child,
		    &dummy1,&dummy2,&x,&y,&key);
    if (child==None || (GetIsDesktop()==True && child==GetWindow())) {
      if (GetKindOfWindow()!=STRUCTURE) {
	if ((key&ShiftMask)==ShiftMask) {
	  if ((key&ControlMask)==ControlMask)
	    CopyCall(x-(int)evnt.xclient.data.s[1],
		     y-(int)evnt.xclient.data.s[2],files,"",REAL,False);
	  else
	    LinkCall(x-(int)evnt.xclient.data.s[1],
		     y-(int)evnt.xclient.data.s[2],files,"");
	} else {
	  if ((key&ControlMask)!=ControlMask) {
	    status=CopyCall(x-(int)evnt.xclient.data.s[1],
			    y-(int)evnt.xclient.data.s[2],files,"",NORMAL,True);
	    if (status==NO_ERROR) {
	      EraseCall(files,False,True);
	      SendWindow(evnt.xclient.window,UPDATE_LIGHT);
	    } else if (status==REPORT_ERROR) {
	      UpdateIcons(True);
	      EraseCall(files,True,True);
	      SendWindow(evnt.xclient.window,UPDATE_LIGHT);
	      UpdateIcons(True);
	    } else {
	      SendWindow(evnt.xclient.window,UPDATE_LIGHT);
	      UpdateIcons(True);
	    }
	  } else
	    status=CopyCall(x-(int)evnt.xclient.data.s[1],
			    y-(int)evnt.xclient.data.s[2],files,"",NORMAL,False);
	}
      }
    } else {
      direntry=GetDirentryFromWindow(child);
      if (direntry!=NULL) {
	if (direntry->filename[strlen(direntry->filename)-1]!='/') {
	  text=malloc(strlen(direntry->filename)+strlen(GetPath())+1);
	  strcpy(text,GetPath());
	  strcat(text,direntry->filename);
	  GetFileInfoFromDesktop(text,&pos,&icon,&start);
	  if (start==NULL || icon==NULL) {
	    free(start);
	    free(icon);
	    GetExtensionParameter(direntry->filename,text,&icon,&start);
	  }
	  ExecuteShellWithFollowLink(text,start,files,0);
	  free(text);
	  free(pos);
	  free(icon);
	  free(start);
	} else {
	  text=strdup(direntry->filename);
	  GoalWasFolder(evnt.xclient.window,text,files,key);
	  free(text);
	}
      }
    }
  }
  if (evnt.xclient.data.s[0]==UPDATE) {
    UpdateIcons(False);
  }
  if (evnt.xclient.data.s[0]==UPDATE_LIGHT) {
    UpdateIcons(True);
  }
  free(files);
}













