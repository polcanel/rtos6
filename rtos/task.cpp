/*********************************/
/*          task.c               */
/*********************************/
#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

void ActivateTask(TTaskCall entry, int priority, char* name) {
    int task, occupy;

    printf("ActivateTask %s\n", name);

    task = RunningTask;

    occupy = FreeTask;
    FreeTask = TaskQueue[occupy].ref;

    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].entry = entry;

    Schedule(occupy, INSERT_TO_TAIL);

    if (task != RunningTask) {
        Dispatch(task);
    }

    printf("End of ActivateTask %s\n", name);
}

void TerminateTask(void) {
    int task;

    task = RunningTask;

    printf("TerminateTask %s\n", TaskQueue[task].name);

    RunningTask = TaskQueue[task].ref;

    TaskQueue[task].ref = FreeTask;
    TaskQueue[task].name = NULL; // Очищаем имя для избежания "Schedule (null)"
    FreeTask = task;

    printf("End of TerminateTask %s\n", TaskQueue[task].name ? TaskQueue[task].name : "(null)");
}

void Schedule(int task, int mode) {
    int cur, prev;
    int priority;

    if (TaskQueue[task].name == NULL) {
        printf("Schedule: Task %d has no name, skipping\n", task);
        return;
    }

    printf("Schedule %s\n", TaskQueue[task].name);

    cur = RunningTask;
    prev = -1;

    priority = TaskQueue[task].priority;

    while (cur != -1 && TaskQueue[cur].priority > priority) {
        prev = cur;
        cur = TaskQueue[cur].ref;
    }

    if (mode == INSERT_TO_TAIL) {
        while (cur != -1 && TaskQueue[cur].priority == priority) {
            prev = cur;
            cur = TaskQueue[cur].ref;
        }
    }

    TaskQueue[task].ref = cur;

    if (prev == -1) RunningTask = task;
    else TaskQueue[prev].ref = task;

    printf("End of Schedule %s\n", TaskQueue[task].name);
}

void Dispatch(int task) {
    printf("Dispatch\n");

    // Выполняем только текущую задачу с наивысшим приоритетом
    if (RunningTask != -1) {
        TaskQueue[RunningTask].entry();
    }

    printf("End of Dispatch\n");
}