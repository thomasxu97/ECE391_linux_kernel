#define ASM 1
#include "idt.h"
#include "keyboard.h"
#include "rtc.h"
#include "assembly_linkage.h"
#include "pit.h"

# declaration of labels
.global common_interrupt
.global division_by_zero
.global single_step
.global NMI
.global breakpoint
.global overflow
.global bounds
.global invalid_opcode
.global coprocessor_not_available
.global double_fault
.global coprocessor_segment_overrun
.global invalid_task_state_segment
.global segment_not_present
.global stack_fault
.global general_protection_fault
.global page_fault
.global reserved
.global math_fault
.global alignment_check
.global machine_check
.global SIMD_floatingpoint_exception
.global virtualization_exception
.global control_protection_exception
.global keyboard_handler_s
.global rtc_interrupt_handler_s
.global system_call_handler_s 
.global pit_handler_s

#  common_interrupt
#  Description: assembly part to call print common interrupt
#  Inputs:  none
#  Return Value: none
#  Function: call function division by zero
common_interrupt:   
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_common_interrupt

    popfl
    popal
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret


#  division_by_zero
#  Description: assembly part to call print divisoin by zero
#  Inputs:  none
#  Return Value: none
#  Function: call function division by zero
division_by_zero:   
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_division_by_zero

    popfl
    popal
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret


#  single_step
#  Description: assembly part to call print single step
#  Inputs:  none
#  Return Value: none
#  Function: call function print single step    
single_step:   
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_single_step

    popfl
    popal
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret


#  NMI
#  Description: assembly part to call print NMI
#  Inputs:  none
#  Return Value: none
#  Function: call function print NMI 
NMI:  
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_NMI

    popfl
    popal
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  breakpoint
#  Description: assembly part to call print breakpoint
#  Inputs:  none
#  Return Value: none
#  Function: call function print breakpoint
breakpoint:   
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call   print_breakpoint 

    popfl
    popal     
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  overflow
#  Description: assembly part to call print overflow
#  Inputs:  none
#  Return Value: none
#  Function: call function print overflow
overflow:    
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_overflow

    popfl
    popal     
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  bounds
#  Description: assembly part to call print bounds
#  Inputs:  none
#  Return Value: none
#  Function: call function print bounds
bounds:    
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_bounds

    popfl
    popal     
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  invalid_opcode
#  Description: assembly part to call print_invalid_opcode
#  Inputs:  none
#  Return Value: none
#  Function: call function print_invalid_opcode
invalid_opcode:    
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_invalid_opcode

    popfl
    popal     
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  coprocessor_not_available
#  Description: assembly part to call print coprocessor_not_available
#  Inputs:  none
#  Return Value: none
#  Function: call function print coprocessor_not_available 
coprocessor_not_available:   
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_coprocessor_not_available

    popfl
    popal     
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  double_fault
#  Description: assembly part to call print double_fault
#  Inputs:  none
#  Return Value: none
#  Function: call function print double_fault
double_fault:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_double_fault

    popal
    popfl 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  coprocessor_segment_overrun
#  Description: assembly part to call print coprocessor_segment_overrun
#  Inputs:  none
#  Return Value: none
#  Function: call function print coprocessor_segment_overrun
coprocessor_segment_overrun:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_coprocessor_not_available

    popal
    popfl 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  invalid_task_state_segment
#  Description: assembly part to call print invalid_task_state_segment
#  Inputs:  none
#  Return Value: none
#  Function: call function print invalid_task_state_segment
invalid_task_state_segment:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_invalid_task_state_segment

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  segment_not_present
#  Description: assembly part to call print segment_not_present
#  Inputs:  none
#  Return Value: none
#  Function: call function print segment_not_present
segment_not_present:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_segment_not_present

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  stack_fault
#  Description: assembly part to call print stack_fault
#  Inputs:  none
#  Return Value: none
#  Function: call function print stack_fault
stack_fault:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_stack_fault

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  general_protection_fault
#  Description: assembly part to call print general_protection_fault
#  Inputs:  none
#  Return Value: none
#  Function: call function print general_protection_fault
general_protection_fault:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_general_protection_fault

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret


#  page_fault
#  Description: assembly part to call print page_fault
#  Inputs:  none
#  Return Value: none
#  Function: call function print page_fault
page_fault:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_page_fault
    # addl $4,%esp

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  reserved
#  Description: assembly part to call print reserved
#  Inputs:  none
#  Return Value: none
#  Function: call function print reserved
reserved:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_reserved

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  math_fault
#  Description: assembly part to call print math_fault
#  Inputs:  none
#  Return Value: none
#  Function: call function print math_fault
math_fault:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_math_fault

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  alignment_check
#  Description: assembly part to call print alignment_check
#  Inputs:  none
#  Return Value: none
#  Function: call function print alignment_check
alignment_check:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_alignment_check

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret


#  machine_check
#  Description: assembly part to call print machine_check
#  Inputs:  none
#  Return Value: none
#  Function: call function print machine_check
machine_check:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_machine_check

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  SIMD_floatingpoint_exception
#  Description: assembly part to call print SIMD_floatingpoint_exception
#  Inputs:  none
#  Return Value: none
#  Function: call function print SIMD_floatingpoint_exception
SIMD_floatingpoint_exception:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_SIMD_floatingpoint_exception

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret

#  virtualization_exception
#  Description: assembly part to call print virtualization_exception
#  Inputs:  none
#  Return Value: none
#  Function: call function print virtualization_exception
virtualization_exception:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_virtualization_exception

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  control_protection_exception
#  Description: assembly part to call print control_protection_exception
#  Inputs:  none
#  Return Value: none
#  Function: call function print control_protection_exception
control_protection_exception:

    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    print_control_protection_exception

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  keyboard_handler_s
#  Description: assembly part to call print keyboard_handler_s
#  Inputs:  none
#  Return Value: none
#  Function: call function print keyboard_handler_s
keyboard_handler_s:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    keyboard_handler

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret



#  rtc_interrupt_handler_s
#  Description: assembly part to call print rtc_interrupt_handler_s
#  Inputs:  none
#  Return Value: none
#  Function: call function print rtc_interrupt_handler_s
rtc_interrupt_handler_s:
    # pushl   %eax
    # pushl   %ebx
    # pushl   %ecx
    # pushl   %edx
    # pushl   %edi
    # pushl   %esi
    # cld
    pushal
    pushfl

    call    rtc_interrupt_handler

    popfl
    popal 
    # popl    %esi
    # popl    %edi
    # popl    %edx
    # popl    %ecx
    # popl    %ebx
    # popl    %eax
    iret

pit_handler_s:
    pushl   %eax
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %edi
    pushl   %esi

    call pit_handler

    popl    %esi
    popl    %edi
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %eax
    iret


#  system_call_handler_s
#  Description: assembly part to call print system_call_handler_s
#  Inputs:  none
#  Return Value: none
#  Function: call function print system_call_handler_s
system_call_handler_s:
    pushl   %fs
    pushl   %es
    pushl   %ds
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %edx
    pushl   %ecx
    pushl   %ebx

    pushl   %edx
    pushl   %ecx
    pushl   %ebx

    cmpl    $1,%eax
    jl      fail

    cmpl    $10,%eax
    jg      fail

    call    *jump_table(,%eax,4)
    jmp     return
fail:
    movl    $-1,%eax
return:
    addl    $12,%esp

    popl    %ebx
    popl    %ecx
    popl    %edx
    popl    %esi
    popl    %edi
    popl    %ebp
    popl    %ds
    popl    %es
    popl    %fs
    iret    

jump_table:
    .long   0, halt, execute, read, write, open, close, getargs,vidmap,set_handler,sigreturn
    # ride

