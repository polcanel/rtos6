/****************************************/
/*           rtos_api.h                 */
/****************************************/

#define DeclareTask(TaskID, priority) \
    TASK(TaskID);                     \
    enum {TaskID##prior = priority}

#define DeclareResource(ResID, priority) \
    enum {ResID = priority}

#define DeclareEvent(EventID, index) \
    enum { EventID = index }

#define TASK(TaskID) void TaskID(void)

typedef void TTaskCall(void);

void ActivateTask(TTaskCall entry, int priority, char* name);
void TerminateTask(void);
int StartOS(TTaskCall entry, int priority, char* name);
void ShutdownOS();
void P(char* name);
void V(char* name);
void InitPVS(char* name);
void WaitEvent(int event_id);
void SetEvent(int event_id);
