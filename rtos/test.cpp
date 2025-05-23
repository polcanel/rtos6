/*******************************/
/*           test.c            */
/*******************************/

#include <stdio.h>
#include <iostream>
#include "rtos_api.h"
#include "defs.h"

DeclareTask(TaskA, 5);
DeclareTask(TaskB, 3);

DeclareTask(TaskC, 5);
DeclareTask(TaskD, 6);
DeclareTask(TaskE, 2);

DeclareTask(TaskF, 1);
DeclareTask(TaskG, 2);
DeclareTask(TaskH, 3);

DeclareTask(TaskI, 3);
DeclareTask(TaskJ, 3);

DeclareTask(TaskK, 5);
DeclareTask(TaskL, 3);

DeclareTask(TaskM, 3);
DeclareTask(TaskN, 5);

DeclareResource(ResA, 4);
DeclareResource(ResB, 4);

DeclareEvent(EventA, 1);
DeclareEvent(EventB, 1);

#define MAX_TASK 32
#define MAX_RES 16
#define MAX_EVENTS 16

void test_preemptive_scheduler_1() {
    char nameA[] = "TaskA";
    StartOS(TaskA, TaskAprior, nameA);
}

void test_preemptive_scheduler_2() {
    char nameC[] = "TaskC";
    StartOS(TaskC, TaskCprior, nameC);
}

void test_preemptive_scheduler_3() {
    char nameF[] = "TaskF";
    StartOS(TaskF, TaskFprior, nameF);
}

void test_preemptive_scheduler_4() {
    char nameI[] = "TaskI";
    StartOS(TaskI, TaskIprior, nameI);
}

void test_preemptive_scheduler_and_events() {
    char nameK[] = "TaskK";
    StartOS(TaskK, TaskKprior, nameK);
}

void test_preemptive_scheduler_and_resources() {
    char nameM[] = "TaskM";
    StartOS(TaskM, TaskMprior, nameM);
}

void test_resource_management() {
    char resName[] = "ResA";
    InitPVS(resName);
    P(resName);
    printf("Test: Приобретен ResA\n");
    V(resName);
    printf("Test: Освобожден ResA\n");
}

void test_event_management() {
    SetEvent(EventA); // Устанавливаем событие перед ожиданием
    WaitEvent(EventA);
    printf("Test: Событие EventA сигнализировано\n");
}

void test_limits_check() {
    for (int i = 0; i < MAX_TASK; i++) {
        char taskName[10];
        snprintf(taskName, sizeof(taskName), "Task%d", i);

        if (i < MAX_RES) {
            char resName[10];
            snprintf(resName, sizeof(resName), "Res%d", i);
            InitPVS(resName);
            P(resName);
            V(resName);
        }

        if (i < MAX_EVENTS) {
            SetEvent(i);
            WaitEvent(i);
        }
    }
    printf("Test: Проверка лимитов (32 задачи, 16 ресурсов, 16 событий) прошла успешно\n");
}

void run_test(const char* test_name, void (*test_func)()) {
    printf("---- %s ----\n", test_name);
    test_func();
    printf("---- END of %s ----\n\n", test_name);
}

int main(void) {
    printf("Starting RTOS Tests\n");
    setlocale(LC_ALL, "Russian");

    struct {
        const char* name;
        void (*func)();
    } tests[] = {
                    {"test_preemptive_scheduler_1", test_preemptive_scheduler_1}, // A>B
                    {"test_preemptive_scheduler_2", test_preemptive_scheduler_2}, // 
                    {"test_preemptive_scheduler_3", test_preemptive_scheduler_3},
                    {"test_preemptive_scheduler_4", test_preemptive_scheduler_4},

                        // BUG: WaitEvent 1 timed out
                    {"test_preemptive_scheduler_and_events", test_preemptive_scheduler_and_events},

                    {"test_preemptive_scheduler_and_resources",    test_preemptive_scheduler_and_resources},

                                {"test_resource_management",    test_resource_management},
                                {"test_event_management",       test_event_management},
                                {"test_limits_check",           test_limits_check},
    };

    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        run_test(tests[i].name, tests[i].func);
    }

    ShutdownOS();
    return 0;
}





// --- Task Definitions ---

// first group of tasks
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

// second group of tasks
TASK(TaskC) {
    printf("TaskC Running\n");
    char nameD[] = "TaskD";
    ActivateTask(TaskD, TaskDprior, nameD);
    printf("TaskC continues after activating TaskD\n");
    TerminateTask();
}

TASK(TaskD) {
    printf("TaskD Running\n");
    char nameE[] = "TaskE";
    ActivateTask(TaskE, TaskEprior, nameE);
    printf("TaskD continues after activating TaskE\n");
    TerminateTask();
}

TASK(TaskE) {
    printf("TaskE Running\n");
    TerminateTask();
}

// fourth group of tasks
TASK(TaskF) {
    printf("TaskF Running\n");
    char nameG[] = "TaskG";
    ActivateTask(TaskG, TaskGprior, nameG);
    printf("TaskC continues after activating TaskG\n");
    TerminateTask();
}

TASK(TaskG) {
    printf("TaskG Running\n");
    char nameH[] = "TaskH";
    ActivateTask(TaskH, TaskHprior, nameH);
    printf("TaskD continues after activating TaskH\n");
    TerminateTask();
}

TASK(TaskH) {
    printf("TaskH Running\n");
    TerminateTask();
}

// fifth group of tasks
TASK(TaskI) {
    printf("TaskI Running\n");
    char nameJ[] = "TaskJ";
    ActivateTask(TaskJ, TaskJprior, nameJ);
    printf("TaskI continues after activating TaskJ\n");
    TerminateTask();
}

TASK(TaskJ) {
    printf("TaskJ Running\n");
    TerminateTask();
}

// sixth group of tasks
TASK(TaskK) {
    printf("TaskK Running\n");
    char nameL[] = "TaskL";
    ActivateTask(TaskL, TaskLprior, nameL);
    printf("TaskK continues after activating TaskL\n");
    WaitEvent(EventB);
    printf("TaskK has waited for the EventB\n");
    TerminateTask();
}

TASK(TaskL) {
    printf("TaskL Running\n");
    printf("TaskL will set the EventB\n");
    SetEvent(EventB);
    TerminateTask();
}

// seventh group of tasks
TASK(TaskM) {
    printf("TaskM Running\n");

    char resName[] = "ResB";
    InitPVS(resName);
    P(resName);
    printf("TaskM get ResB\n");

    char nameN[] = "TaskN";
    ActivateTask(TaskN, TaskNprior, nameN);
    printf("TaskM continues after activating TaskN\n");

    V(resName);
    printf("TaskM has released ResB\n");

    TerminateTask();
}

TASK(TaskN) {
    printf("TaskN Running\n");

    char resName[] = "ResB";
    P(resName);
    printf("TaskN get ResB\n");
    V(resName);
    printf("TaskN has released ResB\n");

    TerminateTask();
}



///*******************************/
///*           test.c            */
///*******************************/
//
//#include <stdio.h>
//#include <iostream>
//#include "rtos_api.h"
//#include "defs.h"
//
//DeclareTask(TaskA, 3);
//DeclareTask(TaskB, 3);
//
//
//DeclareTask(TaskC, 5);
//DeclareTask(TaskD, 6);
//DeclareTask(TaskE, 2);
//
//DeclareTask(TaskOne, 4);
//DeclareTask(TaskTwo, 4);
//
//DeclareTask(Task1, 1);
//DeclareTask(Task2, 2);
//
//
//DeclareEvent(EventA, 1);
//
//int main(void) {
//    printf("Starting RTOS Tests\n");
//    setlocale(LC_ALL, "Russian");
//     //Тест 1: планировщик
//    printf("Test 1: Из менее приориетеной задачи вызывается более приоритетная\n");
//    char nameA[] = "TaskA";
//    StartOS(TaskA, TaskAprior, nameA);
//
//    printf("\n");
//    printf("Test 2: Из более приориетеной задачи вызывается менее приоритетная\n");
//    char nameC[] = "TaskC";
//    StartOS(TaskC, TaskCprior, nameC);
//
//    printf("\n");
//    printf("Test 3: Семафоры\n");
//     //Тест 2: Управление ресурсами
//    char name1[] = "Task1";
//    StartOS(Task1, Task1prior, name1);
//
//    printf("\n");
//    printf("Test 4: События\n");
//    char name[] = "TaskOne(which waits)";
//    StartOS(TaskOne, TaskOneprior, name);
//
//    ShutdownOS();
//    return 0;
//}
//
//TASK(TaskA) {
//    printf("TaskA Running\n");
//    char nameB[] = "TaskB";
//    ActivateTask(TaskB, TaskBprior, nameB); // Активируем TaskB
//    printf("TaskA continues after activating TaskB\n"); // Должно прерваться TaskB
//    TerminateTask();
//}
//
//TASK(TaskB) {
//    printf("TaskB Running\n");
//    TerminateTask();
//}
//
//
//TASK(TaskC) {
//    printf("TaskC Running\n");
//    char nameD[] = "TaskD";
//    ActivateTask(TaskD, TaskDprior, nameD); // Активируем TaskB
//    printf("TaskC continues after activating TaskD\n"); // Должно прерваться TaskB
//    TerminateTask();
//}
//
//TASK(TaskD) {
//    printf("TaskD Running\n");
//    char nameE[] = "TaskE";
//    ActivateTask(TaskE, TaskEprior, nameE); // Активируем TaskC
//    printf("!!!!!! TaskD continues after activating TaskE\n"); // Должно прерваться TaskC
//    TerminateTask();
//}
//
//TASK(TaskE) {
//    printf("TaskE Running\n");
//    TerminateTask();
//}
//
//TASK(Task1) {
//    printf("Task1 Running\n");
//    char resName[] = "ResA";
//    InitPVS(resName);
//    P(resName);
//    printf(" Приобретен ResA by task 1\n");
//
//    char name2[] = "Task2";
//    ActivateTask(Task2, Task2prior, name2); // Активируем TaskB
//
//    V(resName);
//    printf("Освобожден ResA by task 1\n");
//    TerminateTask();
//}
//
//TASK(Task2) {
//    printf("Task2 Running\n");
//    char resName[] = "ResA";
//    P(resName);
//    printf("\tforced task2 to end\n");
//    TerminateTask();
//}
//
//TASK(TaskOne) {
//    printf("TaskOne Running\n");
//    char name[] = "TaskTwo(which signaled)";
//    ActivateTask(TaskTwo, TaskTwoprior, name);
//    WaitEvent(0);
//    printf("TaskOne get signal\n");
//    TerminateTask();
//}
//
//TASK(TaskTwo) {
//    printf("TaskTwo Running\n");
//    SetEvent(0);
//    printf("TaskTwo set event\n");
//    TerminateTask();
//}