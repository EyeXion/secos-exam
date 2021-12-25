#include <page.h>


pde32_t * PGD_KERN = (pde32_t*)PGD_KERN_ADDR; //PGD for the kernel
pte32_t * PTB_KERN1 = (pte32_t*)PTB1_KERN_ADDR; //PTB for the kernel to map the kernel
pte32_t * PTB_KERN2 = (pte32_t*)PTB2_KERN_ADDR; //PTB for the kernel to map the gdt/tss and PGDs/PTBs


pde32_t * PGD_USER1 = (pde32_t*)PGD_USER1_ADDR; //PGD for user1
pte32_t * PTB1_USER1 = (pte32_t*)PTB1_USER1_ADDR; //PTB for user1 to map the kernel (escpecially the kernel stack)
pte32_t * PTB2_USER1 = (pte32_t*)PTB2_USER1_ADDR; //PTB for user1 to map its code and user/kernel stacks, and shared meme page

pde32_t * PGD_USER2 = (pde32_t*)PGD_USER2_ADDR; //PGD for user2
pte32_t * PTB1_USER2 = (pte32_t*)PTB1_USER2_ADDR; //PTB for user2 to map the kernel (escpecially the kernel stack)
pte32_t * PTB2_USER2 = (pte32_t*)PTB2_USER2_ADDR; //PTB for user2 to map its code and user/kernel stacks,  and shared meme page
pte32_t * PTB3_USER2 = (pte32_t*)PTB3_USER2_ADDR; //PTB for user2 to map the code of the function sys_counter


void set_id_mapping_kernel(){
    //We set the first PTB with ID id_mapping (1024 entries per PTB). Kernel code mainly
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB_KERN1[i], (PG_RW | PG_KRN), i); //We put i as offset, but if 1 = 1; ofsset = 4KO cuz alignement on 4KO (but automatic cuz 1 will be interpreted as 1 << 12)
  }

  //Set PTB1 as first entry of PGD1 (we dont use the 12 LSB cuz 4KO alignement)
  pg_set_entry(&PGD_KERN[0], (PG_RW | PG_KRN), (int)PTB_KERN1 >> 12);


  //We set the second PTB in order to answer question 5 (access adress 0x601000)
  for (int i = 0; i <1024; i++){
    pg_set_entry(&PTB_KERN2[i], (PG_RW | PG_KRN), i + 1024*1); //1024 * 1 cuz second PTB, so offsets are 4KO further than 0
  }

  pg_set_entry(&PGD_KERN[1], (PG_RW | PG_KRN), (int)PTB_KERN2 >> 12);

}

/*
The memory zone for user 1 (code and user/kernel stack) between 0x800000 and 0xbfffff
(before 0xc00000) -> mapped in PTB_USER1

Shared memory zone will be mapped as well 

*/
void set_id_mapping_user1(){

  //Mapping of the kernel (includes kernel stack)
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB1_USER1[i], (PG_RW | PG_KRN), i);
  }

  pg_set_entry(&PGD_USER1[0], (PG_RW | PG_KRN), (int)PTB1_USER1 >> 12);

  
  //Mapping of the user space (code, user stack)
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB2_USER1[i], (PG_RW | PG_USR), i + 1024*2);
  }

  //Compute the index in the PTB2_USER1 for the addr of the shared memeory page
  int shared_mem_ptb_index = (SHARED_MEM_VADDR_U1)>>12 & 0x3FF;
  pg_set_entry(&PTB2_USER1[shared_mem_ptb_index], (PG_RW | PG_USR), (int)(SHARED_MEM_PADDR)>>12);

  pg_set_entry(&PGD_USER1[2], (PG_RW | PG_USR), (int)PTB2_USER1 >> 12); //Index 2 cuz 0x8000000 start of user1 space

}

/*
The memory zone for user 2 (code and user stack) between 0xc00000 and 0xffffff
(before 0x1000000) -> mapped in PTB_USER2

Shared memory zone will be mapped as well 

*/
void set_id_mapping_user2(){

  //Mapping of the kernel (includes kernel stack)
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB1_USER2[i], (PG_RW | PG_KRN), i);
  }

  pg_set_entry(&PGD_USER2[0], (PG_RW | PG_KRN), (int)PTB1_USER2 >> 12);

  
  //Mapping of the user space (code, user stack)
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB2_USER2[i], (PG_RW | PG_USR), i + 1024*3);
  }

  //Compute the index in the PTB2_USER1 for the addr of the shared memeory page
  int shared_mem_ptb_index = (SHARED_MEM_VADDR_U2)>>12 & 0x3FF;
  pg_set_entry(&PTB2_USER2[shared_mem_ptb_index], (PG_RW | PG_USR), (int)(SHARED_MEM_PADDR)>>12);

  pg_set_entry(&PGD_USER2[3], (PG_RW | PG_USR), (int)PTB2_USER2 >> 12); //Index 2 cuz 0x8000000 start of user1 space


  //Mapping for the section for the sys_count function
  for (int i = 0; i<1024; i++){
    pg_set_entry(&PTB3_USER2[i], (PG_RW | PG_USR), i + 1024*1);
  }

  pg_set_entry(&PGD_USER2[1], (PG_RW | PG_USR), (int)PTB3_USER2 >> 12);


}



void set_pagination(){
  set_id_mapping_kernel();
  set_id_mapping_user1();
  set_id_mapping_user2();

  //Set shared memory to 0 (before the pagination activated...)
  uint32_t * shared_mem = (uint32_t *) SHARED_MEM_PADDR;
  *shared_mem = 0;

  set_cr3(PGD_KERN_ADDR);
  set_cr0(get_cr0() | CR0_PG);
}


