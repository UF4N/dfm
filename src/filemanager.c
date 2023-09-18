/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "filemanager.h"
#include "windowname.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "shell.h"
#include <malloc.h>
#include "global.h"
#include <stdlib.h>
#include <stdio.h>

char* GenerateLastName(char* pathname)
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
    if (text[j-i-1]=='/') 
      text[j-i-1]=0;
    else
      text[j-i]=0;
  }
  iconname=malloc(strlen(text)+1);
  strcpy(iconname,text);
  free(text);
  return(iconname);
}

char* GetPathName(char* pathname)
{
  char *text;
  int i,j;
  text=malloc(strlen(pathname)+1);
  strcpy(text,pathname);
  j=strlen(text)-1;
  for (i=0;i<strlen(text)-1;i++)
    if (text[i]=='/')
      j=i;
  text[j+1]=0;
  return(text);
}

struct item *GetDirectory(char *filepath)
{
  struct item *firstfile=NULL;
  DIR *directory;
  struct item *issaved=NULL;
  struct item *isnew;
  char *pathsaved,*fullname;
  struct dirent *position;
  struct stat info2;

  pathsaved=malloc(strlen(filepath)+1);
  strcpy(pathsaved,filepath);
  if (pathsaved[0]=='~' && pathsaved[1]=='/')
    pathsaved[0]='.';
  if ((directory=opendir(pathsaved))!=NULL) {
    while((position=readdir(directory))!=NULL) {
      if (strcmp(position->d_name,"..")!=0 && 
	  strcmp(position->d_name,".xvpics")!=0 && 
	  strcmp(position->d_name,".")!=0 &&
	  (GetShowHidden()==True || 
	   (position->d_name)[0]!='.' ||
           GetIsDesktop()==True ||
           GetKindOfWindow()!=NORMAL
	  )
	 ) {
	isnew=malloc(sizeof(struct item));
	if (firstfile==NULL) 
	  firstfile=isnew;
	else
	  issaved->NextItem=isnew;
	issaved=isnew;
	isnew->filename=malloc(strlen(position->d_name)+2);
	strcpy(isnew->filename,position->d_name);
	fullname=malloc(strlen(position->d_name)+strlen(pathsaved)+1);
	strcpy(fullname,pathsaved);
	strcat(fullname,position->d_name);
	isnew->NextItem=NULL;
	isnew->mark=False;
	isnew->type=0;
	stat(fullname,&info2);
	if ((info2.st_mode & S_IFMT) == S_IFDIR) {
	  isnew->type++;
	  strcat(isnew->filename,"/");
	}
	lstat(fullname,&(isnew->info));
	if ((isnew->info.st_mode & S_IFMT) == S_IFLNK)
	  isnew->type=isnew->type+2;
	isnew->info.st_mode=info2.st_mode;
	free(fullname);
      }
    }
    closedir(directory);
  } else {
    XCloseDisplay(GetDisplay());
    exit(0);
  }
  free(pathsaved);
  return(firstfile);
}

struct tree *GetFirstTree()
{
  struct tree *newtree;
  char *pathsaved;
  DIR *directory;
  pathsaved=strdup(GetPath());
  if (pathsaved[0]=='~' && pathsaved[1]=='/')
    pathsaved[0]='.';
  if ((directory=opendir(pathsaved))==NULL) {
    XCloseDisplay(GetDisplay());
    exit(0);
  }
  closedir(directory);
  free(pathsaved);
  newtree=malloc(sizeof(struct tree));
  newtree->initalized=False;
  newtree->subtree=NULL;
  newtree->nexttree=NULL;
  newtree->ThisItem=malloc(sizeof(struct item));
  newtree->ThisItem->filename=malloc(1);
  newtree->ThisItem->type=1;
  newtree->ThisItem->NextItem=NULL;
  newtree->ThisItem->mark=False;
  (newtree->ThisItem->filename)[0]=0;
  DoTree(newtree);
  return(newtree);
}

int sortname(const void *a,const void *b)
{
  return(strcmp(*((char**)b),*((char**)a) ) );
}

void DoTree(struct tree *treeinfo)
{
  char *pathsaved,*fullname;
  struct tree *oldtree=NULL,*newtree;
  DIR *directory;
  struct dirent *position;
  struct stat info2;
  char **filenames=NULL;
  int i,j;
  pathsaved=malloc(strlen(GetPath())+strlen(treeinfo->ThisItem->filename)+1);
  strcpy(pathsaved,GetPath());
  strcat(pathsaved,treeinfo->ThisItem->filename);
  if (pathsaved[0]=='~' && pathsaved[1]=='/')
    pathsaved[0]='.';
  if ((directory=opendir(pathsaved))!=NULL) {
    i=0;
    while((position=readdir(directory))!=NULL) {
      if (strcmp(position->d_name,"..")!=0 && 
	  strcmp(position->d_name,".xvpics")!=0 && 
	  strcmp(position->d_name,".")!=0) {
	fullname=malloc(strlen(position->d_name)+strlen(pathsaved)+1);
	strcpy(fullname,pathsaved);
	strcat(fullname,position->d_name);
	stat(fullname,&info2);
	if ((info2.st_mode & S_IFMT) == S_IFDIR) {
	  i++;
	  filenames=realloc(filenames,i*sizeof(char*));
	  filenames[i-1]=malloc(strlen(position->d_name)+1);
	  strcpy(filenames[i-1],position->d_name);
	}
	free(fullname);
      }
    }
    closedir(directory);
    if (i>0) {
      qsort(filenames,i,sizeof(char*),sortname);    
      for (j=0;j<i;j++) {
	fullname=malloc(strlen(filenames[j])+strlen(pathsaved)+1);
	strcpy(fullname,pathsaved);
	strcat(fullname,filenames[j]);
	stat(fullname,&info2);
	if ((info2.st_mode & S_IFMT) == S_IFDIR) {
	  newtree=malloc(sizeof(struct tree));
	  newtree->nexttree=oldtree;
	  oldtree=newtree;
	  newtree->subtree=NULL;
	  newtree->initalized=False;
	  newtree->ThisItem=malloc(sizeof(struct item));
	  newtree->ThisItem->filename=malloc(strlen(treeinfo->ThisItem->filename)+
					     strlen(filenames[j])+2);
	  strcpy(newtree->ThisItem->filename,treeinfo->ThisItem->filename);
	  strcat(newtree->ThisItem->filename,filenames[j]);
	  strcat(newtree->ThisItem->filename,"/");
	  newtree->ThisItem->type=1;
	  newtree->ThisItem->mark=False;
	  lstat(fullname,&(newtree->ThisItem->info));
	  if ((newtree->ThisItem->info.st_mode & S_IFMT) == S_IFLNK)
	    newtree->ThisItem->type=newtree->ThisItem->type+2;
	  newtree->ThisItem->info.st_mode=info2.st_mode;
	}
	free(fullname);
	free(filenames[j]);
      }
      free(filenames);
    }
  }
  treeinfo->initalized=True;
  treeinfo->subtree=oldtree;
  treeinfo->expanded=False;
  if (treeinfo->subtree==NULL)
    treeinfo->abletoexpand=False;
  else
    treeinfo->abletoexpand=True;    
}

void GetLinkNameWithoutLast(char **text)
{
  char *text2,*text3;
  int i,tobezero;
  if (strcmp(*text,"/")!=0) {
    text2=malloc(strlen(*text)+1);
    strcpy(text2,*text);
    if (text2[strlen(text2)-1]=='/')
      text2[strlen(text2)-1]=0;
    tobezero=0;
    for (i=0;i<strlen(text2);i++)
      if (text2[i]=='/')
	tobezero=i;
    text2[tobezero+1]=0;
    GetLinkName(&text2);
    text3=malloc(strlen(text2)+strlen(*text+tobezero+1)+1);
    strcpy(text3,text2);
    strcat(text3,*text+tobezero+1);
    free(*text);
    *text=text3;
    free(text2);
  }
}

void GetLinkName(char **text) /* buggy? (doesn't work with malloc) */
{
  char text2[1024];
  char text3[1024];
  char a[1024];
  char *li;
  int i,last,status;

  li=GenerateLastName(*text);
  if (strncmp(*text,"~/.dfmdesk/",strlen("~/.dfmdesk/"))==0) {
    if (strcmp(li,".linktohomedir")==0) {
      free(*text);
      *text=strdup(getenv("HOME"));
    } else if (strcmp(li,".linktohomedir")==0) {
      free(*text);
      *text=strdup("/");
    } else {
      strcpy(text2,*text);
      if (text2[0]=='~' && text2[1]=='/')
        text2[0]='.';
      text3[0]=0;
      last=0;
      for (i=0;i<=strlen(text2);i++) {
        if (text2[i]=='/' || text2[i]==0) {
	  strncat(text3,text2+last,i-last);
	  last=i;
	  a[0]=0;
	  status=readlink(text3,a,1023);
	  if( status!=-1 && a[0]=='/') {
	    a[status]=0;
	    if (a[strlen(a)-1]=='/')
	      a[strlen(a)-1]=0;
	    strcpy(text3,a);
	  }
        }
      }
      if (text3[0]=='.' && text3[1]=='/')
        text3[0]='~';
      free(*text);
      *text=malloc(strlen(text3)+1);
      strcpy(*text,text3);
    }
  }
  free(li);
}

/*thanks to Anuj Dev and Henning Ehlermann*/
int CopyFile(char *filename,char *goal,char *subfolder,int move)
{
  int status;
  char *text;
  char *text2;
  char *textf;
  char *textp;
  char *texta;
  char *addfolder;
  int changename;
  text=GenerateLastName(filename);
  text2=GenerateLastName(goal);
  if (strcmp(text,text2)!=0) {
    addfolder=malloc(strlen(subfolder)+strlen("_dfmname")+1);
    strcpy(addfolder,subfolder);
    strcat(addfolder,"_dfmname");    
    if (CreateFolder(addfolder)!=0) {
      free(addfolder);
      free(text);
      free(text2);
      return(-1);
    }
    changename=True;
  } else {
    addfolder=malloc(strlen(subfolder)+1);
    strcpy(addfolder,subfolder);
    changename=False;
  }
  free(text);
  free(text2);
  /*****************/
  textf=GenerateLastName(filename);
  textp=strdup(filename);
  texta=malloc(strlen(addfolder)+strlen(GetPath())+strlen(textf)+2);
  strcpy(texta,GetPath());
  strcat(texta,addfolder);
  if (texta[strlen(texta)-1]=='/')
    texta[strlen(texta)-1]=0;    
  strcat(texta,"/");
  strcat(texta,textf);
  if (textp[0]=='~' && textp[1]=='/')
    textp[0]='.';
  if (texta[0]=='~' && texta[1]=='/')
    texta[0]='.';
  if (textp[strlen(textp)-1]=='/')
    textp[strlen(textp)-1]=0;
  status=rename(textp,texta);
  if (errno==EINVAL) {
    free(texta);
    free(textp);
    free(textf);
    textp=GenerateSlashName(GetPath());
    texta=GenerateSlashName(addfolder);
    textf=GenerateSlashName(filename);
    status=DONT_REPORT_ERROR;
  } else {
    if (move==False && status==0)
      rename(texta,textp);
    free(texta);
    free(textp);
    free(textf);
    textp=GenerateSlashName(GetPath());
    texta=GenerateSlashName(addfolder);
    textf=GenerateSlashName(filename);
    if (status!=0 || move==False) {
#ifdef LINUX
      text=malloc(strlen("cp -rpdf ")+strlen(textf)+strlen(textp)+
		  strlen(texta)+2);
      strcpy(text,"cp -rpdf ");
      strcat(text,textf);
      strcat(text," ");
      strcat(text,textp);
      strcat(text,texta);
      status=usershellsystem(text);
      free(text);
#elif defined(IRIX)     
      text=malloc(strlen("cp -Rf ")+strlen(textf)+strlen(textp)+
		  strlen(texta)+2);
      strcpy(text,"cp -Rf ");
      strcat(text,textf);
      strcat(text," ");
      strcat(text,textp);
      strcat(text,texta);
      status=usershellsystem(text);
      free(text);
#else
      text=malloc(strlen("(cd X; tar cf - X| (cd X; tar xf - ))")+2*strlen(textf)+strlen(textp)+
		  strlen(texta)+2);
      strcpy(text,"(cd ");
      text2=GetPathName(textf);
      strcat(text,text2);
      free(text2);
      strcat(text," ; tar cf - ");
      text2=GenerateLastName(textf);
      strcat(text,text2);
      free(text2);
      strcat(text," | (cd ");
      strcat(text,textp);  
      strcat(text,texta);
      strcat(text," ; tar xf - ))");
      status=usershellsystem(text);
      free(text);
#endif
    }
  }
  if (changename==True) {
    text2=GenerateLastName(filename);
    text=malloc(strlen(GetPath())+strlen(addfolder)+strlen(text2)+2);
    strcpy(text,GetPath());
    strcat(text,addfolder);
    strcat(text,"/");
    strcat(text,text2);
    free(text2);
    text2=malloc(strlen(goal)+1);
    strcpy(text2,goal);
    if (text[0]=='~' && text[1]=='/')
      text[0]='.';
    if (text2[0]=='~' && text2[1]=='/')
      text2[0]='.';
    if (text[strlen(text)-1]=='/')
      text[strlen(text)-1]=0;
    if (text2[strlen(text2)-1]=='/')
      text2[strlen(text2)-1]=0;
    rename(text,text2);
    free(text);
    free(text2);
    text=malloc(strlen(textp)+strlen(texta)+1);
    strcpy(text,textp);
    strcat(text,texta);    
    EraseFile(text);
    free(text);
  }
  free(addfolder);
  free(textp);
  free(texta);
  free(textf);
  return(status);
}

int CreateProgramLink(char *programname)
{
  char goal[256];
  char* text;
  char* text2;
  int c,i,j,status;
  FILE *pprg;
  text=malloc(strlen("\\type ")+strlen(programname)+1);
  strcpy(text,"\\type ");
  strcat(text,programname);
  i=0;
  goal[0]=0;
  if ((pprg=popen(text,"r"))!=NULL) {
    while((c=fgetc(pprg))!=EOF) {
      sprintf(goal+i,"%c",c);
      i++;
    }
    pclose(pprg);
    c=strlen(goal);
    for (j=0;j<i-4;j++) {
      if (strncmp(goal+j," is ",4)==0)
        c=j+4;
    }
    text2=malloc(strlen(goal)+1);
    strcpy(text2,goal+c);
    strcpy(goal,text2);
    free(text2);
  }
  free(text);
  if (strlen(goal)>0) {
    if (goal[strlen(goal)-1]==10)
      goal[strlen(goal)-1]=0;
    text=malloc(strlen("\\ln -s  ~/.dfmdesk")+strlen(goal)+1);
    strcpy(text,"\\ln -s ");
    strcat(text,goal);
    strcat(text," ~/.dfmdesk");
    status=usershellsystem(text);
    free(text);
    if (status==0)
      return(NO_ERROR);
    else
      return(REPORT_ERROR);
  } else
    return(REPORT_ERROR);
}

int CreateFolder(char *foldername)
{
  int status;
  char *text;
  char *textp;
  char *textf;
  textp=GenerateSlashName(GetPath());
  textf=GenerateSlashName(foldername);
  text=malloc(strlen("\\mkdir ")+strlen(textf)+strlen(textp)+2);
  strcpy(text,"\\mkdir ");
  strcat(text,textp);
  strcat(text,textf);
  status=usershellsystem(text);
  free(text);
  free(textp);
  free(textf);
  if (status==0)
    return(NO_ERROR);
  else
    return(REPORT_ERROR);
}

int CreateFile(char *filename)
{
  int status;
  char *text;
  char *textp;
  char *textf;
  textp=GenerateSlashName(GetPath());
  textf=GenerateSlashName(filename);
  text=malloc(strlen("\\touch ")+strlen(textf)+strlen(textp)+2);
  strcpy(text,"\\touch ");
  strcat(text,textp);
  strcat(text,textf);
  status=usershellsystem(text);
  free(text);
  free(textp);
  free(textf);
  if (status==0)
    return(NO_ERROR);
  else
    return(REPORT_ERROR);
}

int FileExist(char *filename)
{
  FILE *testicon=NULL;
  char *text;
  text=malloc(strlen(filename)+1);
  strcpy(text,filename);
  if (text[0]=='~' && text[1]=='/')
    text[0]='.';
  if (text[strlen(text)-1]=='/')
    text[strlen(text)-1]=0;
  if ((testicon=fopen(text, "r")) == NULL) {
    free(text);
    return(False);
  } else {
    free(text);
    fclose(testicon);
    return(True);
  }
}

int RealCopyFile(char *filename,char *goal,char *subfolder)
{
  int status;
  char *text;
  char *text2;
  char *textp;
  char *texta;
  char *textf;
  char *addfolder;
  int changename;
  text=GenerateLastName(filename);
  text2=GenerateLastName(goal);
  if (strcmp(text,text2)!=0) {
    addfolder=malloc(strlen(subfolder)+strlen("_dfmname")+1);
    strcpy(addfolder,subfolder);
    strcat(addfolder,"_dfmname");    
    if (CreateFolder(addfolder)!=0) {
      free(addfolder);
      free(text);
      free(text2);
      return(-1);
    }
    changename=True;
  } else {
    addfolder=malloc(strlen(subfolder)+1);
    strcpy(addfolder,subfolder);
    changename=False;
  }
  free(text);
  free(text2);

  textf=GenerateLastName(filename);
  textp=strdup(filename);
  texta=malloc(strlen(addfolder)+strlen(GetPath())+strlen(textf)+2);
  strcpy(texta,GetPath());
  strcat(texta,addfolder);
  if (texta[strlen(texta)-1]=='/')
    texta[strlen(texta)-1]=0;    
  strcat(texta,"/");
  strcat(texta,textf);
  if (textp[0]=='~' && textp[1]=='/')
    textp[0]='.';
  if (texta[0]=='~' && texta[1]=='/')
    texta[0]='.';
  if (textp[strlen(textp)-1]=='/')
    textp[strlen(textp)-1]=0;
  status=rename(textp,texta);
  if (errno==EINVAL) {
    status=DONT_REPORT_ERROR;
    free(texta);
    free(textp);
    free(textf);
    textp=GenerateSlashName(GetPath());
    texta=GenerateSlashName(addfolder);
    textf=GenerateSlashName(filename);
  } else {
    if (status==0)
      rename(texta,textp);
    free(texta);
    free(textp);
    free(textf);
    textp=GenerateSlashName(GetPath());
    texta=GenerateSlashName(addfolder);
    textf=GenerateSlashName(filename);
    if (status!=0) {
      /********/
      text=malloc(strlen("\\cp -rf ")+strlen(textf)+strlen(textp)+
		  strlen(texta)+2);
      strcpy(text,"\\cp -rf ");
      strcat(text,textf);
      strcat(text," ");
      strcat(text,textp);
      strcat(text,texta);
      status=usershellsystem(text);
      free(text);
      /***********/
    }
  }
  if (changename==True) {
    text2=GenerateLastName(filename);
    text=malloc(strlen(GetPath())+strlen(addfolder)+strlen(text2)+2);
    strcpy(text,GetPath());
    strcat(text,addfolder);
    strcat(text,"/");
    strcat(text,text2);
    free(text2);
    text2=malloc(strlen(goal)+1);
    strcpy(text2,goal);
    if (text[0]=='~' && text[1]=='/')
      text[0]='.';
    if (text2[0]=='~' && text2[1]=='/')
      text2[0]='.';
    if (text[strlen(text)-1]=='/')
      text[strlen(text)-1]=0;
    if (text2[strlen(text2)-1]=='/')
      text2[strlen(text2)-1]=0;
    status=rename(text,text2);
    free(text);
    free(text2);
    text=malloc(strlen(textp)+strlen(texta)+1);
    strcpy(text,textp);
    strcat(text,texta);    
    EraseFile(text);
    free(text);
  }
  free(textp);
  free(texta);
  free(textf);
  free(addfolder);
  if (status==0)
    return(NO_ERROR);
  else
    return(REPORT_ERROR);
}

int LinkFile(char *filename,char *subfolder)
{
  int status;
  char *text;
  char *textp;
  char *texts;
  char *textf;
  int textf_len;
  textp=GenerateSlashName(GetPath());
  texts=GenerateSlashName(subfolder);
  textf=GenerateSlashName(filename);
  textf_len=strlen(textf);
  if (textf[textf_len-1] == '/')
      textf[textf_len-1] = 0;
  text=malloc(strlen("\\ln -s ")+textf_len+strlen(textp)+
	      strlen(texts)+2);
  strcpy(text,"\\ln -s ");
  strcat(text,textf);
  strcat(text," ");
  strcat(text,textp);
  strcat(text,texts);
  status=usershellsystem(text);
  free(textp);
  free(texts);
  free(textf);
  free(text);
  if (status==0)
    return(NO_ERROR);
  else
    return(REPORT_ERROR);
}

/*thanks to Anuj Dev*/
int EraseFile(char *filename)
{
  int status;
  char *text;
  struct stat info;
  text=malloc(strlen(filename)+1);
  strcpy(text,filename);
  if (text[0]=='~' && text[1]=='/')
    text[0]='.';
  if (text[strlen(text)-1]=='/')
    text[strlen(text)-1]=0;  
  lstat(text,&info);
  free(text);
  if ((info.st_mode&S_IFMT)==S_IFLNK) {
    status=0;
  } else {
    text=malloc(strlen("\\touch ")+strlen(filename)+1);
    strcpy(text,"\\touch ");
    strcat(text,filename);
    if (text[strlen(text)-1]=='/')
      text[strlen(text)-1]=0;  
    status=usershellsystem(text);
    free(text);
  }
  if (status==0) {
    text=malloc(strlen("\\rm -rf ")+strlen(filename)+1);
    strcpy(text,"\\rm -rf ");
    strcat(text,filename);
    if (text[strlen(text)-1]=='/')
      text[strlen(text)-1]=0;  
    status=usershellsystem(text);
    free(text);
  }
  if (status==0)
    return(NO_ERROR);
  else
    return(REPORT_ERROR);
}
