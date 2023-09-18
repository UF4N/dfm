/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <X.h>
#include <Xlib.h>
#include <Xutil.h>
#include <Xatom.h>
#include <unistd.h>
#include <cursorfont.h>
#include "global.h"
#include "color.h"
#include "menus.h"
#include "windowname.h"
#include "testinstall.h"
#include "iconmanager.h"
#include "namedemultiplexer.h"
#include "extension.h"
#include "shell.h"
#include "pixmap.h"
#include "gui.h"
#include "string.h" 
#include "iwc.h"
#include "includepixmaps/menuarrow.xpm"
#include "includepixmaps/menuarrow2.xpm"
#include "includepixmaps/ok.xpm"
#include <stdlib.h>


char* maintext[]={"%DFM for X",
		  "-",
		  "*Open",
		  " Options",
		  "-",
		  "*Create",
		  " Update",
		  "!Re-recognize",
		  " Select all",
		  "-",
		  "!Delete",
		  "-",
		  "*Sort",
		  " Arrange",
		  "-end"};
char* sorttext[]={" Type",
		  " Name",
		  " Extension",
		  "-",		   
		  " Date Asc",
		  " Date Desc",
		  "-",		   
		  " Size Asc",
		  " Size Desc",
		  "-end"};
char* ersttext[]={" Folder",
		  " File",
		  "-end"};
char* opentext[]={"$Program",
		  "!XTerm -e ...",
		  "!Without parameters",
		  "-",		   
		  " Normal view",
		  " Detail view",
		  " Structure view",
		  "-",		   
		  " Go up one folder",
		  "-end"};

char* helptextmain[]={" Info",
		      " Help",
		      " XTerm",
		      "-",		   
		      "?Desktop context menu",
		      "-",
		      " Add new program",
		      " Save positions/info",
		      " Create root/home links",
		      " Desktop options",
		      "-",		   
		      " Quit DFM",
		      " Quit X",
		      "-end"};
char* helptextnone[]={" Info",
		      " Help",
		      " XTerm",
		      "-",		   
		      " Kill this window",
		      "-end"};

int inwindow;
char* filenames;
char** shellcommands;
char* iconname;

void CreateText()
{
  char *pos,*icon,*start;
  char *text;
  int i,j;
  text=DeSlashName(filenames);
  GetFileInfoFromDesktop(text,&pos,&icon,&start);
  free(text);
  if (start==NULL || icon==NULL) {
    shellcommands=malloc(sizeof(char*)*2);
    shellcommands[0]=malloc(strlen(" none")+1);
    strcpy(shellcommands[0]," none");
    i=1;
  } else {
    j=NumberOfCmd(start);
    shellcommands=malloc(sizeof(char*)*(j+1));
    for (i=0;i<j;i++) {
      text=ExtractCmd(start,i);
      shellcommands[i]=malloc(strlen(text)+2);
      strcpy(shellcommands[i]," ");
      strcat(shellcommands[i],text);
      free(text);
    }
  }
  shellcommands[i]=malloc(strlen("-end")+1);
  strcpy(shellcommands[i],"-end");
  free(pos);
  free(icon);
  free(start);
}

int GetshnrAndDestroyText(char* shlret)
{
  char *pos,*icon,*start;
  char *text;
  int i,j,back;
  text=DeSlashName(filenames);
  GetFileInfoFromDesktop(text,&pos,&icon,&start);
  free(text);
  if (start==NULL || icon==NULL) {
    free(shellcommands[0]);
    back=0;
    i=1;
  } else {
    j=NumberOfCmd(start);
    back=0;
    for (i=0;i<j;i++) {
      if (shellcommands[i]==shlret)
	back=i;
      free(shellcommands[i]);
    }
  }
  free(shellcommands[i]);
  free(shellcommands);
  free(pos);
  free(icon);
  free(start);
  return(back);
}

char* Menu(Time time,char** text,int xto,int yto,int altxsize,int *shnr)
{
  char* selectedmenu=NULL;
  char* shlret=NULL;
  int done,i;
  XSetWindowAttributes xswa;
  unsigned long valuemask;
  Window selectwin;
  Window buttons[30];
  Cursor selectcur;
  static XEvent event;
  Window presswin,dummy;
  unsigned int width,height;
  unsigned int border,depth;
  int xw,yw,xsize;
  int xr,yr;
  int xnew,ynew;
  unsigned int keys;
  static XFontStruct *fontstruk=NULL;
  static Pixmap morebutton=None;
  static Pixmap morebutton2=None;
  static Pixmap okbutton=None;
  unsigned long darkgrey,grey,white,black;

  darkgrey=GetBestColor(0x8080,0x8080,0x8080);
  grey    =GetBestColor(0xcf3c,0xcf3c,0xcf3c);
  white   =GetBestColor(0xFFFF,0xFFFF,0xFFFF);
  black   =GetBestColor(0x0000,0x0000,0x0000);

  *shnr=0;
  XGetGeometry(GetDisplay(),XDefaultRootWindow(GetDisplay()),&dummy,&xw,&yw,&width,
	       &height,&border,&depth);
  selectcur=XCreateFontCursor(GetDisplay(),XC_hand1);
  if (text==maintext)
    XQueryPointer(GetDisplay(),XDefaultRootWindow(GetDisplay()),&dummy,&presswin,
		  &xr,&yr,&xw,&yw,&keys);
  else {
    xr=xto;
    yr=yto;
  }
  xswa.override_redirect=True;
  xswa.background_pixel=grey;  
  xswa.border_pixel=darkgrey;  
  xswa.event_mask=ButtonReleaseMask|ButtonPressMask;
  valuemask=CWOverrideRedirect|CWBorderPixel|CWBackPixel|CWEventMask;
  selectwin=XCreateWindow(GetDisplay(),DefaultRootWindow(GetDisplay()),
		    0,0,1,1,
		      1,CopyFromParent,InputOutput,CopyFromParent,valuemask,&xswa);
  XSetBackground(GetDisplay(),GetGC(),grey);
  XSetForeground(GetDisplay(),GetGC(),black);
  if (fontstruk==NULL)
    fontstruk=XLoadQueryFont(GetDisplay(),"-*-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*");
  if (fontstruk==NULL)
    fontstruk=XLoadQueryFont(GetDisplay(),"8x13");
  XSetFont(GetDisplay(),GetGC(),fontstruk->fid);
  xsize=0;
  yw=1;
  i=0;
  while(strcmp("-end",text[i])!=0) {
    if (strcmp("-",text[i])!=0) {
      done=XTextWidth(fontstruk,text[i]+1,strlen(text[i]+1));
      if (xsize<done) xsize=done;
      yw=yw+18;
    } else 
      yw=yw+3;
    i++;
  }
  xsize=xsize+40;
  yw++;
  if (xr+xsize>(int)width) { 
    if (text==maintext) 
      xr=(int)width-xsize;
    else
      xr=xr-altxsize-xsize;
  }
  if (text==maintext) {
    if (yr-yw<0) yr=yw;
    XMoveResizeWindow(GetDisplay(),selectwin,xr,yr-yw,xsize,yw);
  } else {
    if (yr+yw>(int)height) yr=(int)height-yw;
    XMoveResizeWindow(GetDisplay(),selectwin,xr,yr,xsize,yw);
  }
  dummy=XCreateSimpleWindow(GetDisplay(),selectwin,
			    0,0,xsize,1,0,
			    white,
			    white
			    );
  XMapRaised(GetDisplay(),dummy);
  dummy=XCreateSimpleWindow(GetDisplay(),selectwin,
			    0,0,1,yw,0,
			    white,
			    white
			    );
  XMapRaised(GetDisplay(),dummy);
  dummy=XCreateSimpleWindow(GetDisplay(),selectwin,
			    0,yw-1,xsize,1,0,
			    black,
			    black
			    );
  XMapRaised(GetDisplay(),dummy);
  dummy=XCreateSimpleWindow(GetDisplay(),selectwin,
			    xsize-1,0,1,yw,0,
			    black,
			    black
			    );
  XMapRaised(GetDisplay(),dummy);
  
  XMapRaised(GetDisplay(),selectwin);
  XGrabPointer(GetDisplay(),selectwin,False,ButtonPressMask|ButtonReleaseMask,
	       GrabModeAsync,GrabModeSync,None,selectcur,time);
  yw=1;
  i=0;
  while(strcmp("-end",text[i])!=0) {
    if (strcmp("-",text[i])!=0) {
      buttons[i]=XCreateSimpleWindow(GetDisplay(),selectwin,
			 1,yw,xsize-1-1,18,0,
			 black,
			 grey
				     );
      XMapRaised(GetDisplay(),buttons[i]);
      if (text[i][0]=='*' || text[i][0]=='$' || text[i][0]=='%' || text[i][0]=='?') {
	dummy=XCreateSimpleWindow(GetDisplay(),buttons[i],
			 xsize-20,2,16,15,0,
			 0,0);
	if (morebutton==None) {
	  DataPixmap(menuarrow_xpm,&morebutton,NULL);
	  DataPixmap(menuarrow2_xpm,&morebutton2,NULL);
	  DataPixmap(ok_xpm,&okbutton,NULL);
	}
	if (text[i][0]=='?') {
	  if (GetMainBackground()==True)
	    XMoveWindow(GetDisplay(),dummy,2,2);
	  else
	    XMoveWindow(GetDisplay(),dummy,-50,2);
	  XSetWindowBackgroundPixmap(GetDisplay(),dummy,okbutton);	  
	} else if (text[i][0]=='*' || text[i][0]=='$')
	  XSetWindowBackgroundPixmap(GetDisplay(),dummy,morebutton);
	else
	  XSetWindowBackgroundPixmap(GetDisplay(),dummy,morebutton2);
	XMapRaised(GetDisplay(),dummy);
      }
      if (text[i]==maintext[10] && inwindow==True &&
	  (strcmp("Trashcan",iconname)==0 ||
	   strcmp("trashcan",iconname)==0 ||
	   strcmp("Papierkorb",iconname)==0)) {
	  XSetForeground(GetDisplay(),GetGC(),black);
	  XDrawImageString(GetDisplay(),buttons[i],GetGC(),20,13,
			   "Delete all",strlen("Delete all"));
      } else {
	if ((inwindow==True && (text[i][0]=='$' || text[i][0]=='!')) || 
	    (text[i]==opentext[8] && strcmp("~/",GetPath())==0))
	  XSetForeground(GetDisplay(),GetGC(),darkgrey);
	else
	  XSetForeground(GetDisplay(),GetGC(),black);
	XDrawImageString(GetDisplay(),buttons[i],GetGC(),20,13,
			 text[i]+1,strlen(text[i]+1));
      }
      yw=yw+18;
    } else {
      dummy=XCreateSimpleWindow(GetDisplay(),selectwin,
			  3,yw+1,xsize-3-3,1,0,
			  darkgrey,
			  darkgrey
				);
      yw=yw+3;
      XMapRaised(GetDisplay(),dummy);
    }
    i++;
  }      
  done =0;
  while (done==0) {
    XNextEvent(GetDisplay(),&event);
    switch(event.type) {
    case ButtonRelease:
      if (event.xbutton.time-time<500)
	break;
    case ButtonPress:
      do {
	done=0;
	XQueryPointer(GetDisplay(),selectwin,&dummy,&presswin,
		    &xr,&yr,&xw,&yw,&keys);
	selectedmenu=NULL;
	i=0;
	while(strcmp("-end",text[i])!=0) {
	  if (strcmp("-",text[i])!=0) {
	    if (buttons[i]==presswin)
	      selectedmenu=text[i];
	  } 
	  i++;
	}
	done=1;
	if (selectedmenu!=NULL) {
	  if (selectedmenu==maintext[0]) {
	    XUngrabPointer(GetDisplay(),event.xbutton.time);
	    XTranslateCoordinates(GetDisplay(),presswin,XDefaultRootWindow(GetDisplay()),
			xsize-2,2,&xnew,&ynew,&dummy);
	    if (GetIsDesktop()==True)
	      selectedmenu=Menu(event.xbutton.time,helptextmain,xnew,ynew,xsize,shnr);
	    else 
	      selectedmenu=Menu(event.xbutton.time,helptextnone,xnew,ynew,xsize,shnr);
	  } else if (selectedmenu==maintext[2] && xw>xsize-20) {
	    XUngrabPointer(GetDisplay(),event.xbutton.time);
	    XTranslateCoordinates(GetDisplay(),presswin,XDefaultRootWindow(GetDisplay()),
			xsize-2,2,&xnew,&ynew,&dummy);
	    selectedmenu=Menu(event.xbutton.time,opentext,xnew,ynew,xsize,shnr);
	  } else if (selectedmenu==opentext[0] && xw>xsize-20 && inwindow==False) {
	    XUngrabPointer(GetDisplay(),event.xbutton.time);
	    XTranslateCoordinates(GetDisplay(),presswin,XDefaultRootWindow(GetDisplay()),
			xsize-2,2,&xnew,&ynew,&dummy);
	    CreateText();
	    shlret=Menu(event.xbutton.time,shellcommands,xnew,ynew,xsize,shnr);
	    if (shlret==NULL)
	      selectedmenu=NULL;
	    else
	      *shnr=GetshnrAndDestroyText(shlret);
	  } else if (selectedmenu==maintext[12] && xw>xsize-20) {
	    XUngrabPointer(GetDisplay(),event.xbutton.time);
	    XTranslateCoordinates(GetDisplay(),presswin,XDefaultRootWindow(GetDisplay()),
			xsize-2,2,&xnew,&ynew,&dummy);
	    selectedmenu=Menu(event.xbutton.time,sorttext,xnew,ynew,xsize,shnr);
	  } else if (selectedmenu==maintext[5] && xw>xsize-20) {
	    XUngrabPointer(GetDisplay(),event.xbutton.time);
	    XTranslateCoordinates(GetDisplay(),presswin,XDefaultRootWindow(GetDisplay()),
			xsize-2,2,&xnew,&ynew,&dummy);
	    selectedmenu=Menu(event.xbutton.time,ersttext,xnew,ynew,xsize,shnr);
	  }
	  if (selectedmenu==NULL) {
	    done=2;
	    XGrabPointer(GetDisplay(),selectwin,False,
			 ButtonPressMask|ButtonReleaseMask,
			 GrabModeAsync,GrabModeSync,None,selectcur,time);
	    i=0;
	    while(strcmp("-end",text[i])!=0) {
	      if (strcmp("-",text[i])!=0) {
		if (inwindow==True && (text[i][0]=='$' || text[i][0]=='!'))
		  XSetForeground(GetDisplay(),GetGC(),darkgrey);
		else
		  XSetForeground(GetDisplay(),GetGC(),black);
		XDrawImageString(GetDisplay(),buttons[i],GetGC(),20,13,
				 text[i]+1,strlen(text[i]+1));
	      }
	      i++;
	    }
	  }
	}
      } while(done==2);
     break;
    default:
      break;
    }
  }
  XFlush(GetDisplay());
  if (event.type==ButtonPress) {
    do {
      XNextEvent(GetDisplay(),&event);  
    } while (event.type!=ButtonRelease);
  }
  XFreeCursor(GetDisplay(),selectcur);
  XDestroyWindow(GetDisplay(),selectwin);
  XFlush(GetDisplay());
  return(selectedmenu);
}

void MenuLaunch(Time time,char *fil,int inw)
{
  char* p;
  char *text;
  char *textf;
  char *selectedmenu;
  char *pos,*icon,*start;
  int i,j;
  int shnr;
  
  iconname=GenerateIconName(GetPath());
  filenames=fil;
  inwindow=inw;
  selectedmenu=Menu(time,maintext,-1,-1,-1,&shnr);
  if (selectedmenu!=NULL) {
    if (selectedmenu==sorttext[0] || selectedmenu==maintext[12]) {
      SortIcons(TYPE);
      PlaceIcons();
    } else if (selectedmenu==sorttext[1]) {
      SortIcons(NAME);
      PlaceIcons();
    } else if (selectedmenu==sorttext[2]) {
      SortIcons(EXTENSION);
      PlaceIcons();
    } else if (selectedmenu==sorttext[4]) {
      SortIcons(DATEASC);
      PlaceIcons();
    } else if (selectedmenu==sorttext[5]) {
      SortIcons(DATEDESC);
      PlaceIcons();
    } else if (selectedmenu==sorttext[7]) {
      SortIcons(SIZEASC);
      PlaceIcons();
    } else if (selectedmenu==sorttext[8]) {
      SortIcons(SIZEDESC);
      PlaceIcons();
    } else if (selectedmenu==maintext[13]) {
      SortIcons(ARRANGE);
      PlaceIcons();
    } else if (selectedmenu==helptextmain[0] || selectedmenu==helptextnone[0]) {
      Info();
    } else if (selectedmenu==helptextmain[1] || selectedmenu==helptextnone[1]) {
      system("xterm -e man dfm &");
    } else if (selectedmenu==helptextmain[2] || selectedmenu==helptextnone[2]) {
      p=strdup(GetPath());
      if (p[0]=='~' && p[1]=='/')
	p[0]='.';
      chdir(p);
      system("xterm -sb -sl 500 &");
      chdir((char*)getenv("HOME"));
      free(p);
    } else if (selectedmenu==helptextnone[4]) {
      XCloseDisplay(GetDisplay());
      exit(0);
    } else if (selectedmenu==helptextmain[4]) {
      if (GetMainBackground()==True) {
	SetMainBackground(False);
	XUnmapWindow(GetDisplay(),GetWindow());
      } else {
	SetMainBackground(True);
	XMapWindow(GetDisplay(),GetWindow());
      }
    } else if (selectedmenu==helptextmain[6]) {
      NewProgram();
    } else if (selectedmenu==helptextmain[7]) {
      SaveFileInfoFromDesktop();
    } else if (selectedmenu==helptextmain[8]) {
      makehomelinks();      
      UpdateIcons(False);
    } else if (selectedmenu==helptextmain[9]) {
      ExecuteShellcommand("./.dfmdesk/","dfm -setdir !0!",NULL,0);
    } else if (selectedmenu==helptextmain[11]) {
      if (Question("Really want to leave DFM ?")==2) {
	XClearWindow(GetDisplay(),XDefaultRootWindow(GetDisplay()));
	XFlush(GetDisplay());
	system("killall dfm");
	XCloseDisplay(GetDisplay());
	exit(0);
      }
    } else if (selectedmenu==helptextmain[12]) {
      if (Question("Really want to do a \"killall X\" ?")==2) 
	system("killall X");
    } else if (selectedmenu==maintext[3]) {
      text=malloc(strlen("dfm -setfile  &")+strlen(filenames)+1);
      if (inwindow==True)
	strcpy(text,"dfm -setdir ");
      else
	strcpy(text,"dfm -setfile ");
      strcat(text,filenames);
      strcat(text," &");
      for(i=0;i<strlen(text)-2;i++) 
	if (text[i]==' ' && text[i+1]=='~' && text[i+2]=='/')
	  text[i+1]='.';
      usershellsystem(text);
      free(text);
    } else if (selectedmenu==ersttext[0] || selectedmenu==maintext[5]) {
      NewFolder();
    } else if (selectedmenu==ersttext[1]) {
      NewFile();
    } else if (selectedmenu==maintext[6]) {
      UpdateIcons(False);
    } else if (selectedmenu==maintext[7] && inwindow==False) {
      if (filenames[0]==' ') {
	EraseMultipleInfo(filenames);
      } else {
	text=malloc(strlen(filenames)+2);
	strcpy(text," ");
	strcat(text,filenames);
	EraseMultipleInfo(text);
	free(text);
      }     
      UpdateIcons(False);
    } else if (selectedmenu==maintext[8]) {
      SetAllMarks();
    } else if (selectedmenu==maintext[10]) {
      if (inwindow==False) {
	if (filenames[0]==' ') {
	  EraseCall(filenames,True,False);
	} else {
	  text=malloc(strlen(filenames)+2);
	  strcpy(text," ");
	  strcat(text,filenames);
	  EraseCall(text,True,False);
	  free(text);
	}
      } else {
	if (strcmp("Trashcan",iconname)==0 ||
	    strcmp("trashcan",iconname)==0 ||
	    strcmp("Papierkorb",iconname)==0) {
	  if (Question("Really delete the files in this Trashcan ?")==2) {
	    text=malloc(strlen("\\rm -rf * .*")+2*strlen(GetPath())+1);
	    strcpy(text,"\\rm -rf ");
	    strcat(text,GetPath());
	    strcat(text,"* ");
	    strcat(text,GetPath());
	    strcat(text,".*");
	    usershellsystem(text);
	    free(text);
	    UpdateIcons(True);
	  }
	}
      }
    } else if ((selectedmenu==maintext[2] || selectedmenu==opentext[0]) && inwindow==False) {
      textf=DeSlashName(filenames);
      GetFileInfoFromDesktop(textf,&pos,&icon,&start);
      if (start==NULL || icon==NULL) {
	free(start);
	free(icon);
      } else{
	ExecuteShellWithFollowLink(textf,start,NULL,shnr);
	free(pos);
	free(icon);
	free(start);
	UnsetMarks();
      }
      free(textf);
    } else if (selectedmenu==opentext[1] && inwindow==False) {
      textf=DeSlashName(filenames);
      GetFileInfoFromDesktop(textf,&pos,&icon,&start);
      if (start==NULL || icon==NULL) {
	free(start);
	free(icon);
      } else{
	text=malloc(strlen(start)+strlen("xterm -e ")+1);
	strcpy(text,"xterm -e ");
	strcat(text,start);
	free(start);
	start=text;
	ExecuteShellWithFollowLink(textf,start,NULL,0);
	free(pos);
	free(icon);
	free(start);
	UnsetMarks();
      }
      free(textf);
    } else if (selectedmenu==opentext[2] && inwindow==False) {
      textf=DeSlashName(filenames);
      GetFileInfoFromDesktop(textf,&pos,&icon,&start);
      if (start==NULL || icon==NULL) {
	free(start);
	free(icon);
      } else{
	ExecuteShellWithFollowLink(textf,"!0!",NULL,0);
	free(pos);
	free(icon);
	free(start);
	UnsetMarks();
      }
      free(textf);
    } else if (selectedmenu==opentext[4]) {
      if (inwindow==False) {
	textf=DeSlashName(filenames);
	GetFileInfoFromDesktop(textf,&pos,&icon,&start);
	if (start==NULL || icon==NULL) {
	  free(start);
	  free(icon);
	} else{
	  ExecuteShellWithFollowLink(textf,"dfm !0!",NULL,0);
	  free(pos);
	  free(icon);
	  free(start);
	  UnsetMarks();
	}
	free(textf);
      } else {
	ExecuteShellcommand(GetPath(),"dfm !0!",NULL,0);
      }
    } else if (selectedmenu==opentext[6]) {
      if (inwindow==False) {
	textf=DeSlashName(filenames);
	GetFileInfoFromDesktop(textf,&pos,&icon,&start);
	if (start==NULL || icon==NULL) {
	  free(start);
	  free(icon);
	} else{
	  ExecuteShellWithFollowLink(textf,"dfm !0! -structure",NULL,0);
	  free(pos);
	  free(icon);
	  free(start);
	  UnsetMarks();
	}
	free(textf);
      } else {
	ExecuteShellcommand(GetPath(),"dfm !0! -structure",NULL,0);
      }
    } else if (selectedmenu==opentext[5]) {
      if (inwindow==False) {
	textf=DeSlashName(filenames);
	GetFileInfoFromDesktop(textf,&pos,&icon,&start);
	if (start==NULL || icon==NULL) {
	  free(start);
	  free(icon);
	} else{
	  ExecuteShellWithFollowLink(textf,"dfm !0! -detail",NULL,0);
	  free(pos);
	  free(icon);
	  free(start);
	  UnsetMarks();
	}
	free(textf);
      } else {
	ExecuteShellcommand(GetPath(),"dfm !0! -detail",NULL,0);
      }
    } else if (selectedmenu==opentext[8]) {
      if (strcmp("~/",GetPath())!=0) {
	text=strdup(GetPath());
	j=0;
	for (i=0;i<(strlen(text)-1);i++)
	  if (text[i]=='/') 
	    j=i;
	text[j+1]=0;
	ExecuteShellcommand(text,"dfm !0!",NULL,0);
	free(text);
      }
    }
  }
  free(iconname);
}
