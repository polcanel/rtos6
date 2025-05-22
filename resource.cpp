/*************************************/
/*             resource.c            */
/*************************************/

#include "sys.h"
#include "rtos_api.h"
#include <stdio.h>

void GetResource(int priority, char* name)
{
	int free_occupy;

	printf("GetResource %s\n", name);

	free_occupy = FreeResource;
	if (free_occupy >= MAX_RES || ResourceQueue[free_occupy].task != -1) {
		printf("GetResource %s failed: resource not available\n", name);
		return;
	}

	FreeResource = ResourceQueue[free_occupy].priority;
	ResourceQueue[free_occupy].priority = priority;
	ResourceQueue[free_occupy].task = RunningTask;
	ResourceQueue[free_occupy].name = name;
}

void ReleaseResource(int priority, char* name)
{
    int i, ResourceIndex;

    printf("ReleaseResource %s\n", name);

    ResourceIndex = 0;
    while (ResourceIndex < MAX_RES &&
        (ResourceQueue[ResourceIndex].task != RunningTask ||
            ResourceQueue[ResourceIndex].priority != priority ||
            ResourceQueue[ResourceIndex].name != name)) {
        ResourceIndex++;
    }

    if (ResourceIndex < MAX_RES) {
        ResourceQueue[ResourceIndex].priority = FreeResource;
        ResourceQueue[ResourceIndex].task = -1;
        FreeResource = ResourceIndex;
    }
    else {
        printf("ReleaseResource %s failed: resource not found\n", name);
    }
}