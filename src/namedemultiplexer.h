/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

/* Inter-Window-Communication */

#ifndef NAMEDEMUX_H
#define NAMEDEMUX_H

#define NORMAL 0
#define REAL 1

int CopyCall(int x,int y,char *filenames,char *subfolder,int kind,int move);

void EraseCall(char *filenames,int ask,int remote);

void LinkCall(int x,int y,char *filenames,char *subfolder);

#endif
