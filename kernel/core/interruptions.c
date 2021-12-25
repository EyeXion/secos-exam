#include <interruptions.h>


void set_idt_handler(int index, int dpl, offset_t handler){
    idt_reg_t idtr;
    get_idtr(idtr);
    int_desc_t * IDT = idtr.desc;
    int_desc(&IDT[index], IDT[index].selector, handler);
    (&IDT[index])->dpl = dpl;
    (&IDT[index])->p = 1;
}