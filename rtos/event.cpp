/*************************************/
/* event.c               */
/*************************************/

#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

// Создает системное событие
void CreateEvent(int event_id, char* name)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        printf("Error: Invalid event ID for CreateEvent\n");
        return;
    }

    // В данной простой реализации, ID события соответствует индексу в EventQueue
    // Если FreeEvent управляет свободными слотами, то это нужно использовать
    int occupy = FreeEvent;
    if (occupy == -1) {
        printf("Error: No free event slots available\n");
        return;
    }

    FreeEvent = EventQueue[occupy].id; // Следующий свободный слот

    EventQueue[occupy].id = event_id; // Устанавливаем ID события
    EventQueue[occupy].status = 0; // Изначально событие не установлено
    EventQueue[occupy].waiting_tasks_head = -1; // Список ожидающих задач пуст
    printf("Event %s created with ID %d\n", name, event_id);
}


// Устанавливает событие, активируя ожидающие задачи
void SetEvent(int event_id)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        printf("Error: Invalid event ID for SetEvent\n");
        return;
    }

    printf("SetEvent %d\n", event_id);

    EventQueue[event_id].status = EVENT_FLAG_SET; // Устанавливаем флаг события

    int current_waiting_task = EventQueue[event_id].waiting_tasks_head;
    int prev_task = -1;

    // Активируем все задачи, ожидающие это событие
    while (current_waiting_task != -1)
    {
        int next_waiting_task = TaskQueue[current_waiting_task].ref; // Сохраняем следующую задачу

        // Если задача ожидает это событие
        if (TaskQueue[current_waiting_task].event_mask == event_id)
        {
            TaskQueue[current_waiting_task].state = 0; // Меняем состояние на READY
            TaskQueue[current_waiting_task].event_mask = 0; // Сбрасываем маску события
            TaskQueue[current_waiting_task].ref = -1; // Сбрасываем ссылку, так как она будет перепланирована

            Schedule(current_waiting_task, INSERT_TO_HEAD); // Добавляем задачу обратно в очередь планировщика

            // Удаляем задачу из списка ожидающих задач события
            if (prev_task == -1) {
                EventQueue[event_id].waiting_tasks_head = next_waiting_task;
            }
            else {
                TaskQueue[prev_task].ref = next_waiting_task;
            }
        }
        prev_task = current_waiting_task;
        current_waiting_task = next_waiting_task;
    }

    // Перезапускаем диспетчер, если текущая задача изменилась или появилась более приоритетная
    if (RunningTask != -1 && TaskQueue[RunningTask].state == EVENT_FLAG_WAITING) {
        Dispatch(RunningTask);
    }
    else if (RunningTask == -1) { // Если ОС еще не запущена или все задачи завершены
        Dispatch(-1);
    }
}

// Очищает событие (сбрасывает флаг)
void ClearEvent(int event_id)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        printf("Error: Invalid event ID for ClearEvent\n");
        return;
    }

    printf("ClearEvent %d\n", event_id);
    EventQueue[event_id].status = 0; // Сбрасываем флаг события
}

// Задача ожидает установки события
void WaitEvent(int event_id)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        printf("Error: Invalid event ID for WaitEvent\n");
        return;
    }

    printf("Task %s is waiting for Event %d\n", TaskQueue[RunningTask].name, event_id);

    if (EventQueue[event_id].status == EVENT_FLAG_SET)
    {
        printf("Event %d is already set. Task %s continues.\n", event_id, TaskQueue[RunningTask].name);
        return; // Событие уже установлено, задача продолжает выполнение
    }

    // Задача переходит в состояние ожидания
    TaskQueue[RunningTask].state = EVENT_FLAG_WAITING;
    TaskQueue[RunningTask].event_mask = event_id; // Указываем, какое событие ожидает задача

    // Добавляем текущую задачу в список ожидающих задач для этого события
    TaskQueue[RunningTask].ref = EventQueue[event_id].waiting_tasks_head;
    EventQueue[event_id].waiting_tasks_head = RunningTask;

    // Переключаемся на следующую готовую задачу
    int prev_running_task = RunningTask;
    RunningTask = TaskQueue[RunningTask].ref; // Временно, чтобы диспетчер выбрал следующую

    // Удаляем задачу из очереди планировщика, так как она теперь в состоянии ожидания
    int cur = RunningTask;
    int prev = -1;
    while (cur != -1 && cur != prev_running_task) {
        prev = cur;
        cur = TaskQueue[cur].ref;
    }
    if (prev != -1) {
        TaskQueue[prev].ref = TaskQueue[prev_running_task].ref;
    }
    else if (RunningTask == prev_running_task) { // Если это была единственная задача
        RunningTask = TaskQueue[prev_running_task].ref;
    }

    // Инициируем диспетчеризацию для выбора следующей готовой задачи
    Dispatch(prev_running_task);
}