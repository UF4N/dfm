/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "filemanager.h"
#include "windowname.h"
#include "shell.h"
#include "shellgui.h"
#include "global.h"
#include "iwc.h"
#include "timer.h"

int NumberOfCmd(char* start)
{
  int i,j;
  j=0;
  for (i=0;i<strlen(start);i++)
    if (start[i]=='&')
      j++;
  j++;
  return j;
}

char* ExtractCmd(char* start,int shnr)
{
  char* back;
  int i,j,z;
  back=malloc(strlen(start)+1);
  i=0;j=0;z=0;
  do {
    back[j]=start[i];
    i++;
    j++;
    if (start[i-1]=='&') {
      back[j-1]=0;
      j=0;
      z++;
    }
  } while (shnr>=z && start[i-1]!=0);
  return(back);
}
    
char* CutHomeDir(char* file)
{
  char *text;
  if (strncmp(file,(char*)getenv("HOME"),strlen((char*)getenv("HOME")))==0) {
    if (file[strlen((char*)getenv("HOME"))]=='/') {
      text=malloc(strlen(file)+1);
      strcpy(text,"~");
      strcat(text,file+strlen((char*)getenv("HOME")));
    } else if (file[strlen((char*)getenv("HOME"))]==0)
      text=strdup("~/");
    else 
      text=strdup(file);
  } else
    text=strdup(file);
  return(text);
}

char* ExpandTilde(char* part)
{
  char *text;
  if (strlen(part)>1 && part[0]=='~' && part[1]=='/') {
    text=malloc(strlen(part)+strlen((char*)getenv("HOME"))+1);
    strcpy(text,(char*)getenv("HOME"));
    strcat(text,part+1);
  } else
    text=strdup(part);
  return(text);
}

int usershellsystem(char* command)
{
  char *text;
  char **argv;
  int ret;
  int end;
  int spaces;
  int i,j,k;
  int shell;

  StopAlarm();  
  i=0;
  shell=False;
  while (command[i]!=0) {
    if (command[i]=='\\' && command[i+1]!=0)
      i++;
    else 
      if (command[i]=='|' ||
	  command[i]=='\"' ||
	  command[i]=='>' ||
	  command[i]=='<' ||
	  command[i]=='*' ||
	  command[i]==';')
	shell=True;
    i++;
  }
  if (shell==True) {
    argv=malloc(4*sizeof(char*));
    argv[0]=strdup((char*)getenv("SHELL"));
    argv[1]=strdup("-c");
    argv[2]=strdup(command);
    argv[3]=NULL;
    j=0;
  } else {
    argv=malloc(2*sizeof(char*));
    argv[0]=malloc(strlen(command)+1);
    end=False;i=0;j=0;k=0;
    spaces=True;
    do {
      if (command[j]=='\\') {
	j++;
	argv[i][k]=command[j];
	k++;
	j++;
	spaces=False;
      } else if (command[j]==' ' && spaces==False) {
	argv[i][k]=0;
	i++;
	j++;
	argv=realloc(argv,sizeof(char*)*(i+2));
	argv[i]=malloc(strlen(command)+1);
	k=0;
	spaces=True;
      } else if (command[j]==' ' && spaces==True) {
	j++;
      } else if (command[j]=='&' || command[j]==0) {
	end=True;
	argv[i][k]=0;
	if (k==0) {
	  free(argv[i]);
	  argv[i]=NULL;
	}
      } else {
	argv[i][k]=command[j];
	k++;
	j++;
	spaces=False;
      }
    } while (end==False);
    argv[i+1]=NULL;
    
    i=0;
    while (argv[i]!=NULL) { 
      text=argv[i];
      argv[i]=ExpandTilde(text);
      free(text);
      i++;
    }
  }
  if (command[j]=='&')
    ret=ShellWindowDoIt(argv,True);
  else
    ret=ShellWindowDoIt(argv,False);
  i=0;
  while (argv[i]!=NULL) { 
    free(argv[i]);
    i++;
  }
  free(argv);
  ReactivateAlarm();
  return(ret);
}


void ExecuteShellWithFollowLink(char *filename,char *start,char *files,int shnr)
{
  char *text;
  text=malloc(strlen(filename)+1);
  strcpy(text,filename);
  GetLinkName(&text);
  ExecuteShellcommand(text,start,files,shnr);
  free(text);
}


void ExecuteShellcommand(char *fil,char *start,char *files,int shnr)
{
  char *shelltext;
  char *start2;
  char *filename;
  int i,j,exe,ext;
  start2=ExtractCmd(start,shnr);
  if (IsSubDesktop()==True && GetKindOfWindow()==NORMAL) {
    if (strncmp(start2,"dfm !0!",strlen("dfm !0!"))==0) {
      filename=CutHomeDir(fil);
      SetWindowSaveStatus(filename,True);
      free(filename);
    }
  }
  filename=GenerateSlashName(fil);
  if (strlen(start2)>=3) {
    if (files!=NULL) 
      shelltext=malloc((strlen(filename)+strlen(start2)
			+strlen(files)+5)*sizeof(char));
    else
      shelltext=malloc((strlen(filename)+strlen(start2)
			+5)*sizeof(char));      
    j=0;exe=0;ext=0;
    for (i=0;i<strlen(start2)-2;i++) {
      if (start2[i]=='!' &&
	  start2[i+1]=='0' &&
	  start2[i+2]=='!' &&
	  exe==0) {
	strcpy(shelltext+j,filename);
	exe++;
	i=i+2;
	j=j+strlen(filename);
      } else {
	if (start2[i]=='!' &&
	    start2[i+1]=='1' &&
	    start2[i+2]=='!' && 
	    ext==0) {
	  if (files!=NULL){
	    strcpy(shelltext+j,files);
	    j=j+strlen(files);
	  }
	  ext++;
	  i=i+2;
	} else {
	  shelltext[j]=start2[i];
	  j++;
	}
      }
    }
    while (start2[i]!=0) {
      shelltext[j]=start2[i];
      j++;i++;
    }
    if (ext==0) {
      if (files!=NULL){      
	shelltext[j]=' ';j++;
	strcpy(shelltext+j,files);
	j=j+strlen(files);
      }
    }
    shelltext[j]=' ';j++;
    shelltext[j]='&';j++;
    shelltext[j]= 0 ;j++;
    if (exe>0) {
      usershellsystem(shelltext);
    }
  free(shelltext);
  }
  free(start2);
  free(filename);
}
