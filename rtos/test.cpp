/*******************************/
/*           test.c            */
/*******************************/

#include <stdio.h>
#include <iostream>
#include "rtos_api.h"
#include "defs.h"

DeclareTask(TaskA, 3);
DeclareTask(TaskB, 3);


DeclareTask(TaskC, 5);
DeclareTask(TaskD, 6);
DeclareTask(TaskE, 2);

DeclareTask(TaskOne, 4);
DeclareTask(TaskTwo, 4);

DeclareTask(Task1, 1);
DeclareTask(Task2, 2);


DeclareEvent(EventA, 1);

int main(void) {
    printf("Starting RTOS Tests\n");
    setlocale(LC_ALL, "Russian");
     //Тест 1: планировщик
    printf("Test 1: Из менее приориетеной задачи вызывается более приоритетная\n");
    char nameA[] = "TaskA";
    StartOS(TaskA, TaskAprior, nameA);

    printf("\n");
    printf("Test 2: Из более приориетеной задачи вызывается менее приоритетная\n");
    char nameC[] = "TaskC";
    StartOS(TaskC, TaskCprior, nameC);

    printf("\n");
    printf("Test 3: Семафоры\n");
     //Тест 2: Управление ресурсами
    char name1[] = "Task1";
    StartOS(Task1, Task1prior, name1);

    printf("\n");
    printf("Test 4: События\n");
    char name[] = "TaskOne(which waits)";
    StartOS(TaskOne, TaskOneprior, name);

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
    InitPVS(resName);
    P(resName);
    printf(" Приобретен ResA by task 1\n");

    char name2[] = "Task2";
    ActivateTask(Task2, Task2prior, name2); // Активируем TaskB

    V(resName);
    printf("Освобожден ResA by task 1\n");
    TerminateTask();
}

TASK(Task2) {
    printf("Task2 Running\n");
    char resName[] = "ResA";
    P(resName);
    printf("\tforced task2 to end\n");
    TerminateTask();
}

TASK(TaskOne) {
    printf("TaskOne Running\n");
    char name[] = "TaskTwo(which signaled)";
    ActivateTask(TaskTwo, TaskTwoprior, name);
    WaitEvent(0);
    printf("TaskOne get signal\n");
    TerminateTask();
}

TASK(TaskTwo) {
    printf("TaskTwo Running\n");
    SetEvent(0);
    printf("TaskTwo set event\n");
    TerminateTask();
}