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
    TaskQueue[occupy].ref = -1;

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

    // Удаляем задачу из очереди
    int priority = TaskQueue[task].priority;
    if (PriorityQueues[priority].head == task) {
        PriorityQueues[priority].head = TaskQueue[task].ref;
        if (PriorityQueues[priority].head == -1) {
            PriorityQueues[priority].tail = -1;
        }
    }
    else {
        int prev = PriorityQueues[priority].head;
        while (prev != -1 && TaskQueue[prev].ref != task) {
            prev = TaskQueue[prev].ref;
        }
        if (prev != -1) {
            TaskQueue[prev].ref = TaskQueue[task].ref;
            if (PriorityQueues[priority].tail == task) {
                PriorityQueues[priority].tail = prev;
            }
        }
    }

    RunningTask = -1; // После завершения задачи выбираем новую
    
    for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
        if (PriorityQueues[i].head != -1) {
            RunningTask = PriorityQueues[i].head;
            break;
        }
    }

    Dispatch(RunningTask); // Выбираем следующую задачу

    printf("End of TerminateTask %s\n", TaskQueue[task].name ? TaskQueue[task].name : "(null)");

    TaskQueue[task].ref = FreeTask;
    TaskQueue[task].name = NULL;
    FreeTask = task;

    
}

void Schedule(int task, int mode) {
    int priority;

    if (TaskQueue[task].name == NULL) {
        printf("Schedule: Task %d has no name, skipping\n", task);
        return;
    }

    printf("Schedule %s\n", TaskQueue[task].name);

    priority = TaskQueue[task].priority;

    // Добавляем задачу в очередь соответствующего уровня приоритета
    if (PriorityQueues[priority].head == -1) {
        PriorityQueues[priority].head = task;
        PriorityQueues[priority].tail = task;
        TaskQueue[task].ref = -1;
    }
    else {
        if (mode == INSERT_TO_TAIL) {
            TaskQueue[PriorityQueues[priority].tail].ref = task;
            PriorityQueues[priority].tail = task;
            TaskQueue[task].ref = -1;
        }
        else {
            TaskQueue[task].ref = PriorityQueues[priority].head;
            PriorityQueues[priority].head = task;
        }
    }

    // Выбираем новую задачу для выполнения
    int i;
    RunningTask = -1;
    for (i = MAX_PRIORITY - 1; i >= 0; i--) {
        if (PriorityQueues[i].head != -1) {
            RunningTask = PriorityQueues[i].head;
            break;
        }
    }

    printf("End of Schedule %s\n", TaskQueue[task].name);
}

void Dispatch(int task) {
    printf("Dispatch\n");

    if (RunningTask != -1) {
        TaskQueue[RunningTask].entry();
    }

    printf("End of Dispatch\n");
}