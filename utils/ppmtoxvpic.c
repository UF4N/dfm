#include <stdio.h>

int main() 
{
  int typ1,typ2,i,r,g,b=0,out,x,y,dummy;
  typ1=getchar();
  typ2=getchar();
  if (typ1==80 && typ2==54) {
    scanf("%d %d",&x,&y);
    scanf("%d",&dummy);
    printf("P7 332\n");
    printf("#created by ppmtoxvpic V1 (w) 1997\n");
    printf("#END_OF_COMMENTS\n");
    printf("%d %d 255\n",x,y);
    getchar();
    for (i=0;i<x*y;i++) {
      r=getchar();
      g=getchar();
      b=getchar();
      out=r&224;
      out=out|((g&224)/8);
      out=out|((b&192)/64);
      putchar(out);
    }
  } else if (typ1==80 && typ2==53) {
    scanf("%d %d",&x,&y);
    scanf("%d",&dummy);
    printf("P7 332\n");
    printf("#created by ppmtoxvpic V1 (w) 1997\n");
    printf("#END_OF_COMMENTS\n");
    printf("%d %d 255\n",x,y);
    getchar();
    for (i=0;i<x*y;i++) {
      b=getchar();
      out=b&224;
      out=out|((b&224)/8);
      out=out|((b&192)/64);
      putchar(out);
    }
  } else if (typ1==80 && typ2==50) {
    scanf("%d %d",&x,&y);
    scanf("%d",&dummy);
    printf("P7 332\n");
    printf("#created by ppmtoxvpic V1 (w) 1997\n");
    printf("#END_OF_COMMENTS\n");
    printf("%d %d 255\n",x,y);
    getchar();
    for (i=0;i<x*y;i++) {
      scanf("%d",&b);
      b=(b*255)/dummy;
      out=b&224;
      out=out|((b&224)/8);
      out=out|((b&192)/64);
      putchar(out);
    }
  } else
    exit(-1);
  if (b==-1)
    exit(-1);
  else
    exit(0);
}
