/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <Xlib.h>
#include <Xutil.h>
#include <Xatom.h>
#include <cursorfont.h>
#ifdef SUNOS
#include <signal.h>
#else
#include <sys/signal.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "detailtitle.h"
#include "positiontext.h"
#include "windowposition.h"
#include "pixmap.h"
#include "iwc.h"
#include "gui.h"
#include "timer.h"
#include "iconmanager.h"
#include "global.h"
#include "testinstall.h"
#include "windowname.h"
#include "options.h"
#include "color.h"
#include "includepixmaps/icon.xpm"

int main(int argc,char *argv[])  
{
  Display *dis;
  Window win,movewin;
  Window *oldwin;
  XEvent event;
  XSizeHints hint;
  int scr;
  int done;
  int mapagain;
  int maindesktop;
  XWindowAttributes rootatr;
  XSetWindowAttributes xswa;
  unsigned long valuemask;
  char *windowname=NULL;
  char *iconname=NULL;
  char *pathname=NULL;
  char *backgroundname=NULL;
  char *positiona=NULL;
  char *positions=NULL;
  Pixmap wm_iconpix;
  static Atom           proto_atom= None, delete_atom= None;
  XClassHint *classhint;

  /*debugging*/
  if (fork()!=0)
    exit(0);
  /***********/

  chdir((char*)getenv("HOME"));
  initgui(argc,argv);
  dis=GetDisplay();
  if (dis==NULL) {
     printf("DFM: Can't open display! Check DISPLAY variable!\n");
     exit(-1);
  }
  /*XSynchronize(GetDisplay(),1);*/
  SetTimer(0);
  if (IsDesktopActive()==True) {
    if (argc==3 && strcmp(argv[1],"-setdir")==0) {
      if (strcmp("./.dfmdesk/",argv[2])==0 || strcmp("./.dfmdesk",argv[2])==0)
	MainWindowOptions(argc,argv);
      else
	WindowOptions(argc,argv);      
    }
    if (argc==3 && strcmp(argv[1],"-setfile")==0)
      FileOptions(argc,argv);
    if (argc>3 && strcmp(argv[1],"-setfile")==0)
      MultipleFileOptions(argc,argv);
  }
  if (argc==3 && strcmp(argv[2],"-detail")==0)
    SetKindOfWindow(DETAIL);
  else if (argc==3 && strcmp(argv[2],"-structure")==0)
    SetKindOfWindow(STRUCTURE);
  else  
    SetKindOfWindow(NORMAL);
  SetCursors(XCreateFontCursor(dis,XC_middlebutton),XCreateFontCursor(dis,XC_watch));
  scr=DefaultScreen(dis);
  if (argc==1) {
    hint.x=0; hint.y=0;
    hint.width=2;hint.height=2;
    hint.flags=PPosition|PSize;
    if (IsDesktopActive()==False) {
      maindesktop=True;
      
      windowname=".dfmdesk --- dfm:~/.dfmdesk/";
      iconname=".dfmdesk";
      pathname="~/.dfmdesk/";
      InitColors();
    } else { 
      printf("DFM is already running.\n");
      XCloseDisplay(dis);
      exit(1);
    }
  } else {
    if (IsDesktopActive()==False) {
      printf("wait for mainprogram\n");
      SetDestroy(30);
      do {} while(IsDesktopActive()==False);
      StopDestroy();
    }
    pathname=malloc(3+strlen(argv[1]));
    pathname[0]=0;
    if (strncmp(argv[1],(char*)getenv("HOME"),strlen((char*)getenv("HOME")))==0 &&
        (argv[1][strlen((char*)getenv("HOME"))]=='/' ||
        argv[1][strlen((char*)getenv("HOME"))]==0)) {
      strcat(pathname,"~");
      strcat(pathname,argv[1]+strlen((char*)getenv("HOME")));
    } else {
      if (argv[1][0]!='.' && argv[1][1]!='/' && argv[1][0]!='/')
	strcat(pathname,"~/");
      strcat(pathname,argv[1]);
    }
    if (pathname[0]=='.')
      pathname[0]='~';
    if (pathname[strlen(pathname)-1]!='/')
      strcat(pathname,"/");
    if (strcmp(pathname,"~/.dfmdesk/")==0 && GetKindOfWindow()==NORMAL) {
      XCloseDisplay(dis);
      exit(0);
    }

    windowname=GenerateWindowName(pathname,GetKindOfWindow());

    iconname=GenerateIconName(pathname);

    oldwin=GetWindowNamed(windowname);

    if (oldwin!=NULL) {
      SendWindow(*oldwin,RAISE);
      XFlush(dis);
      XCloseDisplay(dis);
      exit(0);
    }
    maindesktop=False;
    GetFileInfoFromDesktop(windowname,&positiona,&backgroundname,
			   &positions);
    GetPositionFromText(positiona,&hint.x,&hint.y,NULL);
    GetPositionFromText(positions,&hint.width,&hint.height,NULL);
    hint.flags=PPosition|PSize;
    if (positiona==NULL) {
      hint.x=80; hint.y=80;
      hint.width=500;hint.height=300;
      hint.flags=/*PPosition|*/PSize;
    }
  }
  if (testandinstall(pathname)==False) {
    printf("DFM: Can`t open \"%s\" ?!\n",pathname);
    XCloseDisplay(dis);
    exit(0);
  }
  SetIsDesktop(maindesktop);
  SetPath(pathname);
  SetWindowName(windowname);

  if (maindesktop==True) {
    xswa.override_redirect=True;
    valuemask=CWOverrideRedirect;  
    win=XCreateWindow(dis,DefaultRootWindow(dis),
		      hint.x,hint.y,hint.width,hint.height,
		      0,0,InputOnly,CopyFromParent,valuemask,&xswa);
    movewin=XCreateWindow(dis,win,
		      0,0,hint.width,hint.height,
		      0,0,InputOnly,CopyFromParent,valuemask,&xswa);
  } else {
    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),
			    hint.x,hint.y,hint.width,hint.height,
			    0,0,0);
    movewin=XCreateSimpleWindow(dis,win,
				0,0,hint.width,hint.height,
				0,GetBestColor(0xffff,0xffff,0xffff),
                                  GetBestColor(0xffff,0xffff,0xffff));
  }
  XDefineCursor(dis,win,GetWaitCursor());
  SetGC(XCreateGC(GetDisplay(),XDefaultRootWindow(GetDisplay()),0,0));
  SetWindow(win);
  SetMoveWindow(movewin);
  if (maindesktop==True)
    LoadFileInfoToDesktop();
  DataPixmap(icon_xpm,&wm_iconpix,NULL);
  XSetStandardProperties(dis,win,windowname,iconname,wm_iconpix,argv,argc,&hint);
  classhint=XAllocClassHint();
  classhint->res_name="dfm";
  classhint->res_class="dfm";
  XSetClassHint(dis,win,classhint);
  XLowerWindow(dis,win);
  XMapWindow(dis,movewin);
  UpdateWindow();
  UpdateFontName();
  UpdateTimer();
  UpdateColors();
  if (maindesktop==False) {
    XSelectInput(dis,movewin,VisibilityChangeMask|
		             ButtonReleaseMask|ButtonPressMask|PointerMotionMask);
    XSelectInput(dis,win,KeyPressMask|StructureNotifyMask);
  } else {
    XSelectInput(dis,movewin,EnterWindowMask|ButtonReleaseMask|ButtonPressMask|
		 PointerMotionMask);
    XSelectInput(dis,win,KeyPressMask);
    XSelectInput(dis,XDefaultRootWindow(GetDisplay()),KeyPressMask);
    if (GetMainBackground()==False)
      XUnmapWindow(dis,win);
    else
      XMapWindow(dis,win);
  }
  if (GetKindOfWindow()==DETAIL)
    CreateDetailTitle();
  if (maindesktop==False) {
    XMapRaised(dis,win);
    if (positiona!=NULL)
      SetPosition(win,movewin,hint.x,hint.y,hint.width,hint.height);
    proto_atom = XInternAtom(dis, "WM_PROTOCOLS", False);
    delete_atom = XInternAtom(dis, "WM_DELETE_WINDOW", False);
    if ((proto_atom != None) && (delete_atom != None))
      XChangeProperty(dis, win, proto_atom, XA_ATOM, 32,
		      PropModePrepend, (unsigned char *) &delete_atom, 1);
  }
  if (GetKindOfWindow()==STRUCTURE)
    MakeWindowTree();
  else
    MakeWindowIcons();
  if (IsSubDesktop()==True) {
    SetWindowSaveStatus(GetPath(),True);
    SetIconSaveStatus(GetPath(),True);
  }
  done=0;
  mapagain=0;
  XUndefineCursor(dis,win);
  signal(SIGALRM,updatetimer);
  alarm(GetTimer());
  while (True||done==0) {
    if (mapagain==0) {
      signal(SIGALRM,updatetimer);
      XNextEvent(dis,&event);
      signal(SIGALRM,repeat);
    } else
      XNextEvent(dis,&event);  
    switch(event.type) {
    case ClientMessage:
      if ((maindesktop==False) && 
	    (event.xclient.message_type == proto_atom) &&
	    (event.xclient.window == win) && 
	    (event.xclient.data.l[0] == delete_atom)) {
        XGetWindowAttributes(dis,win,&rootatr);
        if (rootatr.map_state==IsUnmapped || GetWindowCache()==0) {
            XCloseDisplay(GetDisplay());
            exit(0);
        }
        /*XWithdrawWindow(dis,win,0); * what's better? */
    	XUnmapWindow(dis,win);
	mapagain=1;
        signal(SIGALRM,destroy);
	alarm(GetWindowCache());
      } else {
        if (event.xclient.message_type==XInternAtom(GetDisplay(),
					     "_DFMMESSAGE",False)) {
          if (event.xclient.data.s[0]==RAISE) {
	     GetFileInfoFromDesktop(windowname,&positiona,&backgroundname,
	     		            &positions);
	     GetPositionFromText(positiona,&hint.x,&hint.y,NULL);
	     GetPositionFromText(positions,&hint.width,&hint.height,NULL);

             XMapRaised(dis,win);
 	     SetPosition(win,0,hint.x,hint.y,hint.width,hint.height);
             signal(SIGALRM,updatetimer);
             alarm(GetTimer());
             mapagain=0;
          } else
	    if (mapagain==0) {
	      XDefineCursor(dis,win,GetWaitCursor());
	      WorkIncomingCall(event);
	      XUndefineCursor(dis,win);
	    }
	}
      }
      break;
    case MapNotify:
      break;
    case EnterNotify:
      if (maindesktop==True) {
	XLowerWindow(dis,win);
	LowerIcons();
	ManageAutofocus(&event);
      }
      break;
    case Expose:
      XLowerWindow(dis,win);
      LowerIcons();
      break;
    case LeaveNotify:
      XUndefineCursor(dis,win);
      XSelectInput(dis,XDefaultRootWindow(GetDisplay()),PropertyChangeMask);
      XMapWindow(dis,win);
      XLowerWindow(dis,win);
      break;
    case ButtonPress:
      if (maindesktop==True && event.xbutton.button==2 && GetMainBackground()==True) {	
	XDefineCursor(dis,win,GetMidCursor());
	XSelectInput(dis,XDefaultRootWindow(GetDisplay()),PropertyChangeMask|LeaveWindowMask);
	XUnmapWindow(dis,win);
	/*XLowerWindow(dis,win); */
	break;
      }
    case MotionNotify:
      if (maindesktop==True)
        XLowerWindow(dis,win);
    default:
      if (mapagain==0) {
	done=HandleTheEvent(event);
      }
      break;
    }
  }
  if (maindesktop==True) {
    /*SaveFileInfoFromDesktop();*/
  }
  XCloseDisplay(dis);
  exit(0);
}
