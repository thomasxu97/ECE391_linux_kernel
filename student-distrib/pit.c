#include "pit.h"
#include "i8259.h"
#include "schedule.h"
#include "lib.h"

#define DIV_SETTING 1193180
#define PIT_PORT_1  0x43
#define PIT_PORT_2  0x40
#define PIT_MASK    0xFF
#define PIT_IRQ     0

/* pit_init(int frequency)
 * Function: initialize pit
 * Input: frequency
 * Output: none
 */
void pit_init(int frequency){

	int divisor= DIV_SETTING / frequency;
	outb(divisor, PIT_PORT_1);
	outb(divisor & PIT_MASK, PIT_PORT_2);   // set low byte of divisor
	outb(divisor >> 8, PIT_PORT_2);   // set high byte of divisor

	enable_irq(PIT_IRQ);
	// disable_irq(0);
}

/* pit_handler()
 * Function: PIT Interrupt Handler
 * Input: none
 * Output: none
 */
void pit_handler(){
	cli();
	send_eoi(PIT_IRQ);
	context_switch();
}

