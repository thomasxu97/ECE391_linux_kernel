#include "pcb.h"
#include "filesystem.h"
#include "terminal.h"

/* pcb_init
 * initialize pcb structure in kernel
 * Input: none
 * Output: none
 * Return: none
 * Side Effect: none
 */
void pcb_init()
{
    int i;          /* variable for iteration */
    // Initialize all pcb
    for (i = 0; i < NUM_PCB; i++)
        ((pcb_t *)(PCB_BOTTOM - BLOCK_8KB * i))->in_use = 0;
}

/* file_descriptor_init
 * initialize file descriptor in given pcb
 * Input: pcb - address of pcb
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: file descriptor in given pcb is initialized
 */
int32_t file_descriptor_init(pcb_t* pcb)
{
    int i;      /* variable for iteration */
    // check failure
    if (pcb == NULL)
        return -1;
    // loop through fd array and initialize each fd entry
    for (i=0; i<MAX_OPENNING_FILE; i++)
    {
        pcb->fd_table[i].file_operations_table_pointer = NULL;
        pcb->fd_table[i].index_node_number = 0;
        pcb->fd_table[i].file_position = 0;
        pcb->fd_table[i].flags = 0;
    }
    return 0;
}

/* allocate_pcb
 * allocate and initialize an unused pcb for a new task
 * Input: none
 * Output: none
 * Return: pid for success, -1 for failure
 * Side Effect: none
 */
int32_t allocate_pcb()
{
    int i;      /* variable for iteration */
    pcb_t* new;

    /* iterate through all pcb the check availbility */
    for (i = 0; i < NUM_PCB; i++)
    {
        if (((pcb_t *)(PCB_BOTTOM - BLOCK_8KB * i))->in_use == 0)
        {
            /* find available pcb. Initialize it. */
            new = (pcb_t*)(PCB_BOTTOM - BLOCK_8KB * i);
            new->in_use     = 1;
            new->pid        = i;
            new->parent_pid = 0;
            new->parent_esp0= 0;
            new->parent_ebp0= 0;
            new->esp        = 0;
            new->ebp        = 0;
            new->terminal   = &global_terminal[terminal_show];
            memset((void*) new->arg_buffer, 0, ARG_BUFFER_SIZE);
            file_descriptor_init(new);
            return i;
        }
    }
    return -1;
}

/* get_pcb_by_pid
 * calculate the address of pcb by its pid
 * Input: pid - process ID for target pcb
 * Output: none
 * Return: the address calculated
 * Side Effect: none
 */
pcb_t* get_pcb_by_pid(int32_t pid) 
{
    return (pcb_t*)(PCB_BOTTOM - BLOCK_8KB * pid);
}
