#include <tasks.h>
#include <mem_layout.h>
#include <cr.h>
#include <asm.h>
#include <pic.h>


task_ctx_t tasks[NB_MAX_TASKS];
int nb_tasks = -1;
int current_task_pid = -1;

tss_t * tss = (tss_t *) TSS_SEG_ADDR;


void init_task(uint32_t esp_kernel, uint32_t esp_user, uint32_t eip, uint32_t PGD){

    //Setting up the kernel stack for this process
    uint32_t * esp_k = (uint32_t *) esp_kernel; //Cast to pointer
    *(esp_k) = gdt_usr_seg_sel(GDT_IDX_DATA_R3);
    *(esp_k - 1) = esp_user;
    *(esp_k - 2) = EFLAGS_IF;
    *(esp_k - 3) = gdt_usr_seg_sel(GDT_IDX_CODE_R3);
    *(esp_k - 4) = eip;

    //Setup the struct in the array for this task
    nb_tasks++;
    tasks[nb_tasks].pid = nb_tasks;
    // esp_k - 14 cuz we have the interruption stack for the iret, the err code and int code and the general registers to pop when we exit an interruption
    tasks[nb_tasks].esp_kernel = (uint32_t)(esp_k - 14);
    tasks[nb_tasks].PGD = PGD;

    debug("\n####TASK ADDED (PID : %d)####\n\n", nb_tasks);
    debug("SAVED SS : 0x%x\n", *(esp_k));
    debug("SAVED ESP : 0x%x\n", *(esp_k - 1));
    debug("SAVED EFLAGS : 0x%x\n", *(esp_k - 2));
    debug("SAVED CS : 0x%x\n", *(esp_k - 3));
    debug("SAVED EIP : 0x%x\n", *(esp_k - 4));
    debug("ESP KERNEL : 0x%x\n\n", esp_k - 14);
}


void __attribute__((section(".sys_count"))) sys_counter(uint32_t *counter){
    asm volatile("int $80"::"S"(counter));
}


void __attribute__((section(".user1"))) user1(){

    uint32_t * shared_mem = (uint32_t *) SHARED_MEM_VADDR_U1;

    while(1){
        *shared_mem += 1;
    }
}


void __attribute__((section(".user2"))) user2(){

    uint32_t * shared_mem = (uint32_t *) SHARED_MEM_VADDR_U2;

    while(1){
        sys_counter(shared_mem);
    }
}


void scheduler(int_ctx_t * ctx){

    debug("In scheduler with index of task : %d and ctx raw : %p\n", current_task_pid, ctx->cs.raw);
    if (ctx->cs.raw == 0x8){
        printf("Interruption of an interruption (RING 0)\n");
    }
    else{
        printf("Interruption of a task (RING 3)\n");
    }

    if (nb_tasks == 0){
        printf("No other task to switch to, just continuing");
    }
    else if (current_task_pid == -1){
        current_task_pid = 0;
        set_cr3(tasks[current_task_pid].PGD);
    }

    else{
        tasks[current_task_pid].esp_kernel =  (uint32_t) ctx;
        current_task_pid = (current_task_pid + 1) % (nb_tasks +1);
        set_cr3(tasks[current_task_pid].PGD);
    }

    uint32_t esp_kernel = (uint32_t)(((uint32_t *)tasks[current_task_pid].esp_kernel));
    set_esp(esp_kernel);
    tss->s0.esp = tasks[current_task_pid].esp_kernel;
    tss->s0.ss = gdt_krn_seg_sel(GDT_IDX_DATA_R0);
    printf("ESP AFTER MOV : 0x%x\n", get_esp());
    asm volatile ("popa"); // To simulate the resume_from_interruption
    asm volatile("add $8, %esp");
    asm volatile ("iret");
}