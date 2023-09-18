/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef WINDOWNAME_H
#define WINDOWNAME_H

char* GenerateSlashName(char* input);
char* DeSlashName(char* input);
char* GenerateWindowName(char* pathname,int type);
char* GenerateIconName(char* pathname);
char* GenerateLastNameWithSlash(char* filename);

#endif
