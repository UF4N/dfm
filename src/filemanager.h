/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <sys/stat.h>
#include <unistd.h>
#include <Xlib.h>
#include <Xutil.h>

#define NO_ERROR 0
#define REPORT_ERROR -1
#define DONT_REPORT_ERROR -2
#define BREAK_ERROR -3

struct item {
  char *filename;
  struct stat info;
  int type; /* 0 file 1 folder 2 linkfile 3 linkfolder */
  int mark; 
  struct item *NextItem;
  Window Iconwindow;
};

struct tree {
  struct item *ThisItem;
  struct tree *subtree;
  struct tree *nexttree;
  int initalized;/*dotree was called with this argument*/
  int expanded;
  int abletoexpand; /*if subtree=NULL then False*/
  Window Expandbutton;
  Window Expandline;
};

char* GenerateLastName(char* pathname);

char* GetPathName(char* pathname);

struct item *GetDirectory(char *filepath);

struct tree *GetFirstTree();

void DoTree(struct tree *treeinfo);

void GetLinkNameWithoutLast(char **text);

void GetLinkName(char **text);

int CopyFile(char *filename,char *goal,char *subfolder,int move);

int FileExist(char *filename);

int CreateProgramLink(char *programname);

int CreateFolder(char *foldername);

int CreateFile(char *filename);

int RealCopyFile(char *filename,char *goal,char *subfolder);

int LinkFile(char *filename,char *subfolder);

int EraseFile(char *filename);

#endif
