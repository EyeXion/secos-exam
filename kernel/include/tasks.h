#ifndef __TASKS_H_
#define __TASKS_H_

#include <gdt.h>
#include <types.h>
#include <intr.h>


#define NB_MAX_TASKS 10


/*Structure that represents a task that has been started (can be interrupted though).
We consider that all the tasks neven end (so we never destroy a task)
Data structure used to save the context of all the tasks so that the scheduler can choose
which one to reload

Will use an array with an index to count number of tasks (cirucular list could have been used as well ...)
*/
typedef struct task_ctx_t{

    //ID of the task (more for the fun of it :)
    uint32_t pid;

    //ESP in the kernel stack for this user
    uint32_t esp_kernel;

    //The address of the PGD for this task
    uint32_t PGD;

} task_ctx_t;



/*
Will init a new task and add it to the array, but without starting it (will be started with the scheduler)
As well as setting up its kernel stack in order to be called after an iret in the scheduler
*/
void init_task(uint32_t esp_kernel, uint32_t esp_user, uint32_t eip, uint32_t PGD);

//Code of the task user1
void __attribute__((section(".user1"))) user1();

//Code of the task user2
void __attribute__((section(".user2"))) user2();

//Function called during the interruption 32, manages the tasks
void scheduler(int_ctx_t * ctx);


#endif