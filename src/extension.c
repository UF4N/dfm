/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "extension.h"
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "fnmatch.h"


void GetExtensionParameter(char *filename,char *fullname,char **icon,char **start)
{
  FILE *pfext=NULL;
  char *text;
  char *fullname2;
  int i,character;
  int lastline=0;
  struct stat info;
  static int finden=0;
  if (filename[0]==0)
    filename="this_is_never_in_dfmext/";
  fullname2=malloc(strlen(fullname)+1);
  strcpy(fullname2,fullname);
  if (fullname2[0]=='~' && fullname2[1]=='/')
    fullname2[0]='.';

  /* search dfmext */
  if ((pfext=fopen("./.dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/etc/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/share/dfm/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/local/share/dfm/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/local/etc/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/etc/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/local/lib/dfm/dfmext", "r")) == NULL) 
  if ((pfext=fopen("/usr/local/lib/X11/dfm/dfmext", "r")) == NULL) 
       pfext=fopen("/usr/X11/lib/X11/dfm/dfmext", "r");
  /* end search */

  if (pfext==NULL) {
    if (finden==0) {
      printf("DFM Warning: .dfmext or dfmext not found!\n");
      finden=1;
    }
  } else {
    text=malloc(1000);
    do {
      i=-1;
      do {
	character=fgetc(pfext);
	i++;
	text[i]=(char)character;
      } while (character!=-1 && text[i]!=';' && character!=10);
      text[i]=0;
      if (fnmatch(text,filename,FNM_PATHNAME)==0) {
	if (strcmp(text,"*")==0) lastline=1;
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
	*icon=malloc(strlen(text)+1);
	strcpy(*icon,text);
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
	*start=malloc(strlen(text)+1);
	strcpy(*start,text);
      } else {
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
      }
    } while (*icon==NULL && *start==NULL && character!=-1);
    if (lastline==1) {
      lstat(fullname2,&info);
      if ((info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))!=0) {
	free(*icon);
	free(*start);
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
	*icon=malloc(strlen(text)+1);
	strcpy(*icon,text);
	i=-1;
	do {
	  character=fgetc(pfext);
	  i++;
	  text[i]=(char)character;
	} while (character!=-1 && text[i]!=';' && character!=10);
	text[i]=0;
	*start=malloc(strlen(text)+1);
	strcpy(*start,text);
      }
      free(text);
    }
    fclose(pfext);
  }
  if (*icon==NULL || *start==NULL) {
    if (filename[strlen(filename)-1]=='/') {
      *icon=malloc(strlen("/usr/X11/lib/X11/dfm/icons/icon_folder.xpm")+1);
      strcpy(*icon,"/usr/X11/lib/X11/dfm/icons/icon_folder.xpm");
      *start=malloc(strlen("dfm !0!")+1);
      strcpy(*start,"dfm !0!");
    } else {
      lstat(fullname2,&info);
      if ((info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))!=0) {
	*icon=malloc(strlen("/usr/X11/lib/X11/dfm/icons/icon_binary.xpm")+1);
	strcpy(*icon,"/usr/X11/lib/X11/dfm/icons/icon_binary.xpm");
	*start=malloc(strlen("!0!")+1);
	strcpy(*start,"!0!");
      } else {
	*icon=malloc(strlen("/usr/X11/lib/X11/dfm/icons/icon_file.xpm")+1);
	strcpy(*icon,"/usr/X11/lib/X11/dfm/icons/icon_file.xpm");
	*start=malloc(strlen("xedit !0!")+1);
	strcpy(*start,"xedit !0!");
      }
    }
  }
  free(fullname2);
  if ((*icon)[0]=='~' && (*icon)[1]=='/')
    (*icon)[0]='.';
}




