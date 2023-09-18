/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef GUI_H
#define GUI_H

#define CANCEL 1
#define YES    2
#define ALL    3
#define NO     4
#define RENAME 5

void initgui(int argc,char *argv[]);

void Info();

int Question(char* text);

int OverwriteQuestion(char *filename,char **rename);

int EraseQuestion(char *filename);
int EraseQuestionOnDifferent(char *filename);

void NewProgram();

void NewFolder();

void NewFile();

#endif
