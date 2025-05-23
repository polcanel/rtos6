/*************************************/
/*             resource.c            */
/*************************************/

#include "sys.h"
#include "rtos_api.h"
#include <stdio.h>
#include <cstring>

void InitResource(int priority, char* name) {
    int free_occupy = FreeResource;
    if (free_occupy >= MAX_RES) {
        printf("InitResource %s failed: no free slots\n", name);
        return;
    }

    FreeResource = ResourceQueue[free_occupy].priority;
    ResourceQueue[free_occupy].priority = priority;
    ResourceQueue[free_occupy].task = -1; // Ресурс свободен
    ResourceQueue[free_occupy].name = name;

    printf("Resource %s initialized with priority %d\n", name, priority);
}

//void GetResource(int priority, char* name) {
//    int free_occupy;
//
//    printf("GetResource %s\n", name);
//
//    free_occupy = FreeResource;
//    if (free_occupy >= MAX_RES || ResourceQueue[free_occupy].task != -1) {
//        printf("GetResource %s failed: resource not available\n", name);
//        return; // Ресурс занят, простая семафорная модель
//    }
//
//    FreeResource = ResourceQueue[free_occupy].priority;
//    ResourceQueue[free_occupy].priority = priority;
//    ResourceQueue[free_occupy].task = RunningTask;
//    ResourceQueue[free_occupy].name = name;
//
//    //printf("Test 2: Приобретен ResA\n");
//}
//
//void ReleaseResource(int priority, char* name) {
//    int i, ResourceIndex;
//
//    printf("ReleaseResource %s\n", name);
//
//    ResourceIndex = 0;
//    while (ResourceIndex < MAX_RES &&
//        (ResourceQueue[ResourceIndex].task != RunningTask ||
//            ResourceQueue[ResourceIndex].priority != priority ||
//            ResourceQueue[ResourceIndex].name != name)) {
//        ResourceIndex++;
//    }
//
//    if (ResourceIndex < MAX_RES) {
//        ResourceQueue[ResourceIndex].priority = FreeResource;
//        ResourceQueue[ResourceIndex].task = -1;
//        FreeResource = ResourceIndex;
//        //printf("Освобожден ResA\n");
//    }
//    else {
//        printf("ReleaseResource %s failed: resource not found\n", name);
//    }
//}

void GetResource(int priority, char* name) {
    int resource_index = -1;

    printf("GetResource %s\n", name);

    // Ищем ресурс по имени
    for (int i = 0; i < MAX_RES; i++) {
        if (ResourceQueue[i].name != NULL && strcmp(ResourceQueue[i].name, name) == 0) {
            resource_index = i;
            break;
        }
    }

    if (resource_index == -1) {
        printf("GetResource %s failed: resource not found\n", name);
        return;
    }

    // Проверяем, занят ли ресурс
    if (ResourceQueue[resource_index].task != -1) {
        // Ресурс занят, приостанавливаем текущую задачу
        printf("GetResource %s failed: resource is taken, press q to exit task by force otherwise u r stuck here forever :( \n", name);
        while (true) {
            char c = getchar();
            if (c = 'q') {
                break;
            }
        }
        return;
    }

    // Захватываем ресурс
    ResourceQueue[resource_index].task = RunningTask;
    printf("Resource %s acquired by task %s\n", name, TaskQueue[RunningTask].name);
}

void ReleaseResource(int priority, char* name) {
    int resource_index = -1;

    printf("ReleaseResource %s\n", name);

    // Ищем ресурс по имени
    for (int i = 0; i < MAX_RES; i++) {
        if (ResourceQueue[i].name != NULL && strcmp(ResourceQueue[i].name, name) == 0) {
            resource_index = i;
            break;
        }
    }

    if (resource_index == -1 || ResourceQueue[resource_index].task != RunningTask) {
        printf("ReleaseResource %s failed: resource not found or not owned by task\n", name);
        return;
    }

    // Освобождаем ресурс
    ResourceQueue[resource_index].task = -1;
    printf("Resource %s released by task %s\n", name, TaskQueue[RunningTask].name);

    //// Проверяем, есть ли ожидающие задачи
    //if (ResourceQueue[resource_index].waiting_task != -1) {
    //    int waiting_task = ResourceQueue[resource_index].waiting_task;
    //    ResourceQueue[resource_index].waiting_task = -1;
    //    //TaskQueue[waiting_task].state = TASK_READY;
    //    ResumeTask(waiting_task);
    //}
}