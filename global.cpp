/****************************************/
/*        global.c                      */
/****************************************/

#include "sys.h"

TTask TaskQueue[MAX_TASK];
TPriorityQueue PriorityQueues[MAX_PRIORITY];
TResource ResourceQueue[MAX_RES];
TEvent EventQueue[MAX_EVENTS];

int RunningTask;

int FreeTask;

int FreeResource;
