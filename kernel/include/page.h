#ifndef __PAGE_H__
#define __PAGE_H__

#include <pagemem.h>
#include <cr.h>
#include <debug.h>
#include <mem_layout.h>

//Sets the identity mapping for the kernel
void set_id_mapping_kernel();

//Sets the pagination for the user1 task
void set_id_mapping_user1();

//Sets the pagination for the user2 task
void set_id_mapping_user2();

//Wrapper to set the pagination of everything
void set_pagination();


#endif