#include "idt.h"
#include "keyboard.h"
#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "assembly_linkage.h"
#include "types.h"

#define VECTOR_DIVISION_BY_ZERO             0x00
#define VECTOR_SINGLE_STEP                  0x01
#define VECTOR_NMI                          0x02
#define VECTOR_BREAKPOINT                   0x03
#define VECTOR_OVERFLOW                     0x04
#define VECTOR_BOUNDS                       0x05
#define VECTOR_INVALID_OPCODE               0x06
#define VECTOR_COPROCESSOR_NOT_AVAILABLE    0x07
#define VECTOR_DOUBLE_FAULT                 0x08
#define VECTOR_COPROCESSOR_SEGMENT_OVERRUN  0x09
#define VECTOR_INVALID_TASK_STATE_SEGMENT   0x0A
#define VECTOR_SEGMENT_NOT_PRESENT          0x0B
#define VECTOR_STACK_FAULT                  0x0C
#define VECTOR_GENERAL_PROTECTION_FAULT     0x0D
#define VECTOR_PAGE_FAULT                   0x0E
#define VECTOR_RESERVED                     0x0F
#define VECTOR_MATH_FAULT                   0x10
#define VECTOR_ALIGNMENT_CHECK              0x11
#define VECTOR_MACHINE_CHECK                0x12
#define VECTOR_SIMD_FLOATINGPOINT_EXCEPTION 0x13
#define VECTOR_VIRTUALIZATION_EXCEPTION     0x14
#define VECTOR_CONTROL_PROTECTION_EXCEPTION 0x15
#define VECTOR_KEYBOARD_HANDLER             0x21
#define VECTOR_RTC_HANDLER                  0x28
#define VECTOR_SYSCALL                      0x80
#define VECTOR_PIT                          0x20

#define VECTOR_EXCEPTION_MAX                32
#define VECTOR_MAX                          256

/* void print_common_interrupt();
 * Description: function for print common interrupt
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_common_interrupt() {
    cli();
    printf("There is an interrupt!\n");
    while(1);
    sti();
}

/* void print_division_by_zero();
 * Description: function for print division by zero
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_division_by_zero() {
    cli();
    printf("Division by zero!\n");
    while(1);
    sti();
}

/* void print_single_step();
 * Description: function for print single step
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_single_step() {
    cli();
    printf("Single step interrupt!\n");
    while(1);
    sti();
}

/* void print_NMI();
 * Description: function for print NMI
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_NMI() {
    cli();
    printf("None mask interrupt!\n");
    while(1);
    sti();
}

/* void print_breakpoint();
 * Description: function for print breakpoint
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_breakpoint() {
    cli();
    printf("Breakpoint!\n");
    while(1);
    sti();
}

/* void print_overflow();
 * Description: function for print overflow
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_overflow() {
    cli();
    printf("Overflow!\n");
    while(1);
    sti();
}

/* void print_bounds();
 * Description: function for print bounds
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_bounds() {
    cli();
    printf("Bounds!\n");
    while(1);
    sti();
}

/* void print_invalid_opcode();
 * Description: function for print invalid opcode
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_invalid_opcode() {
    cli();
    printf("Invalid opcode!\n");
    while(1);
    sti();
}

/* void print_coprocessor_not_available();
 * Description: function for print coprocessor not available
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_coprocessor_not_available() {
    cli();
    printf("Coprocessor not available!\n");
    while(1);
    sti();
}

/* void print_double_fault();
 * Description: function for print double fault
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_double_fault() {
    cli();
    printf("Double fault!\n");
    while(1);
    sti();
}

/* void print_coprocessor_segment_overrun();
 * Description: function for print coprocessor segment overrun
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_coprocessor_segment_overrun() {
    cli();
    printf("Coprocessor Segment Overrun!\n");
    while(1);
    sti();
}

/* void print_invalid_task_state_segment();
 * Description: function for print invalid task state segment
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_invalid_task_state_segment() {
    cli();
    printf("Invalid Task State Segment!\n");
    while(1);
    sti();
}

/* void print_segment_not_present();
 * Description: function for print segment not present
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_segment_not_present() {
    cli();
    printf("Segment not present!\n");
    while(1);
    sti();
}

/* void print_stack_fault();
 * Description: function for print stack fault
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_stack_fault() {
    cli();
    printf("Stack Fault!\n");
    while(1);
    sti();
}

/* void print_general_protection_fault();
 * Description: function for print general protection fault
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_general_protection_fault() {
    cli();
    printf("General protection fault!\n");
    while(1);
    sti();
}

/* void print_page_fault();
 * Description: function for print page fault
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_page_fault() {
    cli();
    printf("Page fault!\n");
    while(1);
    sti();
}

/* void print_reserved();
 * Description: function for print reserved
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_reserved() {
    cli();
    printf("Reserved!\n");
    while(1);
    sti();
}

/* void print_math_fault();
 * Description: function for print math fault
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_math_fault() {
    cli();
    printf("Math Fault!\n");
    while(1);
    sti();
}

/* void print_alignment_check();
 * Description: function for print alignment check
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_alignment_check() {
    cli();
    printf("Alignment Check!\n");
    while(1);
    sti();
}

/* void print_machine_check();
 * Description: function for print machine check
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_machine_check() {
    cli();
    printf("Machine Check!\n");
    while(1);
    sti();
}

/* void print_SIMD_floatingpoint_exception();
 * Description: function for print SIMD fliatingpoint exception
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_SIMD_floatingpoint_exception() {
    cli();
    printf("SIMD Floating-Point Exception!\n");
    while(1);
    sti();
}

/* void print_virtualization_exception();
 * Description: function for print virtualization exception
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_virtualization_exception() {
    cli();
    printf("Virtualization Exception!\n");
    while(1);
    sti();
}

/* void print_control_protection_exception();
 * Description: function for print control protection exception
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
void print_control_protection_exception() {
    cli();
    printf("Control Protection Exception!\n");
    while(1);
    sti();
}

/* void system_call_handler();
 * Description: temporaray for system call
 * Inputs:  none
 * Return Value: none
 * Function: call function printf() in lib.c */
// void system_call_handler() {
//     cli();
//     printf("System call!\n");
//     while(1);
//     sti();
// }


/* void idt_init();
 * Description: initialization of idt
 * Inputs:  none
 * Return Value: none
 * Function: call function SET_IDT_ENTRY in x86_desc.h */
void idt_init() {

    int i;
    // for smaller than 32 part, set value to each bit
    for (i = 0; i < VECTOR_EXCEPTION_MAX; i++) {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4    = 0;
        idt[i].reserved3    = 1;
        idt[i].reserved2    = 1;
        idt[i].reserved1    = 1;
        idt[i].size         = 1;
        idt[i].reserved0    = 0;
        idt[i].dpl          = 0;
        idt[i].present      = 1;
    }
 
    // for larger than 32 part, set value to each bit
    for (i = 32; i < VECTOR_MAX; i++) {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4    = 0;
        idt[i].reserved3    = 0;
        idt[i].reserved2    = 1;
        idt[i].reserved1    = 1;
        idt[i].size         = 1;
        idt[i].reserved0    = 0;
        idt[i].dpl          = 0;
        idt[i].present      = 1;
    }

    // Set DPL for system call to 3
    idt[0x80].dpl = 3;
    idt[0x80].reserved3=1;

    // iterate and sets runtime parameters for an IDT entry
    for(i = 0; i < VECTOR_MAX; i++) {
        SET_IDT_ENTRY(idt[i], common_interrupt);
    }
    SET_IDT_ENTRY(idt[VECTOR_DIVISION_BY_ZERO], division_by_zero);
    SET_IDT_ENTRY(idt[VECTOR_SINGLE_STEP], single_step);
    SET_IDT_ENTRY(idt[VECTOR_NMI], NMI);
    SET_IDT_ENTRY(idt[VECTOR_BREAKPOINT], breakpoint);
    SET_IDT_ENTRY(idt[VECTOR_OVERFLOW], overflow);
    SET_IDT_ENTRY(idt[VECTOR_BOUNDS], bounds);
    SET_IDT_ENTRY(idt[VECTOR_INVALID_OPCODE], invalid_opcode);
    SET_IDT_ENTRY(idt[VECTOR_COPROCESSOR_NOT_AVAILABLE], coprocessor_not_available);
    SET_IDT_ENTRY(idt[VECTOR_DOUBLE_FAULT], double_fault);
    SET_IDT_ENTRY(idt[VECTOR_COPROCESSOR_SEGMENT_OVERRUN], coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[VECTOR_INVALID_TASK_STATE_SEGMENT], invalid_task_state_segment);
    SET_IDT_ENTRY(idt[VECTOR_SEGMENT_NOT_PRESENT], segment_not_present);
    SET_IDT_ENTRY(idt[VECTOR_STACK_FAULT], stack_fault);
    SET_IDT_ENTRY(idt[VECTOR_GENERAL_PROTECTION_FAULT], general_protection_fault);
    SET_IDT_ENTRY(idt[VECTOR_PAGE_FAULT], page_fault);
    SET_IDT_ENTRY(idt[VECTOR_RESERVED], reserved);
    SET_IDT_ENTRY(idt[VECTOR_MATH_FAULT], math_fault);
    SET_IDT_ENTRY(idt[VECTOR_ALIGNMENT_CHECK], alignment_check);
    SET_IDT_ENTRY(idt[VECTOR_MACHINE_CHECK], machine_check);
    SET_IDT_ENTRY(idt[VECTOR_SIMD_FLOATINGPOINT_EXCEPTION], SIMD_floatingpoint_exception);
    SET_IDT_ENTRY(idt[VECTOR_VIRTUALIZATION_EXCEPTION], virtualization_exception);
    SET_IDT_ENTRY(idt[VECTOR_CONTROL_PROTECTION_EXCEPTION], control_protection_exception);
    SET_IDT_ENTRY(idt[VECTOR_KEYBOARD_HANDLER], keyboard_handler_s);
    SET_IDT_ENTRY(idt[VECTOR_RTC_HANDLER], rtc_interrupt_handler_s);
    SET_IDT_ENTRY(idt[VECTOR_SYSCALL],system_call_handler_s);
    SET_IDT_ENTRY(idt[VECTOR_PIT],pit_handler_s);
}




