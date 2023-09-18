/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "windowname.h"
#include "global.h"
#include <string.h>
#include <stdlib.h>

char* GenerateSlashName(char* input)
{
  char *text;
  int i;
  char *insert;
  insert=strdup(" ");
  text=malloc(2*strlen(input)+2);
  text[0]=0;
  i=0;
  while (input[i]!=0) {
    if (input[i]<'+')
      strcat(text,"\\");
    else if (input[i]>'9' && input[i]<'A')
      strcat(text,"\\");
    else if (input[i]>'z' && input[i]<'~')
      strcat(text,"\\");
    else if (input[i]>'~')
      strcat(text,"\\");
    insert[0]=input[i];
    strcat(text,insert);
    i++;
  }
  free(insert);
  return(text);
}

char* DeSlashName(char* input)
{
  char *text;
  int i;
  char *insert;
  insert=strdup(" ");
  text=malloc(strlen(input)+2);
  text[0]=0;
  if (input[0]==' ')
    i=1;
  else
    i=0;
  while (input[i]!=0) {
    if (input[i]!='\\') {
      insert[0]=input[i];
      strcat(text,insert);
    }
    i++;
  }
  free(insert);
  return(text);
}

char* GenerateWindowName(char* pathname,int type)
{
  char *text;
  char *windowname;
  int i,j;
  if (strcmp(pathname,"/")==0 || strcmp(pathname,"~/")==0) {
    text=malloc(strlen(pathname)+1);
    strcpy(text,pathname);
  } else {
    i=1;j=1;
    do {
      j++;
      if (pathname[j-2]=='/')
	i=j-1;
    } while (pathname[j]!=0);
    text=malloc(j-i+1);
      strncpy(text,pathname+i,j-i);
      text[j-i-1]=0;
  }
  windowname=malloc(strlen(pathname)+strlen(text)+10);
  strcpy(windowname,text);
  free(text);
  if (type==NORMAL)
    strcat(windowname," --- dfm:");
  else if (type==DETAIL)
    strcat(windowname," -d- dfm:");
  else /*STRUCTURE*/ 
    strcat(windowname," -s- dfm:");
  strcat(windowname,pathname);
  return(windowname);
}

char* GenerateIconName(char* pathname)
{
  char *text;
  char *iconname;
  int i,j;
  if (strcmp(pathname,"/")==0 || strcmp(pathname,"~/")==0) {
    text=malloc(strlen(pathname)+1);
    strcpy(text,pathname);
  } else {
    i=1;j=1;
    do {
      j++;
      if (pathname[j-2]=='/')
	i=j-1;
    } while (pathname[j]!=0);
    text=malloc(j-i+1);
      strncpy(text,pathname+i,j-i);
      text[j-i-1]=0;
  }
  iconname=malloc(strlen(text)+1);
  strcpy(iconname,text);
  free(text);
  return(iconname);
}

char* GenerateLastNameWithSlash(char* filename)
{
  char *text;
  char *name;
  int i,j;
  i=1;j=1;
  do {
    j++;
    if (filename[j-2]=='/')
      i=j-1;
  } while (filename[j]!=0);
  text=malloc(j-i+1);
  strcpy(text,filename+i);
  
  name=malloc(strlen(text)+1);
  strcpy(name,text);
  free(text);
  return(name);
}
