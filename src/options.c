/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "options.h"
#include <malloc.h>
#include <forms.h>
#include "MwmUtil.h"
#include "iconmanager.h"
#include "filemanager.h"
#include <Xatom.h>
#include "shell.h"
#include <stdio.h>
#include "global.h"
#include "windowname.h"
#include "positiontext.h"
#include "iwc.h"
#include "selectcolor.h"
#include <stdlib.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>


char *name;
char **filearray;
int gargc;
char **gargv;
char* winame;
char* defaultback=NULL;
char *position=NULL,*icon=NULL,*start=NULL;
FL_FORM *mainform;
FL_OBJECT *iconobj;
FL_OBJECT *wininfobj;
FL_OBJECT *icoinfobj;
FL_OBJECT *objname;
FL_OBJECT *objtime;
FL_OBJECT *objsize;
FL_OBJECT *objmode;
FL_OBJECT *objtimer;
FL_OBJECT *objcache;
FL_OBJECT *objfont;
FL_OBJECT *objdefback;
FL_OBJECT *winhide;
FL_OBJECT *objow,*objor,*objox;
FL_OBJECT *objgw,*objgr,*objgx;
FL_OBJECT *objww,*objwr,*objwx;
FL_OBJECT *objren;
FL_OBJECT *objax,*objs;
FL_OBJECT *objay,*obja;
FL_OBJECT *objbx,*objb;
FL_OBJECT *objby,*objc;
FL_OBJECT *objgrid;
FL_OBJECT *objbottom;
FL_OBJECT *objright;
FL_OBJECT *objchangei;
FL_OBJECT *objchanges;
FL_OBJECT *objchangeo;
FL_OBJECT *objchangep;
FL_OBJECT *objchangem;
FL_OBJECT *objmforcol;
FL_OBJECT *objmbaccol;
FL_OBJECT *objwforcol;
FL_OBJECT *objwbaccol;
char *mbackcol,*wbackcol;
char *mforecol,*wforecol;


void WindowApplyCallback(FL_OBJECT *obj, long arg)
{
  Window *updwin;
  char *text;
  char *pos,*dummy,*endpos;

  GetFileInfoFromDesktop(winame,&pos,&dummy,&endpos);
  free(dummy);
  if (pos!=NULL) {
  if (fl_get_button(winhide)==1)
    pos[0]=' ';
  else
    pos[0]='a';
  }
  EraseFileInfoInDesktop(winame);  
  SendFileInfoToDesktop(winame,pos,icon,endpos);
  free(pos);
  free(endpos);
  if (fl_get_button(wininfobj)==1)
    SetWindowSaveStatus(name,True);
  else
    SetWindowSaveStatus(name,False);
  if (fl_get_button(icoinfobj)==1)
    SetIconSaveStatus(name,True);
  else
    SetIconSaveStatus(name,False);
  updwin=GetWindowNamed(winame);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  text=GenerateWindowName(name,DETAIL);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
  text=GenerateWindowName(name,STRUCTURE);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
}

void FileselCallback(FL_OBJECT *obj, long arg)
{
  const char* fselret;
  char *text;
  int i,j;
  fl_redraw_form(mainform);
  fl_deactivate_all_forms();
  if (icon==NULL) {
    text=malloc(1);
    text[0]=0;
  } else {
    if (icon[0]=='#') {
      text=malloc(strlen("/usr/include/X11/pixmaps")+1);
      strcpy(text,"/usr/include/X11/pixmaps");
    } else {
      text=malloc(strlen(icon)+1);
      strcpy(text,icon);
      j=0;
      for (i=0;i<strlen(text);i++)   
	if (text[i]=='/')
	  j=i;
      text[j]=0;
    }
  }
  fselret=fl_show_fselector("Please choose a pixmap",text,"*",icon);
  free(text);
  if (fselret!=NULL) {
    free(icon);
    icon=malloc(strlen(fselret)+1);
    strcpy(icon,fselret);
    fl_free_pixmapbutton_pixmap(obj);
    fl_set_pixmapbutton_file(obj,icon);
    fl_set_object_label(iconobj,icon);
  }
  fl_activate_all_forms();
  fl_redraw_form(mainform);
}

void DefaultFileselCallback(FL_OBJECT *obj, long arg)
{
  const char* fselret;
  char *text;
  int i,j;
  fl_redraw_form(mainform);
  fl_deactivate_all_forms();
  if (defaultback==NULL) {
    text=malloc(1);
    text[0]=0;
  } else {
    if (defaultback[0]=='#') {
      text=malloc(strlen("/usr/include/X11/pixmaps")+1);
      strcpy(text,"/usr/include/X11/pixmaps");
    } else {
      text=malloc(strlen(defaultback)+1);
      strcpy(text,defaultback);
      j=0;
      for (i=0;i<strlen(text);i++)   
	if (text[i]=='/')
	  j=i;
      text[j]=0;
    }
  }
  fselret=fl_show_fselector("Please choose a pixmap",text,"*",defaultback);
  free(text);
  if (fselret!=NULL) {
    free(defaultback);
    defaultback=malloc(strlen(fselret)+1);
    strcpy(defaultback,fselret);
    fl_set_object_label(objdefback,defaultback);
  }
  fl_activate_all_forms();
}

void ColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&icon);
  fl_set_object_label(iconobj,icon);  
  fl_activate_all_forms();
}

void DefaultColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&defaultback);
  fl_set_object_label(objdefback,defaultback);  
  fl_activate_all_forms();
}

void MFColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&mforecol);
  fl_set_object_label(objmforcol,mforecol);  
  fl_activate_all_forms();
}

void WFColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&wforecol);
  fl_set_object_label(objwforcol,wforecol);  
  fl_activate_all_forms();
}

void MBColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&mbackcol);
  fl_set_object_label(objmbaccol,mbackcol);  
  fl_activate_all_forms();
}

void WBColorselCallback(FL_OBJECT *obj, long arg)
{
  fl_deactivate_all_forms();
  SelectColor(&wbackcol);
  fl_set_object_label(objwbaccol,wbackcol);  
  fl_activate_all_forms();
}

void EraseCallback(FL_OBJECT *obj, long arg)
{
  free(icon);
  icon=NULL;
  fl_set_object_label(iconobj,"");  
}

void WindowOptions(int argc,char *argv[])
{
  FL_OBJECT *obj;
  FL_OBJECT *cancel;
  char* text;
  static Atom motif_wm_hints= None;
  MotifWmHints hints;

  name=malloc(strlen(argv[2])+1);
  strcpy(name,argv[2]);
   if (name[0]=='.' && name[1]=='/')
    name[0]='~';
  text=malloc(strlen("Window:")+strlen(name)+1);
  strcpy(text,"Window:");
  strcat(text,name);

  winame=GenerateWindowName(name,NORMAL);
  GetFileInfoFromDesktop(winame,&position,&icon,
			       &start);
  
  mainform=fl_bgn_form(FL_NO_BOX, 370, 280);
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,270,60,90,90,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    if (icon!=NULL)
      fl_set_pixmapbutton_file(obj,icon);
    fl_set_object_callback(obj,FileselCallback,0);
  obj = fl_add_box(FL_FLAT_BOX,0,0,270,240,"");
  obj = fl_add_box(FL_FLAT_BOX,360,0,10,240,"");
  obj = fl_add_box(FL_FLAT_BOX,0,0,370,60,"");
  obj = fl_add_box(FL_FLAT_BOX,0,150,370,130,"");

  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,200,350,30,"");
  cancel = fl_add_button(FL_NORMAL_BUTTON,190,240,170,30,"Cancel");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,240,170,30,"Apply");
    fl_set_object_callback(obj,WindowApplyCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,0,70,120,20,"Background(pixmap)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,0,90,20,"Window-Folder");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,20,350,30,name);
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  winhide = fl_add_checkbutton(FL_PUSH_BUTTON,10,160,160,30,"Show hidden \".*\" files in the Normal View");
  fl_set_button(winhide,1);
  if (position==NULL) 
    fl_deactivate_object(winhide);
  else if (position[0]=='a')
    fl_set_button(winhide,0);
  wininfobj = fl_add_checkbutton(FL_PUSH_BUTTON,10,200,160,30,"Windowinfo'll be saved");
    if (GetWindowSaveStatus(name)==True)
      fl_set_button(wininfobj,1);
  icoinfobj = fl_add_checkbutton(FL_PUSH_BUTTON,200,200,160,30,"Iconinfo'll be saved");
    if (GetIconSaveStatus(name)==True)
      fl_set_button(icoinfobj,1);
  iconobj = fl_add_text(FL_NORMAL_TEXT,10,90,260,30,icon);
    fl_set_object_boxtype(iconobj,FL_DOWN_BOX);
    fl_set_object_lalign(iconobj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,180,120,90,30,"Color");
    fl_set_object_callback(obj,ColorselCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,90,120,90,30,"Erase");
    fl_set_object_callback(obj,EraseCallback,0);
  fl_end_form();
  fl_set_app_mainform(mainform);
  fl_show_form(mainform,FL_PLACE_CENTER,FL_FULLBORDER,text);
  
  hints.flags=1;
  hints.functions=44;
  if ( motif_wm_hints== None)
    motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
  XChangeProperty(GetDisplay(), mainform->window, motif_wm_hints, motif_wm_hints, 32,
		  PropModePrepend, (unsigned char *) &hints, sizeof(MotifWmHints)/sizeof(CARD32));

  while(fl_do_forms()!=cancel);
  exit(0);
}

void MainWindowApplyCallback(FL_OBJECT *obj, long arg)
{
  Window *updwin;
  char text1[30],text2[30];
  char *test;
  int i;
  int r1;int g1;int b1;
  int r2;int g2;int b2;
  int r3;int g3;int b3;
  int r4;int g4;int b4;

  if (fl_get_button(obja)==1)
    strcpy(text1,"a");
  else if (fl_get_button(objb)==1)
    strcpy(text1,"b");
  else if (fl_get_button(objc)==1)
    strcpy(text1,"c");
  else 
    strcpy(text1," ");
  test=(char*)fl_get_input(objax);
  if (test[0]==0)   
    strcat(text1,"5");
  else
    strcat(text1,test);
  strcat(text1," ");
  test=(char*)fl_get_input(objay);
  if (test[0]==0)   
    strcat(text1,"5");
  else
    strcat(text1,test);
  strcat(text1," ");
  test=(char*)fl_get_input(objgrid);
  if (test[0]==0)   
    strcat(text1,"0");
  else
    strcat(text1,test);

  if (fl_get_button(objbottom)==0 && fl_get_button(objright)==1)
    strcpy(text2,"a");
  else if (fl_get_button(objbottom)==0 && fl_get_button(objright)==0)
    strcpy(text2,"b");
  else if (fl_get_button(objbottom)==1 && fl_get_button(objright)==1)
    strcpy(text2,"c");
  else 
    strcpy(text2," ");
  test=(char*)fl_get_input(objbx);
  if (test[0]==0)   
    strcat(text2,"5");
  else
    strcat(text2,test);
  strcat(text2," ");
  test=(char*)fl_get_input(objby);
  if (test[0]==0)   
    strcat(text2,"5");
  else
    strcat(text2,test);
  SetDefaultBackground(defaultback);
  EraseFileInfoInDesktop(".dfmdesk --- dfm:~/.dfmdesk/");  
  SendFileInfoToDesktop(".dfmdesk --- dfm:~/.dfmdesk/",text1,icon,text2);

  test=(char*)fl_get_input(objfont);
  if (test[0]==0)
    SendFontName("8x13");
  else
    SendFontName(test);

  test=(char*)fl_get_input(objcache);
  if (test[0]==0)   
    i=0;
  else
    sscanf(test,"%d",&i);
  SetWindowCache(i);

  test=(char*)fl_get_input(objtimer);
  if (test[0]==0)   
    i=40;
  else
    sscanf(test,"%d",&i);
  SendTimer(i);
  sscanf(mforecol,"# %d %d %d",&r1,&g1,&b1);
  sscanf(mbackcol,"# %d %d %d",&r2,&g2,&b2);
  sscanf(wforecol,"# %d %d %d",&r3,&g3,&b3);
  sscanf(wbackcol,"# %d %d %d",&r4,&g4,&b4);
  SendColors(r1,g1,b1,
	     r2,g2,b2,
	     r3,g3,b3,
	     r4,g4,b4);

  updwin=GetWindowNamed(".dfmdesk --- dfm:~/.dfmdesk/");
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  updwin=GetWindowNamed(".dfmdesk -d- dfm:~/.dfmdesk/");
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  updwin=GetWindowNamed(".dfmdesk -s- dfm:~/.dfmdesk/");
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
}

void MainWindowOptions(int argc,char *argv[])
{
  FL_OBJECT *obj;
  FL_OBJECT *cancel;
  int ax,ay,bx,by,grid;
  char text[10];  
  int r1;int g1;int b1;
  int r2;int g2;int b2;
  int r3;int g3;int b3;
  int r4;int g4;int b4;
  static Atom motif_wm_hints= None;
  MotifWmHints hints;


  GetFileInfoFromDesktop(".dfmdesk --- dfm:~/.dfmdesk/",&position,&icon,
			       &start);

  mainform=fl_bgn_form(FL_NO_BOX, 600, 470);
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,270,50,90,90,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    if (icon!=NULL)
      fl_set_pixmapbutton_file(obj,icon);
    fl_set_object_callback(obj,FileselCallback,0);
  obj = fl_add_box(FL_FLAT_BOX,0,0,270,470,"");
  obj = fl_add_box(FL_FLAT_BOX,360,0,240,470,"");
  obj = fl_add_box(FL_FLAT_BOX,0,0,600,50,"");
  obj = fl_add_box(FL_FLAT_BOX,0,140,600,340,"");
  /*obj = fl_add_text(FL_NORMAL_TEXT,370,95,220,30,"Use 0 (off) with Enlightenment!!!");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);*/
  obj = fl_add_text(FL_NORMAL_TEXT,10,150,180,20,"Iconplacement");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,10,340,180,20,"Windowmanager context");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,170,340,180,20,"Icontext color (desktop)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,370,320,180,20,"Icontext color (windows)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,60,120,20,"Background(pixmap)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,370,120,100,20,"Icon Font");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,150,350,180,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,370,50,220,370,"");
  cancel = fl_add_button(FL_NORMAL_BUTTON,305,430,285,30,"Cancel");
  obj = fl_add_button(FL_NORMAL_BUTTON,10,430,285,30,"Apply");
    fl_set_object_callback(obj,MainWindowApplyCallback,0);
  fl_bgn_group();
  objs = fl_add_roundbutton(FL_RADIO_BUTTON,10,170,150,30,"left->right, top->down");
  obja = fl_add_roundbutton(FL_RADIO_BUTTON,10,190,150,30,"top->down, left->right");
  objb = fl_add_roundbutton(FL_RADIO_BUTTON,200,170,150,30,"left->right, bottom->up");
  objc = fl_add_roundbutton(FL_RADIO_BUTTON,200,190,150,30,"top->down, right->left");
  fl_end_group();
  if (position[0]=='a')
    fl_set_button(obja,1);
  else if (position[0]=='b')
    fl_set_button(objb,1);
  else if (position[0]=='c')
    fl_set_button(objc,1);
  else
    fl_set_button(objs,1);
  GetPositionFromText(position,&ax,&ay,&grid);
  GetPositionFromText(start,&bx,&by,NULL);

  objax = fl_add_input(FL_INT_INPUT,70,250,70,30,"Left");
    sprintf(text,"%d",ax);
    fl_set_input(objax,text);
  objay = fl_add_input(FL_INT_INPUT,150,230,70,30,"Top");
    fl_set_object_lalign(objay,FL_ALIGN_TOP);
    sprintf(text,"%d",ay);
    fl_set_input(objay,text);
  objbx = fl_add_input(FL_INT_INPUT,230,250,70,30,"Right");
    fl_set_object_lalign(objbx,FL_ALIGN_RIGHT);
    sprintf(text,"%d",bx);
    fl_set_input(objbx,text);
  objby = fl_add_input(FL_INT_INPUT,150,270,70,30,"Bottom");
    fl_set_object_lalign(objby,FL_ALIGN_BOTTOM);
    sprintf(text,"%d",by);
    fl_set_input(objby,text);

  objgrid = fl_add_input(FL_INT_INPUT,70,290,70,30,"Grid tight");
    sprintf(text,"%d",grid);
    fl_set_input(objgrid,text);
  
  obj = fl_add_text(FL_NORMAL_TEXT,10,10,350,30,"Desktop");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,370,10,220,30,"Global");
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_ENGRAVED_STYLE);
  objbottom = fl_add_checkbutton(FL_PUSH_BUTTON,20,390,170,30,"bottom");
  objright = fl_add_checkbutton(FL_PUSH_BUTTON,20,360,160,30,"right");
  if (start[0]=='c'){
    fl_set_button(objbottom,1);
    fl_set_button(objright,1);
  } else if (start[0]=='a')
    fl_set_button(objright,1);
  else if (start[0]!='b')
    fl_set_button(objbottom,1);
  
  objcache = fl_add_input(FL_INT_INPUT,445,70,70,30,"Cache time(sec)");
    fl_set_object_lalign(objcache,FL_ALIGN_TOP);
    sprintf(text,"%d",GetWindowCache());
    fl_set_input(objcache,text);

  objfont = fl_add_input(FL_NORMAL_INPUT,380,140,200,30,"");
  UpdateFontName();
  UpdateTimer();
  UpdateColors();
  fl_set_input(objfont,GetFontName());  

  objtimer = fl_add_input(FL_INT_INPUT,445,280,70,30,"Updatetime(sec)");
    fl_set_object_lalign(objtimer,FL_ALIGN_TOP);
    sprintf(text,"%d",(int)GetTimer());
    fl_set_input(objtimer,text);

  defaultback=GetDefaultBackground();

  obj = fl_add_text(FL_NORMAL_TEXT,370,180,170,20,"Default Background(pixmap)");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  objdefback = fl_add_text(FL_NORMAL_TEXT,380,200,200,30,defaultback);
    fl_set_object_boxtype(objdefback,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,400,230,90,30,"Color");
    fl_set_object_callback(obj,DefaultColorselCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,490,230,90,30,"Pixmap");
    fl_set_object_callback(obj,DefaultFileselCallback,0);

  GetColorsRGB(&r1,&g1,&b1,
	       &r2,&g2,&b2,
	       &r3,&g3,&b3,
	       &r4,&g4,&b4);
  mbackcol=malloc(16);
  wbackcol=malloc(16);
  mforecol=malloc(16);
  wforecol=malloc(16);
  sprintf(mforecol,"# %d %d %d",r1,g1,b1);
  sprintf(mbackcol,"# %d %d %d",r2,g2,b2);
  sprintf(wforecol,"# %d %d %d",r3,g3,b3);
  sprintf(wbackcol,"# %d %d %d",r4,g4,b4);

  obj = fl_add_text(FL_NORMAL_TEXT,280,360,80,30,"Background");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,280,390,80,30,"Foreground");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,480,340,80,30,"Background");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,480,370,80,30,"Foreground");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  objmbaccol = fl_add_button(FL_NORMAL_BUTTON,180,360,100,30,mbackcol);
    fl_set_object_callback(objmbaccol,MBColorselCallback,0);
  objmforcol = fl_add_button(FL_NORMAL_BUTTON,180,390,100,30,mforecol);
    fl_set_object_callback(objmforcol,MFColorselCallback,0);
  objwbaccol = fl_add_button(FL_NORMAL_BUTTON,380,340,100,30,wbackcol);
    fl_set_object_callback(objwbaccol,WBColorselCallback,0);
  objwforcol = fl_add_button(FL_NORMAL_BUTTON,380,370,100,30,wforecol);
    fl_set_object_callback(objwforcol,WFColorselCallback,0);

  iconobj = fl_add_text(FL_NORMAL_TEXT,10,80,260,30,icon);
    fl_set_object_boxtype(iconobj,FL_DOWN_BOX);
    fl_set_object_lalign(iconobj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,90,110,90,30,"Erase");
    fl_set_object_callback(obj,EraseCallback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,180,110,90,30,"Color");
    fl_set_object_callback(obj,ColorselCallback,0);
  fl_end_form();
  fl_set_app_mainform(mainform);
  fl_show_form(mainform,FL_PLACE_CENTER,FL_FULLBORDER,"Desktop Options");
  
  hints.flags=1;
  hints.functions=44;
  if ( motif_wm_hints== None)
    motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
  XChangeProperty(GetDisplay(), mainform->window, motif_wm_hints, motif_wm_hints, 32,
		  PropModePrepend, (unsigned char *) &hints, sizeof(MotifWmHints)/sizeof(CARD32));

  while(fl_do_forms()!=cancel);
  exit(0);
}


void FileApplyCallback(FL_OBJECT *obj, long arg)
{
  Window *updwin;
  char *pos,*dummy1,*dummy2;
  char *oldname;
  char *text;
  char *text2;
  char *textn;
  int x,y;
  char* text3;
  char linktext[1024];
  char* ren;
  struct stat info;
  struct passwd *username;
  struct group *groupname;
  char *number;
  int j;

  ren=GenerateLastName(name);
  GetFileInfoFromDesktop(name,&pos,&dummy1,&dummy2);
  free(dummy1);
  free(dummy2);
  text=(char*)fl_get_input(objren);
  if (strcmp(text,ren)!=0) {
    oldname=malloc(strlen(name)+1);
    strcpy(oldname,name);
    if (oldname[strlen(oldname)-1]=='/')
      oldname[strlen(oldname)-1]=0;
    text2=malloc(strlen(oldname)+strlen(text)+1);
    strcpy(text2,oldname);
    strcpy(text2+strlen(oldname)-strlen(ren),text);
    if (oldname[0]=='~' && oldname[1]=='/')
      oldname[0]='.';
    if (text2[0]=='~' && text2[1]=='/')
      text2[0]='.';
    if (FileExist(text2)==False && rename(oldname,text2)==0) {
      if (text2[0]=='.' && text2[1]=='/')
	text2[0]='~';
      if (name[strlen(name)-1]=='/')
       	strcat(text2,"/");
      EraseFileInfoInDesktop(name);
      free(name);
      name=text2;
    } else
      free(text2);
    free(oldname);
  }
  free(ren);
  if (pos!=NULL) {
    GetPositionFromText(pos,&x,&y,NULL);
    if (fl_get_button(obja)==1)
      SetPositionToText(&position,x,y,'a');
    else
      SetPositionToText(&position,x,y,' ');
  } else {
    fl_set_button(obja,0);
    position=NULL;
  }
  free(pos);
  pos=strdup((char*)fl_get_input(objs));
  j=0;
  while (pos[j]!=0) {
    if (pos[j]==';')
       pos[j]=' ';
    j++;
  }
  SendFileInfoToDesktop(name,position,icon,pos/*shellcommand*/);
  free(pos);
  free(position);
  textn=GenerateSlashName(name);
  text=malloc(strlen("chown  ")+strlen(textn)+strlen((char*)fl_get_input(objax))+1);
  strcpy(text,"chown ");
  strcat(text,(char*)fl_get_input(objax));
  strcat(text," ");
  strcat(text,textn);
  usershellsystem(text);
  free(text);
  text=malloc(strlen("chgrp  ")+strlen(textn)+strlen((char*)fl_get_input(objbx))+1);
  strcpy(text,"chgrp ");
  strcat(text,(char*)fl_get_input(objbx));
  strcat(text," ");
  strcat(text,textn);
  usershellsystem(text);
  free(text);
  text3=malloc(strlen(name)+1);
  strcpy(text3,name);
  if (text3[0]=='~' && text3[1]=='/')
    text3[0]='.';
  text2=malloc(strlen("File:")+strlen(name)+1);
  strcpy(text2,"File:");
  strcat(text2,name);
  ren=GenerateLastName(name);
  fl_set_object_label(objname,name);
  fl_set_input(objren,ren);
  if (fl_get_button(icoinfobj)==1)
    SetIconSaveStatus(winame,True);
  else
    SetIconSaveStatus(winame,False);
  lstat(text3,&info);
  if ((info.st_mode&S_IFMT)!=S_IFLNK) {
    if ((info.st_mode&S_ISUID)==0 &&
	(info.st_mode&S_ISGID)==0 ) {
      text=malloc(strlen("chmod XXX ")+strlen(textn)+1);
      strcpy(text,"chmod 000 ");
      strcat(text,textn);
      if (fl_get_button(objor)==1)
	text[6]=text[6]+4;
      if (fl_get_button(objow)==1)
	text[6]=text[6]+2;
      if (fl_get_button(objox)==1)
	text[6]++;
      if (fl_get_button(objgr)==1)
	text[7]=text[7]+4;
      if (fl_get_button(objgw)==1)
	text[7]=text[7]+2;
      if (fl_get_button(objgx)==1)
	text[7]++;
      if (fl_get_button(objwr)==1)
	text[8]=text[8]+4;
      if (fl_get_button(objww)==1)
	text[8]=text[8]+2;
      if (fl_get_button(objwx)==1)
	text[8]++;
      usershellsystem(text);
      free(text);
    }
    sprintf(linktext,"%ld",info.st_size);
    fl_set_object_label(objsize,linktext);
  }
  free(textn);
  sprintf(linktext,"%s",ctime(&(info.st_mtime)));
  if (linktext[strlen(linktext)-1]==10)
    linktext[strlen(linktext)-1]=0;
  fl_set_object_label(objtime,linktext);
    username=getpwuid(info.st_uid);
    if (username!=NULL)
      fl_set_input(objax,username->pw_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_uid);
      fl_set_input(objax,number);
      free(number);
    }
    groupname=getgrgid(info.st_gid);
    if (groupname!=NULL) 
      fl_set_input(objbx,groupname->gr_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_gid);
      fl_set_input(objbx,number);
      free(number);
    }
  stat(text3,&info);
  if ((info.st_mode&S_IRUSR)!=0) 
    fl_set_button(objor,1);
  else
    fl_set_button(objor,0);
  if ((info.st_mode&S_IWUSR)!=0) 
    fl_set_button(objow,1);
  else
    fl_set_button(objow,0);
  if ((info.st_mode&S_IXUSR)!=0) 
    fl_set_button(objox,1);
  else
    fl_set_button(objox,0);
  if ((info.st_mode&S_IRGRP)!=0) 
    fl_set_button(objgr,1);
  else
    fl_set_button(objgr,0);
  if ((info.st_mode&S_IWGRP)!=0) 
    fl_set_button(objgw,1);
  else
    fl_set_button(objgw,0);
  if ((info.st_mode&S_IXGRP)!=0) 
    fl_set_button(objgx,1);
  else
    fl_set_button(objgx,0);
  if ((info.st_mode&S_IROTH)!=0) 
    fl_set_button(objwr,1);
  else
    fl_set_button(objwr,0);
  if ((info.st_mode&S_IWOTH)!=0) 
    fl_set_button(objww,1);
  else
    fl_set_button(objww,0);
  if ((info.st_mode&S_IXOTH)!=0) 
    fl_set_button(objwx,1);
  else
    fl_set_button(objwx,0);
  fl_set_form_title(mainform,text2);
  text=GenerateWindowName(winame,NORMAL);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
  text=GenerateWindowName(winame,DETAIL);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
  free(text3);
  free(text2);
  free(ren);
}

void FileOptions(int argc,char *argv[])
{
  FL_OBJECT *obj;
  FL_OBJECT *cancel;
  int status;
  char* text2;
  char* text3;
  char linktext[1024];
  char* ren;
  struct stat info;
  struct passwd *username;
  struct group *groupname;
  char *number;
  static Atom motif_wm_hints= None;
  MotifWmHints hints;

  name=malloc(strlen(argv[2])+1);
  text3=malloc(strlen(argv[2])+1);
  strcpy(name,argv[2]);
  strcpy(text3,argv[2]);
  if (text3[strlen(text3)-1]=='/')
    text3[strlen(text3)-1]=0;
  if (name[0]=='.' && name[1]=='/')
    name[0]='~';
  text2=malloc(strlen("File:")+strlen(name)+1);
  strcpy(text2,"File:");
  strcat(text2,name);
  ren=GenerateLastName(name);
  winame=malloc(strlen(name)+1);
  strcpy(winame,name);
  winame[strlen(text3)-strlen(ren)]=0;
  GetLinkNameWithoutLast(&winame);
  
  GetFileInfoFromDesktop(name,&position,&icon,
			       &start);
  lstat(text3,&info);

  mainform=fl_bgn_form(FL_NO_BOX, 370, 480);
  obj = fl_add_box(FL_FLAT_BOX,0,0,370,480,"");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,270,110,90,90,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    if (icon!=NULL)
      fl_set_pixmapbutton_file(obj,icon);
    fl_set_object_callback(obj,FileselCallback,0);
  obj = fl_add_box(FL_FLAT_BOX,0,0,270,480,"");
  obj = fl_add_box(FL_FLAT_BOX,360,0,10,480,"");
  obj = fl_add_box(FL_FLAT_BOX,0,0,370,110,"");
  obj = fl_add_box(FL_FLAT_BOX,0,200,370,280,"");
  cancel = fl_add_button(FL_NORMAL_BUTTON,190,440,170,30,"Cancel");
  obj = fl_add_text(FL_NORMAL_TEXT,0,0,90,20,"File");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,10,440,170,30,"Apply");
    fl_set_object_callback(obj,FileApplyCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,0,120,90,20,"Icon");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    obj = fl_add_text(FL_NORMAL_TEXT,0,300,150,20,"Rights");
      fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,320,40,30,"Owner");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,360,40,30,"Group");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,400,40,30,"World");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
      objax = fl_add_input(FL_NORMAL_INPUT,40,320,140,30,"");
    username=getpwuid(info.st_uid);
    if (username!=NULL)
      fl_set_input(objax,username->pw_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_uid);
      fl_set_input(objax,number);
      free(number);
    }
      objbx = fl_add_input(FL_NORMAL_INPUT,40,360,140,30,"");
    groupname=getgrgid(info.st_gid);
    if (groupname!=NULL) 
      fl_set_input(objbx,groupname->gr_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_gid);
      fl_set_input(objbx,number);
      free(number);
    }
  obj = fl_add_text(FL_NORMAL_TEXT,40,400,140,30,"All");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  objs = fl_add_input(FL_NORMAL_INPUT,10,270,350,30,"");
    fl_set_input(objs,start);
  obj = fl_add_text(FL_NORMAL_TEXT,0,250,90,20,"Shellcommand");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  objren = fl_add_input(FL_NORMAL_INPUT,10,70,350,30,"");
    fl_set_input(objren,ren);
  obj = fl_add_text(FL_NORMAL_TEXT,0,50,90,20,"Rename to");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  if ((info.st_mode&S_IFMT)==S_IFLNK) {
    obj = fl_add_text(FL_NORMAL_TEXT,180,300,150,20,"Mode of the linked file:");
      fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    obj = fl_add_text(FL_NORMAL_TEXT,0,200,90,20,"Link to");
      fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    status=readlink(text3,linktext,1023);
    linktext[status]=0;
  } else {
    if ((info.st_mode&S_ISUID)!=0 ||
	(info.st_mode&S_ISGID)!=0 ) {
      obj = fl_add_text(FL_NORMAL_TEXT,180,300,180,20,"Use better the chmod command:");
        fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);      
    }
    obj = fl_add_text(FL_NORMAL_TEXT,0,200,90,20,"Size");
      fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    sprintf(linktext,"%ld",info.st_size);
  }
  objsize = fl_add_text(FL_NORMAL_TEXT,10,220,190,30,linktext);
    fl_set_object_boxtype(objsize,FL_DOWN_BOX);
    fl_set_object_lalign(objsize,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,200,200,90,20,"Time");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  sprintf(linktext,"%s",ctime(&(info.st_mtime)));
  if (linktext[strlen(linktext)-1]==10)
    linktext[strlen(linktext)-1]=0;
  objtime = fl_add_text(FL_NORMAL_TEXT,210,220,150,30,linktext);
    fl_set_object_boxtype(objtime,FL_DOWN_BOX);
    fl_set_object_lalign(objtime,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obja = fl_add_checkbutton(FL_PUSH_BUTTON,10,170,130,30,"Prevent moving");
  if (position!=NULL && position[0]=='a') 
    fl_set_button(obja,1);
  icoinfobj = fl_add_checkbutton(FL_PUSH_BUTTON,130,170,110,30,"Iconinfo'll be saved");
  if (GetIconSaveStatus(winame)==True)
      fl_set_button(icoinfobj,1);
  if (strcmp(winame,"~/.dfmdesk/")==0)
    fl_deactivate_object(icoinfobj);
  objname = fl_add_text(FL_NORMAL_TEXT,10,20,350,30,name);
    fl_set_object_boxtype(objname,FL_DOWN_BOX);
    fl_set_object_lalign(objname,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  iconobj = fl_add_text(FL_NORMAL_TEXT,10,140,260,30,icon);
    fl_set_object_boxtype(iconobj,FL_DOWN_BOX);
    fl_set_object_lalign(iconobj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  objmode=fl_bgn_group();
  objox = fl_add_lightbutton(FL_PUSH_BUTTON,310,320,50,30,"exec");
  objgx = fl_add_lightbutton(FL_PUSH_BUTTON,310,360,50,30,"exec");
  objwx = fl_add_lightbutton(FL_PUSH_BUTTON,310,400,50,30,"exec");
  objow = fl_add_lightbutton(FL_PUSH_BUTTON,250,320,50,30,"write");
  objgw = fl_add_lightbutton(FL_PUSH_BUTTON,250,360,50,30,"write");
  objww = fl_add_lightbutton(FL_PUSH_BUTTON,250,400,50,30,"write");
  objor = fl_add_lightbutton(FL_PUSH_BUTTON,190,320,50,30,"read");
  objgr = fl_add_lightbutton(FL_PUSH_BUTTON,190,360,50,30,"read");
  objwr = fl_add_lightbutton(FL_PUSH_BUTTON,190,400,50,30,"read");
  fl_end_group();
  if ((info.st_mode&S_IFMT)==S_IFLNK ||
      (info.st_mode&S_ISUID)!=0 ||
      (info.st_mode&S_ISGID)!=0 )
    fl_deactivate_object(objmode);
  stat(text3,&info);
  if ((info.st_mode&S_IRUSR)!=0) 
    fl_set_button(objor,1);
  if ((info.st_mode&S_IWUSR)!=0) 
    fl_set_button(objow,1);
  if ((info.st_mode&S_IXUSR)!=0) 
    fl_set_button(objox,1);
  if ((info.st_mode&S_IRGRP)!=0) 
    fl_set_button(objgr,1);
  if ((info.st_mode&S_IWGRP)!=0) 
    fl_set_button(objgw,1);
  if ((info.st_mode&S_IXGRP)!=0) 
    fl_set_button(objgx,1);
  if ((info.st_mode&S_IROTH)!=0) 
    fl_set_button(objwr,1);
  if ((info.st_mode&S_IWOTH)!=0) 
    fl_set_button(objww,1);
  if ((info.st_mode&S_IXOTH)!=0) 
    fl_set_button(objwx,1);
  fl_end_form();
  free(position);
  fl_set_app_mainform(mainform);
  fl_show_form(mainform,FL_PLACE_CENTER,FL_FULLBORDER,text2);
  
  hints.flags=1;
  hints.functions=44;
  if ( motif_wm_hints== None)
    motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
  XChangeProperty(GetDisplay(), mainform->window, motif_wm_hints, motif_wm_hints, 32,
		  PropModePrepend, (unsigned char *) &hints, sizeof(MotifWmHints)/sizeof(CARD32));
  
  fl_set_focus_object(mainform,objren);
  while(fl_do_forms()!=cancel);
  exit(0);
}

void MultipleFileApplyCallback(FL_OBJECT *obj, long arg)
{
  Window *updwin;
  char *pos,*dummy1,*dummy2;
  int x,y,i,j;
  char* text;
  char* textn;
  
  if (fl_get_button(icoinfobj)==1)
    SetIconSaveStatus(winame,True);
  else
    SetIconSaveStatus(winame,False);

  for (i=0;i<gargc-2;i++) {
    GetFileInfoFromDesktop(filearray[i],&pos,&dummy1,&dummy2);
    free(dummy1);
    free(dummy2);
    if (pos!=NULL) {
      GetPositionFromText(pos,&x,&y,NULL);
      if (fl_get_button(obja)==1)
	SetPositionToText(&position,x,y,'a');
      else
	SetPositionToText(&position,x,y,' ');
    } else {
      position=NULL;
    }
    free(pos);
    pos=strdup((char*)fl_get_input(objs));
    j=0;
    while (pos[j]!=0) {
      if (pos[j]==';')
        pos[j]=' ';
      j++;
    }
    if (fl_get_button(objchangep)==1)
      SendFileInfoToDesktop(filearray[i],position,NULL,NULL);
    if (fl_get_button(objchangei)==1)
      SendFileInfoToDesktop(filearray[i],NULL,icon,NULL);
    if (fl_get_button(objchanges)==1)
      SendFileInfoToDesktop(filearray[i],NULL,NULL,pos/*shellcommand*/);
    free(pos);
    free(position);
    
    textn=GenerateSlashName(filearray[i]);
    if (fl_get_button(objchangeo)==1) {
      text=malloc(strlen("chown  ")+strlen(textn)+strlen((char*)fl_get_input(objax))+1);
      strcpy(text,"chown ");
      strcat(text,(char*)fl_get_input(objax));
      strcat(text," ");
      strcat(text,textn);
      usershellsystem(text);
      free(text);
      text=malloc(strlen("chgrp  ")+strlen(textn)+strlen((char*)fl_get_input(objbx))+1);
      strcpy(text,"chgrp ");
      strcat(text,(char*)fl_get_input(objbx));
      strcat(text," ");
      strcat(text,textn);
      usershellsystem(text);
      free(text);
    }

    if (fl_get_button(objchangem)==1) {
      text=malloc(strlen("chmod XXX ")+strlen(textn)+1);
      strcpy(text,"chmod 000 ");
      strcat(text,textn);
      if (fl_get_button(objor)==1)
	text[6]=text[6]+4;
      if (fl_get_button(objow)==1)
	text[6]=text[6]+2;
      if (fl_get_button(objox)==1)
	text[6]++;
      if (fl_get_button(objgr)==1)
	text[7]=text[7]+4;
      if (fl_get_button(objgw)==1)
	text[7]=text[7]+2;
      if (fl_get_button(objgx)==1)
	text[7]++;
      if (fl_get_button(objwr)==1)
	text[8]=text[8]+4;
      if (fl_get_button(objww)==1)
	text[8]=text[8]+2;
      if (fl_get_button(objwx)==1)
	text[8]++;
      usershellsystem(text);
      free(text);
    }
    free(textn);
  }
  text=GenerateWindowName(winame,NORMAL);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
  text=GenerateWindowName(winame,DETAIL);
  updwin=GetWindowNamed(text);
  if (updwin!=NULL) 
    SendWindow(*updwin,UPDATE);
  free(text);
}

void MultipleFileOptions(int argc,char *argv[])
{
  FL_OBJECT *obj;
  FL_OBJECT *cancel;
  int i;
  char *text;
  struct stat info;
  struct passwd *username;
  struct group *groupname;
  char *number;
  static Atom motif_wm_hints= None;
  MotifWmHints hints;
  
  gargc=argc;
  gargv=argv;

  filearray=malloc(sizeof(char*)*(argc-2));
  for (i=2;i<argc;i++) {
    filearray[i-2]=malloc(strlen(argv[i])+1);
    strcpy(filearray[i-2],argv[i]);
    if (filearray[i-2][0]=='.' && filearray[i-2][1]=='/')
      filearray[i-2][0]='~';
  }
  text=GenerateLastNameWithSlash(filearray[0]);
  winame=malloc(strlen(filearray[0])+1);
  strcpy(winame,filearray[0]);
  winame[strlen(filearray[0])-strlen(text)]=0;
  GetLinkNameWithoutLast(&winame);
  free(text);
  GetFileInfoFromDesktop(filearray[0],&position,&icon,
			 &start);
  
  lstat(argv[2],&info);
  mainform=fl_bgn_form(FL_NO_BOX, 370, 460);
  obj = fl_add_box(FL_FLAT_BOX,0,0,370,460,"");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,270,110,90,90,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
    if (icon!=NULL)
      fl_set_pixmapbutton_file(obj,icon);
    fl_set_object_callback(obj,FileselCallback,0);
  obj = fl_add_box(FL_FLAT_BOX,0,0,270,460,"");
  obj = fl_add_box(FL_FLAT_BOX,360,0,10,460,"");
  obj = fl_add_box(FL_FLAT_BOX,0,0,370,110,"");
  obj = fl_add_box(FL_FLAT_BOX,0,200,370,260,"");
  cancel = fl_add_button(FL_NORMAL_BUTTON,190,420,170,30,"Cancel");
  obj = fl_add_text(FL_NORMAL_TEXT,0,0,90,20,"Files");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,10,420,170,30,"Apply");
    fl_set_object_callback(obj,MultipleFileApplyCallback,0);
  obj = fl_add_text(FL_NORMAL_TEXT,0,300,40,30,"Owner");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,340,40,30,"Group");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,0,380,40,30,"World");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
      objax = fl_add_input(FL_NORMAL_INPUT,40,300,140,30,"");
    username=getpwuid(info.st_uid);
    if (username!=NULL)
      fl_set_input(objax,username->pw_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_uid);
      fl_set_input(objax,number);
      free(number);
    }
      objbx = fl_add_input(FL_NORMAL_INPUT,40,340,140,30,"");
    groupname=getgrgid(info.st_gid);
    if (groupname!=NULL) 
      fl_set_input(objbx,groupname->gr_name);
    else {
      number=malloc(13);
      sprintf(number,"%d",info.st_gid);
      fl_set_input(objbx,number);
      free(number);
    }
  obj = fl_add_text(FL_NORMAL_TEXT,40,380,140,30,"All");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  objmode=fl_bgn_group();
  objox = fl_add_lightbutton(FL_PUSH_BUTTON,310,300,50,30,"exec");
  objgx = fl_add_lightbutton(FL_PUSH_BUTTON,310,340,50,30,"exec");
  objwx = fl_add_lightbutton(FL_PUSH_BUTTON,310,380,50,30,"exec");
  objow = fl_add_lightbutton(FL_PUSH_BUTTON,250,300,50,30,"write");
  objgw = fl_add_lightbutton(FL_PUSH_BUTTON,250,340,50,30,"write");
  objww = fl_add_lightbutton(FL_PUSH_BUTTON,250,380,50,30,"write");
  objor = fl_add_lightbutton(FL_PUSH_BUTTON,190,300,50,30,"read");
  objgr = fl_add_lightbutton(FL_PUSH_BUTTON,190,340,50,30,"read");
  objwr = fl_add_lightbutton(FL_PUSH_BUTTON,190,380,50,30,"read");
  fl_end_group();
  obj = fl_add_browser(FL_NORMAL_BROWSER,10,20,350,85,"");
 for (i=2;i<argc;i++) {
    text=GenerateLastNameWithSlash(argv[i]);
    fl_addto_browser(obj,text);
    free(text);
 }
  objs = fl_add_input(FL_NORMAL_INPUT,10,240,350,30,"");
    fl_set_input(objs,start);
  objchangei = fl_add_checkbutton(FL_PUSH_BUTTON,0,120,90,20,"Change icon");
  objchanges = fl_add_checkbutton(FL_PUSH_BUTTON,0,220,100,20,"Change startcall");
  objchangeo = fl_add_checkbutton(FL_PUSH_BUTTON,0,280,100,20,"Change owners");
  objchangep = fl_add_checkbutton(FL_PUSH_BUTTON,0,180,90,20,"Change preventing");
  objchangem = fl_add_checkbutton(FL_PUSH_BUTTON,180,280,100,20,"Change mode");
  for (i=2;i<argc;i++) {
    lstat(argv[i],&info);
    if ((info.st_mode&S_IFMT)==S_IFLNK ||
	(info.st_mode&S_ISUID)!=0 ||
	(info.st_mode&S_ISGID)!=0 ) {
      fl_deactivate_object(objchangem); 
      fl_deactivate_object(objmode); 
    }
  }
  stat(argv[2],&info);
  if ((info.st_mode&S_IRUSR)!=0) 
    fl_set_button(objor,1);
  if ((info.st_mode&S_IWUSR)!=0) 
    fl_set_button(objow,1);
  if ((info.st_mode&S_IXUSR)!=0) 
    fl_set_button(objox,1);
  if ((info.st_mode&S_IRGRP)!=0) 
    fl_set_button(objgr,1);
  if ((info.st_mode&S_IWGRP)!=0) 
    fl_set_button(objgw,1);
  if ((info.st_mode&S_IXGRP)!=0) 
    fl_set_button(objgx,1);
  if ((info.st_mode&S_IROTH)!=0) 
    fl_set_button(objwr,1);
  if ((info.st_mode&S_IWOTH)!=0) 
    fl_set_button(objww,1);
  if ((info.st_mode&S_IXOTH)!=0) 
    fl_set_button(objwx,1);
  iconobj = fl_add_text(FL_NORMAL_TEXT,10,140,260,30,icon);
    fl_set_object_boxtype(iconobj,FL_DOWN_BOX);
    fl_set_object_lalign(iconobj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obja = fl_add_checkbutton(FL_PUSH_BUTTON,10,190,130,30,"Prevent moving");
  if (position!=NULL && position[0]=='a') 
    fl_set_button(obja,1);
  icoinfobj = fl_add_checkbutton(FL_PUSH_BUTTON,220,210,110,30,"Iconinfo'll be saved");
  if (GetIconSaveStatus(winame)==True)
      fl_set_button(icoinfobj,1);
  if (strcmp(winame,"~/.dfmdesk/")==0)
    fl_deactivate_object(icoinfobj);
  fl_end_form();
  fl_set_app_mainform(mainform);
  fl_show_form(mainform,FL_PLACE_CENTER,FL_FULLBORDER,winame);
  
  hints.flags=1;
  hints.functions=44;
  if ( motif_wm_hints== None)
    motif_wm_hints= XInternAtom(GetDisplay(), "_MOTIF_WM_HINTS", False);
  XChangeProperty(GetDisplay(), mainform->window, motif_wm_hints, motif_wm_hints, 32,
		  PropModePrepend, (unsigned char *) &hints, sizeof(MotifWmHints)/sizeof(CARD32));
  
  while(fl_do_forms()!=cancel);
  exit(0);
}
