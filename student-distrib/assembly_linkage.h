#ifndef ASM

// assembly part to call print common interrupt
extern void common_interrupt();

// assembly part to call print divisoin by zero
extern void division_by_zero();

// assembly part to call print single step
extern void single_step();

// assembly part to call print NMI
extern void NMI();

// assembly part to call print breakpoint
extern void breakpoint();

// assembly part to call print overflow
extern void overflow();

// assembly part to call print bounds
extern void bounds();

// assembly part to call print_invalid_opcode
extern void invalid_opcode();

// assembly part to call print coprocessor_not_available
extern void coprocessor_not_available();

// assembly part to call print double_fault
extern void double_fault();

// assembly part to call print coprocessor_segment_overrun
extern void coprocessor_segment_overrun();

// assembly part to call print invalid_task_state_segment
extern void invalid_task_state_segment();

// assembly part to call print segment_not_present
extern void segment_not_present();

// assembly part to call print stack_fault
extern void stack_fault();

// assembly part to call print general_protection_fault
extern void general_protection_fault();

// assembly part to call print page_fault
extern void page_fault();

// assembly part to call print reserved
extern void reserved();

// assembly part to call print math_fault
extern void math_fault();

// assembly part to call print alignment_check
extern void alignment_check();

// assembly part to call print machine_check
extern void machine_check();

// assembly part to call print SIMD_floatingpoint_exception
extern void SIMD_floatingpoint_exception();

// assembly part to call print virtualization_exception
extern void virtualization_exception();

// assembly part to call print control_protection_exception
extern void control_protection_exception();

// assembly part to call print keyboard_handler_s
extern void keyboard_handler_s();

// assembly part to call print rtc_interrupt_handler_s
extern void rtc_interrupt_handler_s();

// assembly part to call print system_call_handler_s
extern void system_call_handler_s();

// assembly part to call pit_handler
extern void pit_handler_s();

#endif
