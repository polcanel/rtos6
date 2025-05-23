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
    }
    TaskQueue[MAX_TASK - 1].ref = -1;

    for (int i = 0; i < MAX_RES; i++) {
        SemaphoreQueue[i].task = -1;
        SemaphoreQueue[i].name = NULL;
    }

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
        int timeout = 10;
        while (!EventQueue[event_id].signaled && timeout > 0) {
            timeout--;
        }
        if (EventQueue[event_id].signaled) {
            EventQueue[event_id].signaled = 0;
        }
        else {
            printf("WaitEvent %d timed out\n", event_id);
        }
    }
}

void SetEvent(int event_id) {
    if (event_id >= 0 && event_id < MAX_EVENTS) {
        EventQueue[event_id].signaled = 1;
        printf("SetEvent %d\n", event_id);
    }
}