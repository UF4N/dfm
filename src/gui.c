/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "gui.h"
#include "iwc.h"
#include "MwmUtil.h"
#include "forms.h"
#include "version.h"
#include "iconmanager.h"
#include "filemanager.h"
#include <Xatom.h>
#include <stdio.h>
#include <malloc.h>
#include "global.h"

int exitvar=0;

void initgui(int argc,char *argv[])
{
  fl_initialize(&argc,argv,"DFM",0,0);
  SetDisplay(fl_get_display());
}

int destroying(FL_FORM *form,void *a)
{
  exitvar=CANCEL;
  return FL_IGNORE;
}

void ExitCallback(FL_OBJECT *obj, long arg)
{
  exitvar=CANCEL;
}

void YesCallback(FL_OBJECT *obj, long arg)
{
  exitvar=YES;
}

void AllCallback(FL_OBJECT *obj, long arg)
{
  exitvar=ALL;
}

void NoCallback(FL_OBJECT *obj, long arg)
{
  exitvar=NO;
}

void RenameCallback(FL_OBJECT *obj, long arg)
{
  exitvar=RENAME;
}

void DFMguiMainLoop(FL_FORM *formobj)
{
  XEvent event;
  static Atom motif_wm_hints= None;
  FL_OBJECT *obj;
  MotifWmHints hints;
  
  exitvar=0;
  fl_check_forms(); 
  XFlush(fl_get_display());
  XSync(fl_get_display(),False);
  fl_check_forms(); 
  XFlush(fl_get_display());

  hints.flags=1;
  hints.functions=0x24;
  if ( motif_wm_hints== None)
     motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
  XChangeProperty(GetDisplay(), formobj->window, motif_wm_hints, motif_wm_hints, 32,
		      PropModePrepend, (unsigned char *) (&hints), sizeof(MotifWmHints)/sizeof(CARD32));

  XSetTransientForHint(fl_get_display(),formobj->window,GetWindow());
  while(exitvar==0) {
    obj=fl_check_forms();
    if (obj==FL_EVENT) {
      fl_XNextEvent(&event);
      if (event.type==EnterNotify && event.xcrossing.window==GetWindow() && 
	  GetIsDesktop()==True) {
	XLowerWindow(GetDisplay(),GetWindow());
	LowerIcons();
	ManageAutofocus(&event);
      }
    }  
  }
  while(XEventsQueued(GetDisplay(),QueuedAfterReading)!=0)
    XNextEvent(GetDisplay(),&event);
}

void Info()
{
  FL_OBJECT *obj;
  FL_FORM *infoform;
  infoform = fl_bgn_form(FL_NO_BOX, 240, 180);
  obj = fl_add_box(FL_FLAT_BOX,0,0,240,180,"");
  obj = fl_add_text(FL_NORMAL_TEXT,10,10,220,20,"DFM for X11");
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_SHADOW_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,30,220,20,VERSION);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,50,220,20,"Desktop-File-Manager");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,70,220,20,"(c) 1997 by Achim Kaiser");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,10,140,220,30,"Ok");
    fl_set_object_callback(obj,ExitCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,10,90,220,20,"Distribute only under GNU GPL");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,110,220,20,"Man Page by Hans Paijmans");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(infoform,FL_PLACE_CENTER,FL_TRANSIENT,"Info");
  DFMguiMainLoop(infoform);
  fl_hide_form(infoform);
  fl_free_form(infoform);
}

int Question(char *text)
{
  FL_OBJECT *obj;
  FL_FORM *eraseform;
  eraseform= fl_bgn_form(FL_NO_BOX, 310, 70);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,70,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,30,140,30,"Yes");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,30,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,10,0,290,30,text);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(eraseform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(eraseform);
  fl_hide_form(eraseform);
  fl_free_form(eraseform);
  return exitvar;
}

int OverwriteQuestion(char *filename,char **rename)
{
  char *text;
  char *text2;
  FL_OBJECT *obj;
  FL_FORM *overwriteform;
  FL_OBJECT *inp_obj;
  overwriteform=fl_bgn_form(FL_NO_BOX, 310, 230);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,230,""); 
  obj = fl_add_button(FL_NORMAL_BUTTON,10,60,65,30,"Yes");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,85,60,65,30,"All");
    fl_set_object_callback(obj,AllCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,60,140,30,"No, Cancel To Next");
    fl_set_object_callback(obj,NoCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,10,150,290,30,"Rename");
    fl_set_object_callback(obj,RenameCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,10,190,290,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  inp_obj = fl_add_input(FL_NORMAL_INPUT,10,110,290,30,"");
    text=GenerateLastName(filename);
    text2=malloc(strlen(text)+2);
    strcpy(text2,text);
    strcat(text2,"z");
    free(text);
    fl_set_input(inp_obj,text2);
    free(text2);
  obj = fl_add_text(FL_NORMAL_TEXT,10,40,300,20,"Allready exist! Delete this file before copying ... ?");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,10,290,30,filename);
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,90,180,20,"... or rename source to");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(overwriteform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(overwriteform);
  if (exitvar==5) {
    *rename=malloc(strlen((char*)fl_get_input(inp_obj))+1);
    strcpy(*rename,(char*)fl_get_input(inp_obj));
  } else
    *rename=NULL;
  fl_hide_form(overwriteform);
  fl_free_form(overwriteform);
  return exitvar;
}

void NewProgram()
{
  FL_OBJECT *obj;
  FL_OBJECT *inp_obj;
  FL_FORM *nfileform;
  const char *text;
  nfileform= fl_bgn_form(FL_NO_BOX, 310, 90);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,90,"");
  inp_obj = fl_add_input(FL_NORMAL_INPUT,50,10,250,30,"Program");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,50,140,30,"Create");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,50,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(nfileform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(nfileform);
  fl_hide_form(nfileform);
  if (exitvar!=1) {
    text=fl_get_input(inp_obj);
    CreateProgramLink((char*)text);
    UpdateIcons(True);
  }
  fl_free_form(nfileform);
}

void NewFile()
{
  FL_OBJECT *obj;
  FL_OBJECT *inp_obj;
  FL_FORM *nfileform;
  const char *text;
  nfileform= fl_bgn_form(FL_NO_BOX, 310, 90);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,90,"");
  inp_obj = fl_add_input(FL_NORMAL_INPUT,50,10,250,30,"File");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,50,140,30,"Create");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,50,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(nfileform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(nfileform);
  fl_hide_form(nfileform);
  if (exitvar!=1) {
    text=fl_get_input(inp_obj);
    CreateFile((char*)text);
    UpdateIcons(True);
  }
  fl_free_form(nfileform);
}

void NewFolder()
{
  FL_OBJECT *obj;
  FL_OBJECT *inp_obj;
  FL_FORM *nfolderform;
  const char *text;
  nfolderform= fl_bgn_form(FL_NO_BOX, 310, 90);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,90,"");
  inp_obj = fl_add_input(FL_NORMAL_INPUT,50,10,250,30,"Folder");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,50,140,30,"Create");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,50,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(nfolderform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(nfolderform);
  fl_hide_form(nfolderform);
  if (exitvar!=1) {
    text=fl_get_input(inp_obj);
    CreateFolder((char*)text);
    UpdateIcons(True);
  }
  fl_free_form(nfolderform);
}

int EraseQuestion(char *filename)
{
  FL_OBJECT *obj;
  FL_FORM *eraseform;
  eraseform= fl_bgn_form(FL_NO_BOX, 310, 100);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,100,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,60,40,30,"Yes");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,60,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,60,60,40,30,"All");
    fl_set_object_callback(obj,AllCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,110,60,40,30,"No");
    fl_set_object_callback(obj,NoCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,10,40,160,20,"Really delete this file?");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,10,290,30,filename);
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(eraseform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(eraseform);
  fl_hide_form(eraseform);
  fl_free_form(eraseform);
  return exitvar;
}

int EraseQuestionOnDifferent(char *filename)
{
  FL_OBJECT *obj;
  FL_FORM *eraseform;
  eraseform=fl_bgn_form(FL_NO_BOX, 310, 120);
  obj = fl_add_box(FL_FLAT_BOX,0,0,310,120,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,80,40,30,"Yes");
    fl_set_object_callback(obj,YesCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,160,80,140,30,"Cancel");
    fl_set_object_callback(obj,ExitCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,60,80,40,30,"All");
    fl_set_object_callback(obj,AllCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,110,80,40,30,"No");
    fl_set_object_callback(obj,NoCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,10,40,300,20,"Some files aren't copied correctly! There are differences!");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,10,290,30,filename);
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,60,290,20,"Really delete this orginal?");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();
  fl_set_atclose(destroying,"");
  fl_show_form(eraseform,FL_PLACE_CENTER,FL_TRANSIENT,"Control");
  DFMguiMainLoop(eraseform);
  fl_hide_form(eraseform);
  fl_free_form(eraseform);
  return exitvar;
}

