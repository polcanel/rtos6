/****************************************/
/*        global.c                      */
/****************************************/

#include "sys.h"

TTask TaskQueue[MAX_TASK];
PriorityQueue PriorityQueues[MAX_PRIORITY]; // Массив очередей по уровням приоритета
TSemaphore SemaphoreQueue[MAX_RES];
TEvent EventQueue[MAX_EVENTS];
int RunningTask;
int FreeTask;
int FreeResource;