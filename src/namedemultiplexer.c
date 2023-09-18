/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "namedemultiplexer.h"
#include "gui.h"
#include "shellgui.h"
#include "filemanager.h"
#include "iconmanager.h"
#include "windowname.h"
#include "global.h"
#include <malloc.h>
#include <string.h>

/*subfolder="" */

int CopyCall(int x,int y,char *filenames,char *subfolder,int kind,int move)
{
  int i=1;
  int j;
  int k,l;
  int answer=YES;
  char *text;
  char *text2;
  char *text3;
  char *rename;
  int statusb=0;
  int status=0;
  char *goal,*source;
  ShellWindowInit();
  do {
    j=i;
    do {
      if (filenames[j]=='\\') j++;
      j++;
    } while (filenames[j]!=' ' && filenames[j]!=0);
    text3=malloc(j-i+1);
    strncpy(text3,filenames+i,j-i);
    text3[j-i]=0;
    
    text=DeSlashName(text3);
    ShellWindowInfo(text);
    k=0;l=0;
    while (text[k+1]!=0){
      if (text[k]=='/')
	l=k;
      k++;
    }
    text2=malloc(strlen(text+l)+strlen(GetPath())+strlen(subfolder)+1);
    strcpy(text2,GetPath());
    strcat(text2,subfolder);
    strcat(text2,text+l+1);
    if (FileExist(text2)==True) {
      if (answer!=ALL) {
	do {
	  if (strcmp(text2,text)==0) {
	    answer=CANCEL;
	    rename=malloc(1);
	  } else
	    answer=OverwriteQuestion(text2,&rename);
	  if (answer==RENAME) {
	    if (text2[strlen(text2)-1]=='/') {
	      free(text2);
	      text2=malloc(strlen(rename)+strlen(GetPath())+strlen(subfolder)+2);
	      strcpy(text2,GetPath());
	      strcat(text2,subfolder);
	      strcat(text2,rename);
	      strcat(text2,"/");
	    } else {
	      free(text2);
	      text2=malloc(strlen(rename)+strlen(GetPath())+strlen(subfolder)+1);
	      strcpy(text2,GetPath());
	      strcat(text2,subfolder);
	      strcat(text2,rename);
	    }
	  }
	  free(rename);
	} while(answer==RENAME && FileExist(text2)==True);
      }
      if (answer==YES || answer==ALL) {
	EraseFile(text2);
	if (subfolder[0]==0)
	  EraseIcon(text2);
      } else {
	status=DONT_REPORT_ERROR;
      }
    } else {
      if (answer!=ALL)
	answer=YES;
    }
    if (answer==YES || answer==ALL || answer==RENAME) {
      if (kind==NORMAL)
	statusb=CopyFile(text,text2,subfolder,move);
      else
	statusb=RealCopyFile(text,text2,subfolder);
      if (statusb==BREAK_ERROR)
	status=statusb;
      if (status==NO_ERROR && (statusb==REPORT_ERROR || statusb==DONT_REPORT_ERROR))
	status=statusb;
      if (status==REPORT_ERROR && statusb==DONT_REPORT_ERROR)
	status=statusb;
      if (status==NO_ERROR && subfolder[0]==0) {
	goal=DeSlashName(text2);
	source=DeSlashName(text);
	NewIcon(goal,source,x,y,i);
	free(source);
	free(goal);
      }
    }
    i=j+1;
    free(text);
    free(text2);
    free(text3);
    if (ShellWindowUpdate()==True)
      status=BREAK_ERROR;
  } while (filenames[j]!=0 && answer!=CANCEL && status!=BREAK_ERROR);
  ShellWindowDestroy();
  UpdateIcons(True);
  return(status);
}

void EraseCall(char *filenames,int ask,int remote)
{
  int i=1;
  int j;
  char *text;
  char *textf;
  int status=NO_ERROR;
  int answer=YES;
  ShellWindowInit();
  do {
    j=i;
    do {
      if (filenames[j]=='\\') j++;
      j++;
    } while (filenames[j]!=' ' && filenames[j]!=0);
    text=malloc(j-i+1);
    strncpy(text,filenames+i,j-i);
    text[j-i]=0;
    textf=DeSlashName(text);
    /*ShellWindowInfo(text);*/
    if (ask==True && answer!=ALL) {
      if (remote==True)
	answer=EraseQuestionOnDifferent(textf);
      else
	answer=EraseQuestion(textf);
    }
    free(textf);
    if (ask==False || answer==ALL || answer==YES)
      status=EraseFile(text);
    if (GetKindOfWindow()==NORMAL)
      UpdateIcons(True);
    free(text);
    i=j+1;
  } while (filenames[j]!=0 && answer!=CANCEL && status!=BREAK_ERROR);
  ShellWindowDestroy();
  UpdateIcons(True);
}

void LinkCall(int x,int y,char *filenames,char *subfolder)
{
  int i=1;
  int j;
  char *text;
  char *text2;
  int status=NO_ERROR;
  ShellWindowInit();
  do {
    j=i;
    do {
      if (filenames[j]=='\\') j++;
      j++;
    } while (filenames[j]!=' ' && filenames[j]!=0);
    text2=malloc(j-i+1);
    strncpy(text2,filenames+i,j-i);
    text2[j-i]=0;
    text=DeSlashName(text2);
    /*ShellWindowInfo(text);*/
    status=LinkFile(text,subfolder);
    if (status==0 && subfolder[0]==0)
      NewIcon(text,text,x,y,i);
    free(text);
    free(text2);
    i=j+1;
  } while (filenames[j]!=0 && status!=BREAK_ERROR);
  ShellWindowDestroy();
  UpdateIcons(True);
}
