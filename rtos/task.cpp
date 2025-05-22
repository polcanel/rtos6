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
    TaskQueue[occupy].suspended = 0;

    Schedule(occupy, INSERT_TO_TAIL);

    if (task != -1 && TaskQueue[task].priority < priority && !TaskQueue[task].suspended) {
        SuspendTask(task);
    }

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


    printf("End of TerminateTask %s\n", TaskQueue[task].name ? TaskQueue[task].name : "(null)");
    TaskQueue[task].ref = FreeTask;
    TaskQueue[task].name = NULL;
    TaskQueue[task].suspended = 0;
    FreeTask = task;



    RunningTask = -1;
    for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
        int curr = PriorityQueues[i].head;
        while (curr != -1) {
            if (!TaskQueue[curr].suspended) {
                RunningTask = curr;
                break;
            }
            else {
                //taskToResume = curr;
                //ResumeTask(curr);
                RunningTask = curr;
                break;
            }
            curr = TaskQueue[curr].ref;
        }
        if (RunningTask != -1) break;
    }

    if (RunningTask != -1) {
        Dispatch(RunningTask);
    }
    

    //RunningTask = -1; // После завершения задачи выбираем новую
    //for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
    //    if (PriorityQueues[i].head != -1) {
    //        RunningTask = PriorityQueues[i].head;
    //        break;
    //    }
    //}

    //Dispatch(RunningTask); // Выбираем следующую задачу    
}


void SuspendTask(int task) {
    //int task = RunningTask;
    if (task != -1) {
        printf("SuspendTask %s\n", TaskQueue[task].name);
        TaskQueue[task].suspended = 1;
        if (setjmp(TaskQueue[task].context) == 0) {
            // Сохраняем контекст и возвращаемся к Dispatch
            return;
        }
    }
}

void ResumeTask(int task) {
    if (task != -1 && TaskQueue[task].suspended) {
        printf("ResumeTask %s\n", TaskQueue[task].name);
        TaskQueue[task].suspended = 0;
        longjmp(TaskQueue[task].context, 1); // Возвращаемся к точке прерывания
    }
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
        int curr = PriorityQueues[i].head;
        while (curr != -1) {
            if (!TaskQueue[curr].suspended) {
                RunningTask = curr;
                break;
            }
            curr = TaskQueue[curr].ref;
        }
        if (RunningTask != -1) break;
    }

    /*int i;
    RunningTask = -1;
    for (i = MAX_PRIORITY - 1; i >= 0; i--) {
        if (PriorityQueues[i].head != -1) {
            RunningTask = PriorityQueues[i].head;
            break;
        }
    }*/

    printf("End of Schedule %s\n", TaskQueue[task].name);
}

void Dispatch(int task) {
    printf("Dispatch\n");

    /*if (RunningTask != -1) {
        TaskQueue[RunningTask].entry();
    }*/


    static int prev_task = -1; // Сохраняем предыдущую задачу для возобновления

    // Если текущая задача завершена, возобновляем приостановленные задачи
    if (task != -1 && task != RunningTask && prev_task != -1 && TaskQueue[prev_task].suspended) {
        ResumeTask(prev_task);
    }

    if (RunningTask != -1 && !TaskQueue[RunningTask].suspended) {
        TaskQueue[RunningTask].entry();
    }

    prev_task = RunningTask;


    printf("End of Dispatch\n");
}