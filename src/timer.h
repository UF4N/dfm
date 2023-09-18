/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#ifndef TIMER_H
#define TIMER_H

void destroy();

void updatetimer();

void repeat();

void StopAlarm();

void ReactivateAlarm();

void SetDestroy(int sec);

void StopDestroy();
/*
void WaitMs(int ms);

int WaitExp();
*/
#endif
