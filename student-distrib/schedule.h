#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "types.h"

#define _8MB     0x800000
#define _4MB     0x400000
#define _8KB     0x2000

extern volatile uint32_t terminal_execute;
extern volatile uint32_t next_term;

extern void context_switch();

#endif 
