/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask	= 0xFF; /* IRQs 0-7  */
uint8_t slave_mask	= 0xFF;  /* IRQs 8-15 */

/*
 * i8259_init()
 * Inputs: none
 * Outputs: none
 * Side Effects: Initialize the 8259 PIC
 */
void i8259_init(void) {

	cli();
	// Save mask
	// uint8_t save_master=inb(MASTER_8259_PORT+1);
	// uint8_t save_slave=inb(SLAVE_8259_PORT+1);

	// Mask all input
	outb(0xFF, MASTER_8259_PORT + DATA_PORT_OFFSET);
	outb(0xFF, SLAVE_8259_PORT + DATA_PORT_OFFSET);

	// Send ICWs for master
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_PORT + DATA_PORT_OFFSET);
	outb(ICW3_MASTER, MASTER_8259_PORT + DATA_PORT_OFFSET);
	outb(ICW4, MASTER_8259_PORT + DATA_PORT_OFFSET);

	// Set ICWs for slave
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_8259_PORT + DATA_PORT_OFFSET);
	outb(ICW3_SLAVE, SLAVE_8259_PORT + DATA_PORT_OFFSET);
	outb(ICW4, SLAVE_8259_PORT + DATA_PORT_OFFSET);

	// Restore mask
	// outb(save_master,MASTER_8259_PORT+1);
	// outb(save_slave,SLAVE_8259_PORT+1);

	sti();
	return;
}

/* 
 * enable_irq(irq_num);
 * Inputs: irq_num
 * Outputs: none
 * Side Effects: Send data to ports to enable IRQ line on PIC
 * Enable (unmask) the specified IRQ
 */
void enable_irq(uint32_t irq_num) {

	// Set active HIGH mask to 0x01, shift later
	uint8_t mask=0x01;
	int i;

	// Invalid IRQ#
	if (irq_num > IRQ_MAX || irq_num < 0) {
		printf("Invalid irq_num!\n");
	}

	// Master IRQ
	else if (irq_num <= MASTER_IRQ_MAX) {
		// Shift mask
		for(i = 0; i < irq_num; i++, mask <<= 1);
		// Flip mask to active LOW
		mask =~ mask;
		// Combine current bitmask with master_mask value
		master_mask &= mask;
		// Set mask
		outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
	}

	// Slave IRQ
	else {
		// Calculate IRQ# on slave
		irq_num -= SLAVE_IRQ_OFFSET;
		// Shift mask
		for(i = 0; i < irq_num; i++, mask <<= DATA_PORT_OFFSET);
		// Flip mask to active LOW
		mask =~ mask;
		// Combine current bitmask with slave_mask value
		slave_mask &= mask;
		// Set slave mask
		outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);
		// Enable IRQ2 on master
		uint8_t irq2_mask = 0xFB;
		master_mask &= irq2_mask;
		outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
	}
	return;
}

/*
 * disable_irq(irq_num);
 * Inputs: irq_num
 * Outputs: none
 * Side Effects: Send data to ports to disable IRQ line on PIC
 * Disable (mask) the specified IRQ
 */
void disable_irq(uint32_t irq_num) {

	// Set current mask to 0x01, shift later
	uint8_t mask = 0x01;
	int i;

	// Invalid IRQ#
	if (irq_num > IRQ_MAX || irq_num < 0) {
		printf("Invalid irq_num!\n");
	}

	// Master IRQ
	else if (irq_num <= MASTER_IRQ_MAX) {
		// Shift mask
		for(i = 0; i < irq_num; i++, mask <<= 1);
		// Combine current bitmask with master_mask value
		master_mask |= mask;
		// Set mask
		outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
	}

	// Slave IRQ
	else {
		// Calculate IRQ# on slave
		irq_num -= SLAVE_IRQ_OFFSET;
		// Shift mask
		for(i = 0; i < irq_num; i++, mask <<= 1);
		// Combine current bitmask with slave_mask value
		slave_mask |= mask;
		// Set mask
		outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);
	}
	return;
}

/*
 * send_eoi(irq_num);
 * Inputs: irq_num
 * Outputs: none
 * Side Effects: Send EOI to ports on PIC
 * Send end-of-interrupt signal for the specified IRQ
 */
void send_eoi(uint32_t irq_num) {

	// Invalid IRQ#
	if (irq_num > IRQ_MAX || irq_num < 0) {
		printf("Invalid irq_num!\n");
	}

	// Master
	else if (irq_num <= MASTER_IRQ_MAX) {
		outb(EOI + irq_num, MASTER_8259_PORT);						// Send EOI to master port
	}

	// Slave
	else {
		outb(EOI + irq_num - SLAVE_IRQ_OFFSET, SLAVE_8259_PORT);	// Send EOI to slave port
		outb(EOI + 2, MASTER_8259_PORT);							// Send EOI for IRQ2 to master port
	}

	return;
}
