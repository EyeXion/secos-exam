#include <gdt.h>


seg_desc_t gdt[NB_MAX_SEG_DESC];

void show_gdt(){
    printf("\n\t####### NEW SHOW GDT######\n\n");
    gdt_reg_t grub_gdtr;
    get_gdtr(grub_gdtr);

    seg_desc_t * base_gdt = grub_gdtr.desc;
    offset_t end_gdt = grub_gdtr.addr + (grub_gdtr.limit + 1);


    for (seg_desc_t * desc_addr = base_gdt; desc_addr < (seg_desc_t *) end_gdt; desc_addr++){
        seg_desc_t seg = *(desc_addr);
        uint32_t base_seg = seg.base_1 | (seg.base_2 << 16) | (seg.base_3 << 24);
        uint32_t limit_seg = seg.limit_1 | (seg.limit_2 << 16);

        printf("### Seg desc located at 0x%x - RAW GDT : 0x%x\n", desc_addr, seg.raw);
        printf("\tBASE : 0x%x - ", base_seg);
        printf("LIMIT : 0x%x - ", limit_seg);
        printf("TYPE : 0x%x - ", seg.type);
        printf("S : 0x%x - ", seg.s);
        printf("DPL : 0x%x - ", seg.dpl);
        printf("P : 0x%x - ", seg.p);
        printf("AVL : 0x%x - ", seg.avl);
        printf("L : 0x%x - ", seg.l);
        printf("D : 0x%x - ", seg.d);
        printf("G : 0x%x\n", seg.g);
    }
}

void init_seg_desc(uint64_t limit, uint64_t base, uint64_t type, uint64_t dpl){
    int i;
    gdt_reg_t gdtr;
    get_gdtr(gdtr);
    if (gdtr.limit == 0){
        i = 0;
    }
    else{
        i = (gdtr.limit + 1)/sizeof(seg_desc_t);
    }
    gdt[i].raw = 0ULL;
    gdt[i].base_1 = base;
    gdt[i].base_2 = base >> 16;
    gdt[i].base_3 = base >> 24;
    gdt[i].limit_1 = limit;
    gdt[i].limit_2 = limit >> 16;
    gdt[i].type = type;
    gdt[i].dpl = dpl;

    gdt[i].s = 1; 
    gdt[i].p = 1;
    gdt[i].avl = 0;
    gdt[i].l = 0;
    gdt[i].d = 1;
    gdt[i].g = 1;

    if (i == 0){
        gdtr.limit = sizeof(seg_desc_t) - 1;
        gdt[0].raw = 0ULL;
    }
    else{
        gdtr.limit = gdtr.limit + sizeof(seg_desc_t);
    }
    set_gdtr(gdtr);
}

void init_seg_desc_tss(){
    int i;
    gdt_reg_t gdtr;
    get_gdtr(gdtr);
    if (gdtr.limit == 0){
        i = 0;
    }
    else{
        i = (gdtr.limit + 1)/sizeof(seg_desc_t);
    }

    gdt[i].raw = 0ULL;

    offset_t base = (offset_t)TSS_SEG_ADDR;
    int limit = sizeof(tss_t);
    gdt[i].base_1 = base;
    gdt[i].base_2 = base >> 16;
    gdt[i].base_3 = base >> 24;
    gdt[i].limit_1 = limit;
    gdt[i].limit_2 = limit >> 16;
    gdt[i].type = SEG_DESC_SYS_TSS_AVL_32;
    gdt[i].dpl = 0;

    gdt[i].s = 0; 
    gdt[i].p = 1;
    gdt[i].avl = 0;
    gdt[i].l = 0;
    gdt[i].d = 0;
    gdt[i].g = 0;

    gdtr.limit = gdtr.limit + sizeof(seg_desc_t);
    set_gdtr(gdtr);
}

void load_seg_regs_R0(){
  set_cs(gdt_krn_seg_sel(GDT_IDX_CODE_R0));
  set_ds(gdt_krn_seg_sel(GDT_IDX_DATA_R0));
  set_es(gdt_krn_seg_sel(GDT_IDX_DATA_R0));
  set_fs(gdt_krn_seg_sel(GDT_IDX_DATA_R0));
  set_gs(gdt_krn_seg_sel(GDT_IDX_DATA_R0));
  set_ss(gdt_krn_seg_sel(GDT_IDX_DATA_R0));
}

void init_gdt(){
    gdt_reg_t gdtr;
    gdtr.limit = 0;
    gdtr.desc = gdt;
    set_gdtr(gdtr);
    init_seg_desc(0,0,0,0); //1er segment null Index0
    init_seg_desc(0xfffff,0,SEG_DESC_CODE_XR,0); //Segment code Ring 0 Index1
    init_seg_desc(0xfffff,0,SEG_DESC_DATA_RW,0); //Segment data Ring 0 Index2
    init_seg_desc(0xfffff,0,SEG_DESC_CODE_XR,3); //Segment Code Ring 3 Index3
    init_seg_desc(0xfffff,0,SEG_DESC_DATA_RW,3); //Segment data Ring 3 index4
    init_seg_desc_tss(); 
    set_tr(gdt_krn_seg_sel(GDT_IDX_TSS));
}
