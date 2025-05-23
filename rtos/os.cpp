/******************************/
/*          os.c              */
/******************************/

#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

int StartOS(TTaskCall entry, int priority, char* name) {
    int i;

    RunningTask = -1;
    FreeTask = 0;
    FreeResource = 0;

    printf("StartOS!\n");

    for (i = 0; i < MAX_TASK; i++) {
        TaskQueue[i].ref = i + 1;
        TaskQueue[i].suspended = 0;
        TaskQueue[i].waiting_event = -1;
    }
    TaskQueue[MAX_TASK - 1].ref = -1;

    for (int i = 0; i < MAX_RES; i++) {
        ResourceQueue[i].priority = i + 1;
        ResourceQueue[i].task = -1;
        ResourceQueue[i].name = NULL;
    }
    ResourceQueue[MAX_RES - 1].priority = -1;

    for (i = 0; i < MAX_EVENTS; i++) {
        EventQueue[i].event_id = i;
        EventQueue[i].signaled = 0;
    }

    // »нициализаци€ очередей по уровн€м приоритета
    for (i = 0; i < MAX_PRIORITY; i++) {
        PriorityQueues[i].head = -1;
        PriorityQueues[i].tail = -1;
    }

     ActivateTask(entry, priority, name);

    return 0;
}

void ShutdownOS() {
    printf("ShutdownOS!\n");
}

void WaitEvent(int event_id) {
    if (event_id >= 0 && event_id < MAX_EVENTS) {
        int task = RunningTask;

        printf("WaitEvent %d by task %s\n", event_id, TaskQueue[task].name);

        TaskQueue[task].waiting_event = event_id;
        SuspendTask(task);
        RunningTask = -1;

        for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
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

        Dispatch(RunningTask);
    }
}

void SetEvent(int event_id) {
    if (event_id >= 0 && event_id < MAX_EVENTS) {
        //EventQueue[event_id].signaled = 1;

        printf("SetEvent %d\n", event_id);

        for (int i = 0; i < MAX_TASK; i++) {
            if (TaskQueue[i].suspended && TaskQueue[i].waiting_event == event_id) {
                TaskQueue[i].waiting_event = -1;
                ResumeTask(i);
                break;
            }
        }
    }
}