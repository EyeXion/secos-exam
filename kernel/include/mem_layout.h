#ifndef __MEMLAY_H__
#define __MEMLAY_H__

//Memory layout, all constants are here (in theory)

//PAGINATION

#define PGD_KERN_ADDR 0x200000
#define PTB1_KERN_ADDR 0x201000
#define PTB2_KERN_ADDR 0x202000

#define PGD_USER1_ADDR 0x203000
#define PTB1_USER1_ADDR 0x204000
#define PTB2_USER1_ADDR 0x205000


#define PGD_USER2_ADDR 0x206000
#define PTB1_USER2_ADDR 0x207000
#define PTB2_USER2_ADDR 0x208000
#define PTB3_USER2_ADDR 0x209000



// SHARED MEM

#define SHARED_MEM_PADDR 0x500000
#define SHARED_MEM_VADDR_U1 0xa05000
#define SHARED_MEM_VADDR_U2 0xe05000


//USER SPACE LAYOUT (start from the end of the page !)

// USER1 space starts at 0x800000 (see linker)
// USER2 space starts at 0xc00000 (see linker)


#define USER1_USTACK_ADDR 0xbe0000
#define USER2_USTACK_ADDR 0xfe0000

// KERNEL SPACE LAYOUT (start from the end of the page !)

#define USER1_KSTACK_ADDR 0x290000
#define USER2_KSTACK_ADDR 0x2b0000

// Sections of code users

#define USER1_CODE_ADDR 0x800000
#define USER2_CODE_ADDR 0xc00000


//TSS

#define TSS_SEG_ADDR 0x105000;


#endif