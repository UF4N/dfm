/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "positiontext.h"
#include <malloc.h>
#include <stdio.h>

void GetPositionFromText(char *text,int *x,int *y,int *grid)
{
  int a=0;
  if (text!=NULL) {
    if (text[0]==' ' || /* moving allowed, left->right top-down, context bottom*/ 
	text[0]=='a' || /* moving not allowed,  top->down left->right, context right*/
	text[0]=='b' || /* bottom->up left->right, no context*/
	text[0]=='c')   /* left->right bottom->up, context both*/
      a=1;
    if (grid==NULL)
      sscanf(text+a,"%d %d",x,y);
    else
      if (a==1)
	sscanf(text+a,"%d %d %d",x,y,grid);
      else {
	sscanf(text+a,"%d %d",x,y);
	*grid=0;
      }
  } else {
    *x=0;
    *y=0;
    if (grid!=NULL)
      *grid=50;
  }
}

void SetPositionToText(char **text,int x,int y,char marker)
{
  char helpstring[15];

  sprintf(helpstring," %d %d",x,y);
  helpstring[0]=marker;
  *text=malloc(strlen(helpstring)+1);
  strcpy(*text,helpstring); 
}



