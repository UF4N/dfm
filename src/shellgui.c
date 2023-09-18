/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "forms.h"
#include "MwmUtil.h"
#ifdef SUNOS
#include <signal.h>
#include <types.h>
#include <wait.h>
#else
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/types.h>
#endif
#include <unistd.h>
#include "global.h"
#include "iconmanager.h"
#include "filemanager.h"
#include "iwc.h"

pid_t process;

int shellexitvar;
int killed;

FL_FORM *shformobj;
FL_OBJECT *shinfoobj;

int KillCallback(FL_FORM *form,void *a)
{
  killed=True;
  shellexitvar=1;
  if (process!=0)
    kill(process,SIGTERM);
  return FL_IGNORE;
}

void KillCallback2(FL_OBJECT *obj, long arg)
{
  killed=True;
  shellexitvar=1;
  if (process!=0)
    kill(process,SIGTERM);
}

void sigchildren()
{
  shellexitvar=1;
}

void ShellWindowInit()
{
  FL_OBJECT *obj;
  process=0;
  killed=False;
  shformobj = fl_bgn_form(FL_NO_BOX,200, 60);
  shinfoobj = fl_add_text(FL_NORMAL_TEXT,0,0,200,30,"---");
    fl_set_object_lalign(shinfoobj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_boxtype(shinfoobj,FL_DOWN_BOX);
  obj = fl_add_button(FL_NORMAL_BUTTON,0,30,200,30,"Stop...");
    fl_set_object_callback(obj,KillCallback2,0);
  fl_end_form();
  fl_set_atclose(KillCallback,"");
}

void ShellWindowInfo(char *name)
{
  fl_set_object_label(shinfoobj,name);
}

int ShellWindowUpdate()
{
  XEvent event;
  FL_OBJECT *obj;

  obj=fl_check_forms();
  if (obj==FL_EVENT) {
    fl_XNextEvent(&event);
    if (event.type==EnterNotify && GetIsDesktop()==True) {
      XLowerWindow(GetDisplay(),GetWindow());
      LowerIcons();
      ManageAutofocus(&event);
    }
  }  
  obj=fl_check_forms();
  if (obj==FL_EVENT) {
    fl_XNextEvent(&event);
    if (event.type==EnterNotify && GetIsDesktop()==True) {
      XLowerWindow(GetDisplay(),GetWindow());
      LowerIcons();
      ManageAutofocus(&event);
    }
  } 
  return killed;
}

int ShellWindowDoIt(char** argv,int cont)
{
  int ret;
  XEvent event;
  static Atom motif_wm_hints= None;
  MotifWmHints hints;
  FL_OBJECT *obj;

  shellexitvar=0;
  signal(SIGCHLD,sigchildren);
  if ((process=fork())==0) {
    if (cont==True)
      if (fork()!=0)
	exit(0);
    execvp(argv[0],argv);
    exit(-1);
  } else {
    if (cont==False) {
      if (shellexitvar==0 && strcmp(argv[0],"cp")==0) {
	if (fl_form_is_visible(shformobj)==False) {
	  fl_show_form(shformobj,FL_PLACE_CENTER,FL_TRANSIENT,"DFM control");
	  fl_check_forms(); 
	  XFlush(fl_get_display());
	  XSync(fl_get_display(),False);
	  fl_check_forms(); 
	  XFlush(fl_get_display());
	  hints.flags=1;
	  hints.functions=36;
	  if ( motif_wm_hints== None)
	    motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
	  XChangeProperty(GetDisplay(), shformobj->window, motif_wm_hints, motif_wm_hints, 32,
			  PropModePrepend, (unsigned char *) &hints, sizeof(MotifWmHints)/sizeof(CARD32));
	  XSetTransientForHint(fl_get_display(),shformobj->window,GetWindow());
	}
	while(shellexitvar==0) {
	  obj=fl_check_forms();
	  if (obj==FL_EVENT) {
	    fl_XNextEvent(&event);
	    if (event.type==EnterNotify && GetIsDesktop()==True) {
	      XLowerWindow(GetDisplay(),GetWindow());
	      LowerIcons();
	      ManageAutofocus(&event);
	    }
	  }  
	}
	while(XEventsQueued(GetDisplay(),QueuedAfterReading)!=0)
	  XNextEvent(GetDisplay(),&event);
      }
    }
    wait(&ret);
  }
  process=0;
  if (killed==True)
    return BREAK_ERROR;
  else {
    if (ret==0)
      return NO_ERROR;
    else
      return REPORT_ERROR;   
  }
}

void ShellWindowDestroy()
{
  XEvent event;

  while(XEventsQueued(GetDisplay(),QueuedAfterReading)!=0)
    XNextEvent(GetDisplay(),&event);
  if (fl_form_is_visible(shformobj)==True)
    fl_hide_form(shformobj);
  fl_free_form(shformobj);
  if (GetIsDesktop()==True) {
    XLowerWindow(GetDisplay(),GetWindow());
    LowerIcons();
  }
}
