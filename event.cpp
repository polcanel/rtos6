/*************************************/
/*             event.c               */
/*************************************/

#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

void WaitEvent(int event_id)
{
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

void SetEvent(int event_id)
{
    if (event_id >= 0 && event_id < MAX_EVENTS) {
        EventQueue[event_id].signaled = 1;
        printf("SetEvent %d\n", event_id);
    }
}
