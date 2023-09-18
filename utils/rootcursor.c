#include <Xlib.h>
#include <Xutil.h>
#include <cursorfont.h>
#include <stdlib.h>

int main(int argc,char *argv[])  
{
  Display *dis;
  XColor fore,back;
  Cursor curs;
  dis=XOpenDisplay((char*)getenv("DISPLAY"));
  curs=XCreateFontCursor(dis,XC_top_left_arrow);
  /*curs=XCreateFontCursor(dis,XC_left_ptr);*/
  fore.red  =0;
  fore.green=0;
  fore.blue =0;
  back.red  =65535;
  back.green=65535;
  back.blue =65535;
  XRecolorCursor(dis,curs,&fore,&back);
  XDefineCursor(dis,XDefaultRootWindow(dis),curs);
  XCloseDisplay(dis);
  exit(0);
}
