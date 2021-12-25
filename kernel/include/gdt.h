#ifndef __GDT_H__
#define __GDT_H__



#include <info.h>
#include <segmem.h>
#include <string.h>
#include <debug.h>
#include <mem_layout.h>

#define NB_MAX_SEG_DESC 6
#define GDT_IDX_CODE_R0 1 
#define GDT_IDX_DATA_R0 2
#define GDT_IDX_CODE_R3 3
#define GDT_IDX_DATA_R3 4
#define GDT_IDX_TSS 5


//Prints the gdt from the gdtr
void show_gdt();

//Creates another segment in the GDT (flat)
void init_seg_desc(uint64_t limit, uint64_t base, uint64_t type, uint64_t dpl);

//Creates the segment for the TSS
void init_seg_desc_tss();

//Initialises the GDT
void init_gdt();

//Loads the segment registers in R0
void load_seg_reg_R0();


#endif
