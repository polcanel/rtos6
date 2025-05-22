/****************************************/
/*               sys.h                   /
/****************************************/

#include "defs.h"

#define INSERT_TO_TAIL 1
#define INSERT_TO_HEAD 0

typedef struct Type_Task
{
	int ref;
	int priority;
	void (*entry)(void);
	char* name;

} TTask;

typedef struct Type_resource
{
	int task;
	int priority;
	char* name;

} TResource;

typedef struct Type_event
{
	int event_id;
	int signaled;

} TEvent;

typedef struct Type_priority_queue
{
	int head;
	int tail;

} TPriorityQueue;

extern TTask TaskQueue[MAX_TASK];
extern TPriorityQueue PriorityQueues[MAX_PRIORITY];
extern TResource ResourceQueue[MAX_RES];
extern TEvent EventQueue[MAX_EVENTS];

extern int RunningTask;

extern int FreeTask;

extern int FreeResource;

void Schedule(int task, int mode);

void Dispatch(int task);

