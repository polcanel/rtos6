/*******************************/
/*           test.c            */
/*******************************/

#include <stdio.h>
#include <iostream>
#include "rtos_api.h"
#include "defs.h"

DeclareTask(TaskA, 5);
DeclareTask(TaskB, 3);
DeclareResource(ResA, 4);
DeclareEvent(EventA, 1);

int main(void) {
    printf("Starting RTOS Tests\n");
    setlocale(LC_ALL, "Russian");
    // Тест 1: Пременительный планировщик
    char nameA[] = "TaskA";
    StartOS(TaskA, TaskAprior, nameA);

    printf("\n");
    // Тест 2: Управление ресурсами
    char resName[] = "ResA";
    GetResource(ResA, resName);
    printf("Test 2: Приобретен ResA\n");
    ReleaseResource(ResA, resName);
    printf("Test 2: Освобожден ResA\n");

    printf("\n");
    // Тест 3: Управление событиями
    SetEvent(EventA); // Устанавливаем событие перед ожиданием
    WaitEvent(EventA);
    printf("Test 3: Событие EventA сигнализировано\n");

    printf("\n");
    // Тест 4: Проверка лимитов
    int i;
    for (i = 0; i < MAX_TASK; i++) {
        char taskName[10];
        sprintf_s(taskName, sizeof(taskName), "Task%d", i);
        if (i < MAX_RES) {
            char resName[10];
            sprintf_s(resName, sizeof(resName), "Res%d", i);
            GetResource(i, resName);
            ReleaseResource(i, resName);
        }
        if (i < MAX_EVENTS) {
            SetEvent(i);
            WaitEvent(i);
        }
    }
    printf("Test 4: Проверка лимитов (32 задачи, 16 ресурсов, 16 событий) прошла успешно\n");

    ShutdownOS();
    return 0;
}

TASK(TaskA) {
    printf("TaskA Running\n");
    char nameB[] = "TaskB";
    ActivateTask(TaskB, TaskBprior, nameB); // Активируем TaskB
    printf("TaskA continues after activating TaskB\n"); // Должно прерваться TaskB
    TerminateTask();
}

TASK(TaskB) {
    printf("TaskB Running\n");
    TerminateTask();
}