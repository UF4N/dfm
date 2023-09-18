/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef SHELLGUI_H
#define SHELLGUI_H

void ShellWindowInit();

void ShellWindowInfo(char *name);

int ShellWindowUpdate();

int ShellWindowDoIt(char** argv,int cont);

void ShellWindowDestroy();

#endif

