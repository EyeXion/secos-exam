/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <gdt.h>
#include <page.h>
#include <tasks.h>
#include <mem_layout.h>
#include <asm.h>


extern info_t *info;

void tp()
{
  init_gdt();
  show_gdt();
  set_pagination();

  init_task(USER1_KSTACK_ADDR, USER1_USTACK_ADDR, USER1_CODE_ADDR, PGD_USER1_ADDR);
  init_task(USER2_KSTACK_ADDR, USER2_USTACK_ADDR, USER2_CODE_ADDR, PGD_USER2_ADDR);

  
  //force_interrupts_on();
  printf("Helloooo\n");

  asm volatile ("int $32");

  printf("Goodbye\n");


  while(1){}
}
