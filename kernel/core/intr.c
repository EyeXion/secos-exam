/* GPLv2 (c) Airbus */
#include <intr.h>
#include <debug.h>
#include <info.h>
#include <tasks.h>

extern info_t *info;
extern void idt_trampoline();
extern void idt_80();
extern void idt_32();
static int_desc_t IDT[IDT_NR_DESC];


void intr_init()
{
   idt_reg_t idtr;
   offset_t  isr;
   size_t    i;

   isr = (offset_t)idt_trampoline;

   /* re-use default grub GDT code descriptor */
   for(i=0 ; i<IDT_NR_DESC ; i++, isr += IDT_ISR_ALGN)
      int_desc(&IDT[i], gdt_krn_seg_sel(1), isr);


   //set handler for the int 80
   int_desc(&IDT[80], gdt_krn_seg_sel(1), (offset_t)idt_80);
   (&IDT[80])->dpl = 3;
   
   //set the handler for the int 32
   int_desc(&IDT[32], gdt_krn_seg_sel(1), (offset_t)idt_32);

   idtr.desc  = IDT;
   idtr.limit = sizeof(IDT) - 1;
   set_idtr(idtr);
}


void __regparm__(1) intr_hdlr_80(int_ctx_t * ctx){
   uint32_t* compteur = (uint32_t*) ctx->gpr.esi.raw;
   debug("############# COMPTEUR = %d ###############\n", *compteur);
}


void __regparm__(1) intr_hdlr_32(int_ctx_t * ctx){
      scheduler(ctx);
}

void __regparm__(1) intr_hdlr(int_ctx_t *ctx)
{

   uint8_t vector = ctx->nr.blow;
   
   debug("\nIDT event\n"
         " . int    #%d\n"
         " . error  0x%x\n"
         " . cs:eip 0x%x:0x%x\n"
         " . ss:esp 0x%x:0x%x\n"
         " . eflags 0x%x\n"
         "\n- GPR\n"
         "eax     : 0x%x\n"
         "ecx     : 0x%x\n"
         "edx     : 0x%x\n"
         "ebx     : 0x%x\n"
         "esp     : 0x%x\n"
         "ebp     : 0x%x\n"
         "esi     : 0x%x\n"
         "edi     : 0x%x\n"
         ,ctx->nr.raw, ctx->err.raw
         ,ctx->cs.raw, ctx->eip.raw
         ,ctx->ss.raw, ctx->esp.raw
         ,ctx->eflags.raw
         ,ctx->gpr.eax.raw
         ,ctx->gpr.ecx.raw
         ,ctx->gpr.edx.raw
         ,ctx->gpr.ebx.raw
         ,ctx->gpr.esp.raw
         ,ctx->gpr.ebp.raw
         ,ctx->gpr.esi.raw
         ,ctx->gpr.edi.raw);



   if(vector < NR_EXCP)
      excp_hdlr(ctx);
   else
      debug("ignore IRQ %d\n", vector);
}
