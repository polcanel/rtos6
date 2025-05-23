/*************************************/
/*             resource.c            */
/*************************************/

#include "sys.h"
#include "rtos_api.h"
#include <stdio.h>
#include <cstring>

void InitPVS(char* name) {
    int free_occupy = FreeResource;
    if (free_occupy >= MAX_RES) {
        printf("InitPVS %s failed: no free slots\n", name);
        return;
    }

    SemaphoreQueue[free_occupy].task = -1; // Ресурс свободен
    SemaphoreQueue[free_occupy].name = name;
    FreeResource++;

    printf("Semaphore for %s initialized\n", name);
}



void P(char* name) {
    int resource_index = -1;

    printf("P(semahore) for %s\n", name);

    // Ищем ресурс по имени
    for (int i = 0; i < MAX_RES; i++) {
        if (SemaphoreQueue[i].name != NULL && strcmp(SemaphoreQueue[i].name, name) == 0) {
            resource_index = i;
            break;
        }
    }

    if (resource_index == -1) {
        printf("P(semaphore) for %s failed: resource not found\n", name);
        return;
    }

    // Проверяем, занят ли ресурс
    if (SemaphoreQueue[resource_index].task != -1) {
        // Ресурс занят, приостанавливаем текущую задачу
        printf("P(semaphore) for %s failed: resource is taken, press q to exit task by force otherwise u r stuck here forever :( \n", name);
        while (true) {
            char c = getchar();
            if (c == 'q') {
                break;
            }
        }
        return;
    }

    // Захватываем ресурс
    SemaphoreQueue[resource_index].task = RunningTask;
    printf("P(semaphore) for %s acquired by task %s\n", name, TaskQueue[RunningTask].name);
}

void V(char* name) {
    int resource_index = -1;

    printf("V %s\n", name);

    // Ищем ресурс по имени
    for (int i = 0; i < MAX_RES; i++) {
        if (SemaphoreQueue[i].name != NULL && strcmp(SemaphoreQueue[i].name, name) == 0) {
            resource_index = i;
            break;
        }
    }

    if (resource_index == -1 || SemaphoreQueue[resource_index].task != RunningTask) {
        printf("V(semaphore) %s failed: resource not found or not owned by task\n", name);
        return;
    }

    // Освобождаем ресурс
    SemaphoreQueue[resource_index].task = -1;
    printf("V(semaphore) for %s released by task %s\n", name, TaskQueue[RunningTask].name);
}