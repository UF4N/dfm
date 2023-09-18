/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "testinstall.h"
#include "shell.h"
#include "filemanager.h"
#include <Xlib.h>
#include <string.h>
#include <stdlib.h>

void makehomelinks()
{
  usershellsystem("cd .dfmdesk ; rm -f .linktohomedir .linktorootdir ; ln -s ~/ .linktohomedir ; ln -s / .linktorootdir ; cd ..");
}

int testandinstall(char* path)
{
  FILE *dir;
  char *ownpath;
  ownpath=malloc(strlen(path)+1);
  strcpy(ownpath,path);
  if (ownpath[0]=='~')
    ownpath[0]='.';
  if ((dir=fopen(ownpath,"r"))==NULL) {
    if (strcmp("./.dfmdesk/",ownpath)==0) {
      system("mkdir .dfmdesk");
      /*system("mkdir ./.dfmdesk/Trashcan");*/
      makehomelinks();
      if (CreateProgramLink("xemacs")!=0)
	CreateProgramLink("emacs");
      CreateProgramLink("xe");
      if (CreateProgramLink("neditc")!=0)
        CreateProgramLink("nedit");
      CreateProgramLink("xvpicmaker");
      CreateProgramLink("xkill");
      CreateProgramLink("exm");
      CreateProgramLink("xv");
      CreateProgramLink("winterm");
      CreateProgramLink("xterm");
      CreateProgramLink("xedit");
      CreateProgramLink("netscape");
      CreateProgramLink("lyx");
      free(ownpath);
      return True;
    } else {
      free(ownpath);     
      return False;
    }
  } else {
    fclose(dir);
    free(ownpath);
    return True;
  }
} 



