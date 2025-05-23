/****************************************/
/*               sys.h                   */
/****************************************/

#include "defs.h"
#include <setjmp.h> // ƒл€ сохранени€ контекста

#define INSERT_TO_TAIL 1
#define INSERT_TO_HEAD 0

typedef struct Type_Task {
    int ref; // »ндекс следующей задачи в очереди того же приоритета
    int priority;
    void (*entry)(void);
    char* name;
    int suspended;
    int waiting_event;
    jmp_buf context; // —охран€ем контекст выполнени€
} TTask;

typedef struct Type_semaphore {
    int task;
    char* name;
} TSemaphore;

typedef struct {
    int event_id;
    int signaled;
} TEvent;

// ћассив очередей: кажда€ очередь соответствует уровню приоритета
typedef struct {
    int head; // »ндекс первой задачи в очереди
    int tail; // »ндекс последней задачи в очереди
} PriorityQueue;

extern TTask TaskQueue[MAX_TASK];
extern PriorityQueue PriorityQueues[MAX_PRIORITY];
extern TSemaphore SemaphoreQueue[MAX_RES];
extern TEvent EventQueue[MAX_EVENTS];
extern int RunningTask;
extern int FreeTask;
extern int FreeResource;

void Schedule(int task, int mode);
void Dispatch(int task);
void SuspendTask(int task);
void ResumeTask(int task);