/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

/* Inter-Window-Communication */

#ifndef IWC_H
#define IWC_H

#include <Xlib.h>
#include <Xutil.h>

#define SEND 10
#define CLOSE 11
#define UPDATE 12
#define UPDATE_LIGHT 13
#define RAISE 14

#define DndFile		2
#define	DndFiles	3

int IsDesktopActive();  

Window *GetWindowNamed(char *windowname);

Window GetGreatestParent(Window w);

void SendFiles(XEvent evnt,int x,int y,char *filenames);

void SendFontName(char *name);
void UpdateFontName();

void SendColors(int r1,int g1,int b1,
		int r2,int g2,int b2,
		int r3,int g3,int b3,
		int r4,int g4,int b4);
void UpdateColors();

void SetWindowCache(int status);
int GetWindowCache();

void SetMainBackground(int status);
int GetMainBackground();

void LoadFileInfoToDesktop();
void SaveFileInfoFromDesktop();

int GetIconSaveStatus(char* path);
void SetIconSaveStatus(char* path,int status);

int GetWindowSaveStatus(char* path);
void SetWindowSaveStatus(char* path,int status);

void ManageAutofocus();

void SendTimer(long timer);
void UpdateTimer();

void SetDefaultBackground(char* defback);
char* GetDefaultBackground();

void GetIconPixmap(char* iconname,Pixmap *iconpixmap,Pixmap *shapepixmap);
void SetIconPixmap(char* iconname,Pixmap iconpixmap,Pixmap shapepixmap);
void EraseIconPixmaps();

void GetFileInfoFromDesktop(char *filename,char **position,char **icon,char **start);
void SendFileInfoToDesktop(char *filename,char *position,char *icon,char *start);
void EraseFileInfoInDesktop(char *filename);
void EraseMultipleInfo(char *filenames);

void GoalWasFolder(Window Quellwindow,char *foldername,
		     char *filenames,unsigned int keys);

void SendWindow(Window w,int message);

void WorkIncomingCall(XEvent evnt);

#endif
