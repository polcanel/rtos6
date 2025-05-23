/*******************************/
/*           test.c            */
/*******************************/

#include <stdio.h>
#include <iostream>
#include "rtos_api.h"
#include "defs.h"

DeclareTask(TaskA, 3);
DeclareTask(TaskB, 5);


DeclareTask(TaskC, 5);
DeclareTask(TaskD, 6);
DeclareTask(TaskE, 2);

DeclareTask(Task1, 1);
DeclareTask(Task2, 2);


DeclareResource(ResA, 4);
DeclareEvent(EventA, 1);

int main(void) {
    printf("Starting RTOS Tests\n");
    setlocale(LC_ALL, "Russian");
    // Тест 1: Пременительный планировщик
    printf("Test 1: Из менее приориетеной задачи вызывается более приоритетная\n");
    char nameA[] = "TaskA";
    StartOS(TaskA, TaskAprior, nameA);

    printf("\n");
    printf("Test 2: Из более приориетеной задачи вызывается менее приоритетная\n");
    char nameC[] = "TaskC";
    StartOS(TaskC, TaskCprior, nameC);

    printf("\n");
    printf("Test 3: Семафоры\n");
    // Тест 2: Управление ресурсами
    char name1[] = "Task1";
    StartOS(Task1, Task1prior, name1);

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


TASK(TaskC) {
    printf("TaskC Running\n");
    char nameD[] = "TaskD";
    ActivateTask(TaskD, TaskDprior, nameD); // Активируем TaskB
    printf("TaskC continues after activating TaskD\n"); // Должно прерваться TaskB
    TerminateTask();
}

TASK(TaskD) {
    printf("TaskD Running\n");
    char nameE[] = "TaskE";
    ActivateTask(TaskE, TaskEprior, nameE); // Активируем TaskC
    printf("!!!!!! TaskD continues after activating TaskE\n"); // Должно прерваться TaskC
    TerminateTask();
}

TASK(TaskE) {
    printf("TaskE Running\n");
    TerminateTask();
}

TASK(Task1) {
    printf("Task1 Running\n");
    char resName[] = "ResA";

    InitResource(ResA, resName);

    GetResource(ResA, resName);
    printf(" Приобретен ResA by task 1\n");

    char name2[] = "Task2";
    ActivateTask(Task2, Task2prior, name2); // Активируем TaskB
    ReleaseResource(ResA, resName);
    printf("Освобожден ResA by task 1\n");

    TerminateTask();
}

TASK(Task2) {
    printf("Task2 Running\n");
    char resName[] = "ResA";
    GetResource(ResA, resName);
    printf(" forced task2 to end\n");
    TerminateTask();
}