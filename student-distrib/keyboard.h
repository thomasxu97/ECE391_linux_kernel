#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#ifndef ASM

#define KEYBOARD_IRQ    1

#define MAX_KBUF_SIZE	128

#define NUM_ROWS        25
#define NUM_COLS        80

extern volatile int flag_execute;

typedef struct keyboard_buffer {
    int8_t buffer[MAX_KBUF_SIZE];
    int32_t curr_size;
    int32_t max_size;
} keyboard_buffer_t;

// init of keyboard
int32_t keyboard_init();
// keyboard interrupt handler
void keyboard_handler();
// init keyboard buffer
void keyboard_buffer_init();
// keyboard buffer backspace
void keyboard_buffer_backspace();
// keyboard buffer clear
void keyboard_buffer_clear();

#endif

#endif
