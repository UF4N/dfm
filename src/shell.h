/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef SHELL_H
#define SHELL_H
 
int NumberOfCmd(char* start);

char* ExtractCmd(char* start,int shnr);

char* ExpandTilde(char* part);

int usershellsystem(char* command);

void ExecuteShellWithFollowLink(char *filename,char *start,char *files,int shnr);

void ExecuteShellcommand(char *filename,char *start,char *files,int shnr);

#endif
