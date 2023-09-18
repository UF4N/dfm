/**************************************************************
  (C) 1997 by Achim Kaiser

  You may distribute under the terms of the GNU General Public
  License as specified in the COPYING file.
**************************************************************/

#include "timer.h"
#include "global.h"
#include "iwc.h"
#include <unistd.h>
#include <stdlib.h>
#ifdef SUNOS
#include <signal.h>
#include <time.h>
#else
#include <sys/signal.h>
#include <sys/time.h>
#endif

struct timeval timegoal;

void destroy()
{
  XCloseDisplay(GetDisplay());
  exit(0);
}

void updatetimer()
{
  if (GetKindOfWindow()!=STRUCTURE) {
    signal(SIGALRM,updatetimer);
    alarm(GetTimer());
    SendWindow(GetMoveWindow(),UPDATE_LIGHT);
  }
}

void repeat()
{
  signal(SIGALRM,repeat);
  alarm(GetTimer());
}

void StopAlarm()
{
  alarm(0);
}

void ReactivateAlarm()
{
  signal(SIGALRM,repeat);
  alarm(GetTimer());
}

void SetDestroy(int sec)
{
  signal(SIGALRM,destroy);
  alarm(sec);
}

void StopDestroy()
{
  signal(SIGALRM,repeat);
}

/*
int comptimeval(struct timeval *time1,struct timeval *time2)
{
  if (time1->tv_sec>time2->tv_sec)
    return(1);
  else if (time1->tv_sec<time2->tv_sec)
    return(-1);
  else {
    if (time1->tv_usec>time2->tv_usec)
      return(1);
    else if (time1->tv_usec<time2->tv_usec)
      return(-1);
    else 
      return(0);
  }
}

void WaitMs(int ms)
{
  struct timeval timenow;
  div_t p;
  div_t q;

  gettimeofday(&timenow,NULL); 
  p=div(ms,1000);
  timegoal.tv_sec=timenow.tv_sec+p.quot;
  q=div(p.rem+(int)((double)timenow.tv_usec/1000),1000);
  timegoal.tv_sec=timegoal.tv_sec+q.quot;
  timegoal.tv_usec=q.rem*1000;
}

int WaitExp()
{
  struct timeval timenow;  
  gettimeofday(&timenow,NULL);     
  if (comptimeval(&timegoal,&timenow)>0)
    return False;
  else
    return True;
}
*/
