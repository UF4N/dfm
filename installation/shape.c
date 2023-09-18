#include <X.h>
#include <Xlib.h>
#include <Xutil.h>
#include <extensions/shape.h>

main()
{
  Display *d;
  Window w;
  Pixmap p;
  XShapeCombineMask(d,w,0,0,0,p,0);
}
