#ifndef PCB_H
#define PCB_H

#include "filesystem.h"
#include "types.h"
#include "terminal.h"

/* constants */

#define KERNEL_BOTTOM   0x800000
#define KERNEL_TOP      0x400000
#define PCB_BOTTOM      0x7fe000
#define BLOCK_8KB       0x2000
#define NUM_PCB         6

#define ARG_BUFFER_SIZE 128

/* define the pcb structure */

typedef struct program_control_block {
    uint32_t  in_use;                               /* record whether pcb is in use. */
    uint32_t  pid;                                  /* process ID */
    uint32_t  parent_pid;                           /* parent process ID to calculate parent pcb address */
    uint32_t  parent_esp0;                          /* parent kernel stack pointer */
    uint32_t  parent_ebp0;                          /* parent kernel base pointer */
    uint32_t  esp;                                  /* curent kernel stack pointer */
    uint32_t  ebp;                                  /* curent kernel base pointer */
    terminal_t*         terminal;                   /* terminal to which the process belong */
    file_descriptor_t   fd_table[MAX_OPENNING_FILE];/* file descriptor array */
    uint8_t   arg_buffer[ARG_BUFFER_SIZE];          /* arguments from command */
} pcb_t;

/* initialize pcb structure in kernel and allocate pcb for shell */
extern void pcb_init();

/* initialize file descriptor in given pcb */
extern int32_t file_descriptor_init(pcb_t* pcb);

/* allocate and initialize an unused pcb for a new task */
extern int32_t allocate_pcb();

/* calculate the address of pcb by its pid */
extern pcb_t* get_pcb_by_pid(int32_t pid);

extern int curr_pid; /* global integer recording the latest pid */


#endif
