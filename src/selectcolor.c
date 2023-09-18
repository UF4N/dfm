/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <forms.h>
#include <malloc.h>
#include "selectcolor.h"

int colorexitvar=3;
FL_FORM *colorform;
FL_OBJECT *pobj;
int r,g,b;


int colordestroying(FL_FORM *form,void *a)
{
  colorexitvar=1;
  return FL_IGNORE;
}

void colorExitCallback(FL_OBJECT *obj, long arg)
{
  colorexitvar=1;
}

void colorApplyCallback(FL_OBJECT *obj, long arg)
{
  colorexitvar=2;
}

void updatePreview()
{
  fl_mapcolor(FL_FREE_COL1, r, g, b);
  fl_redraw_object(pobj);
}

void colorR(FL_OBJECT *obj, long arg)
{
  r = (int)fl_get_slider_value(obj);
  updatePreview();
}

void colorG(FL_OBJECT *obj, long arg)
{
  g = (int)fl_get_slider_value(obj);
  updatePreview();
}

void colorB(FL_OBJECT *obj, long arg)
{ 
  b = (int)fl_get_slider_value(obj);
  updatePreview();
}

void SelectColor(char **oldnew)
{
  FL_OBJECT *obj;

  if (colorexitvar==3) {
    colorexitvar=0;
    if (oldnew!=NULL && *oldnew!=NULL && (*oldnew)[0]=='#' && (*oldnew)[1]==' ')
      sscanf(*oldnew,"# %d %d %d",&r,&g,&b);
    else {
      r=255;g=255;b=255;
    }
    colorform = fl_bgn_form(FL_NO_BOX, 190, 340);
    obj = fl_add_box(FL_FLAT_BOX,0,0,190,340,"");
    obj = fl_add_valslider(FL_VERT_SLIDER,70,10,30,270,"Red");
      fl_set_slider_bounds (obj, 0.0, 255.0);
      fl_set_slider_step (obj, 1.0);
      fl_set_slider_precision (obj, 0);
      fl_set_slider_value(obj, (double)r);
      fl_set_object_callback(obj,colorR,0);
      fl_set_object_color(obj,FL_COL1,FL_RED);
      fl_set_slider_increment(obj,1.0,1.0);
    obj = fl_add_valslider(FL_VERT_SLIDER,110,10,30,270,"Green");
      fl_set_slider_bounds (obj, 0.0, 255.0);
      fl_set_slider_step (obj, 1.0);
      fl_set_slider_precision (obj, 0);
      fl_set_slider_value(obj, (double)g);
      fl_set_object_callback(obj,colorG,0);
      fl_set_object_color(obj,FL_COL1,FL_GREEN);
      fl_set_slider_increment(obj,1.0,1.0);
    obj = fl_add_valslider(FL_VERT_SLIDER,150,10,30,270,"Blue");
      fl_set_slider_bounds (obj, 0.0, 255.0);
      fl_set_slider_step (obj, 1.0);
      fl_set_slider_precision (obj, 0);
      fl_set_slider_value(obj, (double)b);
      fl_set_object_callback(obj,colorB,0);
      fl_set_object_color(obj,FL_COL1,FL_BLUE);
      fl_set_slider_increment(obj,1.0,1.0);
    obj = fl_add_button(FL_NORMAL_BUTTON,10,300,80,30,"Apply");
      fl_set_object_callback(obj,colorApplyCallback,0);
    obj = fl_add_button(FL_NORMAL_BUTTON,100,300,80,30,"Cancel");
      fl_set_object_callback(obj,colorExitCallback,0);
    fl_mapcolor(FL_FREE_COL1, r, g, b);
    pobj = fl_add_box(FL_DOWN_BOX,10,10,50,270,"");
      fl_set_object_color (pobj, FL_FREE_COL1, 0);
    fl_end_form();
    fl_set_form_atclose(colorform,colordestroying,"");
    fl_show_form(colorform,FL_PLACE_CENTER,FL_TRANSIENT,"Info");
    while(colorexitvar==0)
      fl_check_forms();
    fl_hide_form(colorform);
    fl_free_form(colorform);
    if (colorexitvar==2) {
      free(*oldnew);
      *oldnew=malloc(20);
      sprintf(*oldnew,"# %d %d %d",r,g,b);
    }
    colorexitvar=3;
  }
}

