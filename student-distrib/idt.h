#ifndef ASM

// initialization for idt
extern void idt_init();

// function for print common interrupt
extern void print_common_interrupt();

// function for print division by zero
extern void print_division_by_zero();

// function for print single step
extern void print_single_step();

// function for print NMI
extern void print_NMI();

// function for print breakpoint
extern void print_breakpoint();

// function for print overflow
extern void print_overflow();

// function for print bounds
extern void print_bounds();

// function for print invalid opcode
extern void print_invalid_opcode();

// function for print coprocessor not available
extern void print_coprocessor_not_available();

// function for print double fault
extern void print_double_fault();

// function for print coprocessor segment overrun
extern void print_coprocessor_segment_overrun();

// function for print invalid task state segment
extern void print_invalid_task_state_segment();

// function for print segment not present
extern void print_segment_not_present();

// function for print stack fault
extern void print_stack_fault();

// function for print general protection fault
extern void print_general_protection_fault();

// function for print page fault
extern void print_page_fault();

// function for print reserved
extern void print_reserved();

// function for print math fault
extern void print_math_fault();

// function for print alignment check
extern void print_alignment_check();
    
// function for print machine check
extern void print_machine_check();
   
// function for print SIMD fliatingpoint exception
extern void print_SIMD_floatingpoint_exception();

// function for print virtualization exception
extern void print_virtualization_exception();

// function for print control protection exception
extern void print_control_protection_exception();
 
//temporaray for system call
// extern void system_call_handler();

#endif
