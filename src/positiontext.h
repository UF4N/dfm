/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef POSITIONTEXT_H
#define POSITIONTEXT_H

void GetPositionFromText(char *text,int *x,int *y,int *grid);

void SetPositionToText(char **text,int x,int y,char marker);

#endif

