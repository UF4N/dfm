/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef POSITIONARRANGE_H
#define POSITIONARRANGE_H

#include "filemanager.h"

void SetNextPosition(int x,int y);
void ResetPositionHistory();
void ResetPosition();
void PositionArrange(struct item *direntry,int sx,int sy);

#endif
