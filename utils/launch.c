/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fnmatch.h>

#ifndef True
#define True 1
#endif

#ifndef False
#define False 0
#endif

int usershellsystem(char* command)
{
  char *text;
  int ret;
  text=malloc(strlen("$SHELL -c X X ")+strlen(command)+1);
  strcpy(text,"$SHELL -c \'");
  strcat(text,command);
  strcat(text,"\'");
  ret=system(text);
  free(text);
  return(ret);
}

void ExecuteShellcommand(char *filename,char *start,char *files)
{
  char *shelltext;
  int i,j,exe,ext;
  if (strlen(start)>=3) {
    if (files!=NULL) 
      shelltext=malloc((strlen(filename)+strlen(start)
			+strlen(files)+5)*sizeof(char));
    else
      shelltext=malloc((strlen(filename)+strlen(start)
			+5)*sizeof(char));      
    j=0;exe=0;ext=0;
    for (i=0;i<strlen(start)-2;i++) {
      if (start[i]=='!' &&
	  start[i+1]=='0' &&
	  start[i+2]=='!' &&
	  exe==0) {
	strcpy(shelltext+j,filename);
	exe++;
	i=i+2;
	j=j+strlen(filename);
      } else {
	if (start[i]=='!' &&
	    start[i+1]=='1' &&
	    start[i+2]=='!' && 
	    ext==0) {
	  if (files!=NULL){
	    strcpy(shelltext+j,files);
	    j=j+strlen(files);
	  }
	  ext++;
	  i=i+2;
	} else {
	  shelltext[j]=start[i];
	  j++;
	}
      }
    }
    while (start[i]!=0) {
      shelltext[j]=start[i];
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
}

int main(int argc,char** argv)
{
  FILE *pfile;
  FILE *pprg;
  char** programerg;
  char** programnames;
  char *give;
  char get[1024];
  int i,d,z;
  int counterprg;
  int found;
  int argcount;
  char* arguments;
  give=malloc(strlen((char*)getenv("HOME"))+strlen(".launch")+1);
  strcpy(give,(char*)getenv("HOME"));
  strcat(give,"/.launch");
  if ((pfile=fopen(give,"r"))==NULL) {
    if ((pfile=fopen(give,"w"))==NULL) { 
      printf("launch: failed to write a new \"~/.launch\" file");
      exit(0);
    } else {
      fprintf(pfile,"ldd;file\n");
      fprintf(pfile,"*libX11*;*;!0!\n");
      fprintf(pfile,"*;*wish*;!0!\n");
      fprintf(pfile,"*;*Windows*;wine !0!\n");
      fprintf(pfile,"*;*MS-DOS executable*;echo \"How to start this DOS-program ? :\" !0!\n");
      fprintf(pfile,"*;*shell*;xterm -e !0!\n");
      fprintf(pfile,"*;*executable*;xterm -e !0!\n");
      fprintf(pfile,"*;*;xedit !0!\n");
    }
  }
  fclose(pfile);
  if (argc==1) {
    printf("Usage: %s <file> [ <arguments> ... ]\n",argv[0]);
    printf("see \"~/.launch\" (you may add more testprograms)\n");
    exit(0);
  }
  pfile=fopen(give,"r");
  free(give);
  programnames=NULL;
  counterprg=0;
  do {
    programnames=realloc(programnames,sizeof(char*)*counterprg+1);
    i=0;
    do {
      d=fgetc(pfile);
      if ((char)d!=';' && d!=10) {
	get[i]=(char)d;
	i++;
      }
    } while ((char)d!=';' && d!=10);
    get[i]=0;
    programnames[counterprg]=malloc(strlen(get));
    strcpy(programnames[counterprg],get);
    counterprg++;
  } while ((char)d==';');
  programerg=malloc(sizeof(char*)*counterprg);
  for (z=0;z<counterprg;z++) {
    give=malloc(strlen(programnames[z])+1+strlen(argv[1]));
    strcpy(give,programnames[z]);
    strcat(give," ");
    strcat(give,argv[1]);
    if ((pprg=popen(give,"r"))==NULL)
      get[0]=0;
    else {
      i=0;
      do {
	d=fgetc(pprg);
	if (d!=EOF) {
	    get[i]=(char)d;
	    i++;
	}
      } while (d!=EOF);
      get[i]=0;
      pclose(pprg);
    }
    free(give);
    programerg[z]=malloc(strlen(get)+1);
    strcpy(programerg[z],get);
  }
  do {
    z=0;
    found=True;
    do {
      i=0;
      do {
	d=fgetc(pfile);
	if (d!=EOF && d!=';') {
	  get[i]=(char)d;
	  i++;
	}
      } while (d!=EOF && d!=';');
      get[i]=0;
      if (fnmatch(get,programerg[z],FNM_NOESCAPE)==FNM_NOMATCH)
	found=False;
      if (found==False) {
	do {
	  d=fgetc(pfile);
	} while (d!=EOF && d!=10);
      }
      z++;
    } while (z<counterprg && found==True);
    if (found==True) {
      i=0;
      do {
	d=fgetc(pfile);
	if (d!=EOF && d!=10) {
	  get[i]=(char)d;
	  i++;
	}
      } while (d!=EOF && d!=10);
      get[i]=0;
      i=0;
      argcount=0;
      for (i=2;i<argc;i++)
	argcount=argcount+strlen(argv[i])+1;
      arguments=malloc(argcount+1);
      arguments[0]=0;
      for (i=2;i<argc;i++) {
	strcat(arguments,argv[i]);
	strcat(arguments," ");
      }
      ExecuteShellcommand(argv[1],get,arguments);
    }    
  } while(d!=EOF && found==False);
  exit(0);
}
