#ifndef _PAGING_H
#define _PAGING_H

/* constants */

#define NUM_PAGE_DIRECTORY_ENTRY  1024
#define NUM_PAGE_TABLE_ENTRY      1024
#define BYTES_TO_ALIGN_TO         4096
#define MEMORY_SIZE_4KB           4096
#define _4MB                      0x400000
#define KERNEL_ADDRESS            0x400000
#define VIDMAP_MEMORY             0xB8000


/* page directory entry setting -- manual 3-24
 * | 31                                12 | 11   9 | 8 |  7  | 6 | 5 |  4  |  3  |  2  |  1  | 0 |
 * |       Page-table base address        |  Avail | G |  PS | 0 | A | PCD | PWT | U/S | R/W | P |
 *
 *
 * page_directory_0_setting:
 * |                 *                    |  0 0 0 | 0 |  0  | 0 | 0 |  0  |  0  |  0  |  1  | 1 |
 * page_directory_1_setting:
 * |                 *                    |  0 0 0 | 0 |  1  | 0 | 0 |  0  |  0  |  0  |  1  | 1 |
 * not present
 * |                                                                                         | 0 |
 */

#define PAGE_DIRECTORY_INIT       0x00
#define PAGE_TABLE_INIT           0x00
#define PAGE_DIRECTORY_0_SETTING  0x03
#define PAGE_DIRECTORY_1_SETTING  0x83

/* used by user level page directory */
#define PAGE_DIRECTORY_USER_SETTING 0x07



/* page table entry 4-kB setting -- manual 3-24
 * | 31                                12 | 11   9 | 8 |  7  | 6 | 5 |  4  |  3  |  2  |  1  | 0 |
 * |       Page-table base address        |  Avail | G | PAT | D | A | PCD | PWT | U/S | R/W | P |
 *
 *
 * page_table_initial_setting:
 * |                 *                    |  0 0 0 | 0 |  0  | 0 | 0 |  0  |  0  |  0  |  1  | 1 |
 * page_table_video_setting:
 * |                 *                    |  0 0 0 | 0 |  0  | 0 | 0 |  0  |  0  |  0  |  1  | 1 |
 */
#define PAGE_TABLE_INIT_SETTING     0x00
#define PAGE_TABLE_VIDEO_SETTING    0x03
#define PAGE_TABLE_GENERAL_SETTING  0x03

/* used by user level page table */
#define PAGE_TABLE_USER_SETTING     0x07


/* virtual address 
 * | 31                   22 | 21               12 | 11              0 |
 * | index in page directory | index in page table | off set into page |
 */

#define VIRTUAL_ADDRESS_PAGE_TABLE_OFFSET   12
#define VIRTUAL_ADDRESS_PAGE_TABLE_MASK     0x3FF


#define PYHSYCAL_START_ADDRESS              0x800000
#define MEMORY_USER_LEVEL_PROGRAM           0x400000
#define USER_LEVEL_PROGRAM_PDE_INDEX        32

/* Initialize and enable paging. 
 * Virtual memory 0-4MB is broken down into 4kB pages. Only vedio memory is mapped 
 * to physical memory, other memory is marked as not presented.
 * Virtual memory 4-8MB is mapped to physical memory 4-8Mb using single 4Mb page.
 * Virtual memory 8Mb-4GB is marked as not presented. */
extern void paging_init(void);

/* Initialize and enable user level program paging. 
 * User level program is located at 128 MB, which correspons to page directory[32]
 * A new page table is created to map the 4 MB memory into */
extern void create_user_level_program_paging(int pcb_index);


extern void create_vidmap_paging();

extern void remap(uint32_t virtual, uint32_t physical);

#endif
