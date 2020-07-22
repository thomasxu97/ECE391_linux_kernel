#include "schedule.h"
#include "paging.h"
#include "system_call.h"
#include "types.h"
#include "terminal.h"
#include "x86_desc.h"
#include "pcb.h"
#include "lib.h"
#include "keyboard.h"

volatile uint32_t terminal_execute = 0; // Current executing terminal
volatile uint32_t next_term = 0;        // Next terminal to execute

/* Context_switch()
 * Function:    do context switch between current term and next term,
 *              and manages current executing terminal
 * Inputs:      none
 * Outputs:     none
 */
void context_switch(){
    
	uint32_t pid;           // PID
	uint8_t* screen_start;  // Screen start location
	uint32_t  counter=0;    // Counter for calculating next terminal

    // If need to open a shell in a new terminal
	if (flag_execute != 0)
	{
		if (flag_execute != terminal_show)
		{
			printf("Warning: New terminal fail to execute!\n");
			return;
		}

        // get the pcb of old process
		pcb_t* old_pcb = get_pcb_by_pid(global_terminal[terminal_execute].last_pid);

		terminal_execute = flag_execute;

        // reset flag
		flag_execute = 0;

		asm volatile(
			"movl %%esp,%0;"
			"movl %%ebp,%1;"
			:"=r"(old_pcb->esp),"=r"(old_pcb->ebp)
			:
			:"esp","ebp"
        );

        execute((const uint8_t*)"shell");

        return;
	}

    // Return if there is no need to use a scheduler to select next terminal
	if (global_terminal[1].num_task == 0 && global_terminal[2].num_task == 0)

		return;

    // Select next terminal to execute
	while (counter < 3) {
		next_term = (counter + terminal_execute + 1) % 3;
		if (global_terminal[next_term].num_task >= 1)
			break;
		counter++;
	}

    // Get the PID
	pid = global_terminal[next_term].last_pid;
    // Create User Level Program Paging
	create_user_level_program_paging(pid);

    // Get PCB of old process and new process(terminal)
	pcb_t* old_pcb = get_pcb_by_pid(global_terminal[terminal_execute].last_pid);
	pcb_t* new_pcb = get_pcb_by_pid(pid);

	if (old_pcb == new_pcb)
		return;

    // Update terminal_execute
	terminal_execute = next_term;

    // Map program paging to video buffer if it is not the current shown terminal
	if(terminal_execute != terminal_show) {
		screen_start = (uint8_t*)_132MB;
		remap((uint32_t)screen_start, (uint32_t)video_buf[terminal_execute]);
	}
	
	// Otherwise map program paging to video memory
	else{
		// vidmap(&screen_start);
		screen_start=(uint8_t*)_132MB;
		remap((uint32_t)screen_start, (uint32_t)VIDEO);
	}

    // Update TSS
	tss.ss0=KERNEL_DS;
	tss.esp0=_8MB-_8KB*pid-4;

    // Save old ESP/EBP
	asm volatile(
		"movl %%esp,%0;"
		"movl %%ebp,%1;"
		:"=r"(old_pcb->esp),"=r"(old_pcb->ebp)
		:
		:"esp","ebp"
        );

    // Load new ESP/EBP
	asm volatile(
		"movl %0,%%esp;"
		"movl %1,%%ebp;"
		:
		:"r"(new_pcb->esp),"r"(new_pcb->ebp)
		:"esp","ebp"
		);

	return;
}

