#include "lib.h"
#include "paging.h"

static uint32_t page_directory[NUM_PAGE_DIRECTORY_ENTRY] __attribute__((aligned(BYTES_TO_ALIGN_TO)));
static uint32_t page_table[NUM_PAGE_TABLE_ENTRY] __attribute__((aligned(BYTES_TO_ALIGN_TO)));
static uint32_t page_table_execute[NUM_PAGE_TABLE_ENTRY] __attribute__((aligned(BYTES_TO_ALIGN_TO)));
static uint32_t page_table_vid[NUM_PAGE_TABLE_ENTRY] __attribute__((aligned(BYTES_TO_ALIGN_TO)));

/* 
 * paging_init
 *   DESCRIPTION: Initialize and enable paging. 
 *                Virtual memory 0-4MB is broken down into 4kB pages. Only vedio memory is mapped 
 *                  to physical memory, other memory is marked as not presented.
 *                Virtual memory 4-8MB is mapped to physical memory 4-8Mb using single 4Mb page.
 *                Virtual memory 8Mb-4GB is marked as not presented.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: control register CR0, CR3, CR4 are modified.
 *                 page fault is enabled.
 */

void paging_init(void)
{
    int i;          // Variable used for iteration
    int pt_vedio_index;   // Variable used for setting page table for vedio memory
    int pt_vedio_index0;
    int pt_vedio_index1;
    int pt_vedio_index2; 


    // Initialize page directory
    for (i = 0; i < NUM_PAGE_DIRECTORY_ENTRY; i++)
        page_directory[i] = PAGE_DIRECTORY_INIT;

    // Only 2 page directory entries are used
    // Initialize both page directory entries
    // format see page.h and manual 3-24
    page_directory[0] = (uint32_t)page_table | PAGE_DIRECTORY_0_SETTING;
    page_directory[1] = KERNEL_ADDRESS | PAGE_DIRECTORY_1_SETTING;

    // Initialize page table
    for (i = 0; i < NUM_PAGE_TABLE_ENTRY; i++)
        page_table[i] = i * MEMORY_SIZE_4KB | PAGE_TABLE_INIT_SETTING;

    // Calculate index in page table of video memory
    pt_vedio_index = (VIDEO >> VIRTUAL_ADDRESS_PAGE_TABLE_OFFSET) & VIRTUAL_ADDRESS_PAGE_TABLE_MASK;
    pt_vedio_index0 = (VBUF_0 >> VIRTUAL_ADDRESS_PAGE_TABLE_OFFSET) & VIRTUAL_ADDRESS_PAGE_TABLE_MASK;
    pt_vedio_index1 = (VBUF_1 >> VIRTUAL_ADDRESS_PAGE_TABLE_OFFSET) & VIRTUAL_ADDRESS_PAGE_TABLE_MASK;
    pt_vedio_index2 = (VBUF_2 >> VIRTUAL_ADDRESS_PAGE_TABLE_OFFSET) & VIRTUAL_ADDRESS_PAGE_TABLE_MASK;

    // Initialze video memory in page table
    page_table[pt_vedio_index] = VIDEO | PAGE_TABLE_VIDEO_SETTING;
    page_table[pt_vedio_index0] = VBUF_0 | PAGE_TABLE_VIDEO_SETTING;
    page_table[pt_vedio_index1] = VBUF_1 | PAGE_TABLE_VIDEO_SETTING;
    page_table[pt_vedio_index2] = VBUF_2 | PAGE_TABLE_VIDEO_SETTING;


    // Set CR3 to be the base address of page_directory
    // Enable PG flag and PE flag in CR0
    // Enable in CR4 to enable 4MiB pages
    // reference https://wiki.osdev.org/Paging
    // Must set CR0 after setting CR3 & CR4 or the system will crash
    asm volatile("             \n\
        movl %0, %%eax         \n\
        movl %%eax, %%cr3      \n\
        movl %%cr4, %%eax      \n\
        orl $0x00000010, %%eax \n\
        movl %%eax, %%cr4      \n\
        movl %%cr0, %%eax      \n\
        orl $0x80000001, %%eax \n\
        movl %%eax, %%cr0      \n\
        "
                :
                : "g"(page_directory)
                : "eax");
}

/* 
 * create_user_level_program_paging
 *   DESCRIPTION: Initialize and enable user level program paging. 
 *                User level program is located at 128 MB, which correspons to page directory[32]
 *                A new page table is created to map the 4 MB memory into
 *   INPUTS: pcb_index - indicate the physical memory which this program maps to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: control register CR3 is modified.
 */

void create_user_level_program_paging(int pcb_index) {
    int i; // iteration variable

    // Update the value of page directory
    page_directory[USER_LEVEL_PROGRAM_PDE_INDEX] = (uint32_t)page_table_execute | PAGE_DIRECTORY_USER_SETTING;

    // Update each entry of the new page table
    for (i = 0; i < NUM_PAGE_TABLE_ENTRY; i++)
        page_table_execute[i] = (PYHSYCAL_START_ADDRESS + MEMORY_USER_LEVEL_PROGRAM * pcb_index + i * MEMORY_SIZE_4KB) 
            | PAGE_TABLE_USER_SETTING;

    // Set the address of page direcoty to register cr3 to enable new paging
    asm volatile("             \n\
        movl %0, %%eax         \n\
        movl %%eax, %%cr3      \n\
        "
                :
                : "g"(page_directory)
                : "eax");
}

/* 
 * create_user_level_program_paging
 *   DESCRIPTION: Initialize and enable user level program paging. 
 *                User level program is located at 128 MB, which correspons to page directory[32]
 *                A new page table is created to map the 4 MB memory into
 *   INPUTS: pcb_index - indicate the physical memory which this program maps to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: control register CR3 is modified.
 */

void create_vidmap_paging() {
    
    // mapped to 132MB
    page_directory[USER_LEVEL_PROGRAM_PDE_INDEX + 1] = (uint32_t)page_table_vid | PAGE_DIRECTORY_USER_SETTING;
    
    page_table_vid[0] = VIDMAP_MEMORY | PAGE_TABLE_USER_SETTING;
    // page_table_vid[1] = (VIDMAP_MEMORY + 0x00048000) | PAGE_TABLE_USER_SETTING;

    // Set the address of page direcoty to register cr3 to enable new paging
    asm volatile("             \n\
        movl %0, %%eax         \n\
        movl %%eax, %%cr3      \n\
        "
                :
                : "g"(page_directory)
                : "eax");
}

/* 
 * create_user_level_program_paging
 *   DESCRIPTION: Initialize and enable user level program paging. 
 *                User level program is located at 128 MB, which correspons to page directory[32]
 *                A new page table is created to map the 4 MB memory into
 *   INPUTS: pcb_index - indicate the physical memory which this program maps to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: control register CR3 is modified.
 */
void remap(uint32_t virtual, uint32_t physical){
    uint32_t pde_index = virtual / _4MB;
    // page_directory[pde_index]=(uint32_t) page_table_remap | PAGE_DIRECTORY_USER_SETTING;
    // page_table_remap[0]=physical | PAGE_TABLE_USER_SETTING;

    page_directory[pde_index] = (uint32_t) page_table_vid | PAGE_DIRECTORY_USER_SETTING;
    page_table_vid[0] = physical | PAGE_TABLE_USER_SETTING;
    // page_table_vid[1]=(physical+0x00048000) | PAGE_TABLE_USER_SETTING ;

    asm volatile(
        "movl %0, %%eax;"
        "movl %%eax, %%cr3;"
        :
        :"g"(page_directory)
        :"eax"
        ); 
}
