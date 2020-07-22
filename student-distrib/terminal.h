#ifndef TERMINAL_H 
#define TERMINAL_H

#include "types.h"
#include "keyboard.h"

#define NUM_ROWS		25
#define NUM_COLS		80

#define ATTRIB			0x7
#define NUM_TERMINAL    3

/* Terminal Structure */
typedef struct terminal_struct {
    int term_id;
    int last_pid;
    int num_task;
    // int screen_x;
    // int screen_y;
    keyboard_buffer_t kbuf;
} terminal_t;

// Global terminal structure array
extern terminal_t global_terminal[NUM_TERMINAL];
// Terminal shown id
extern volatile uint32_t terminal_show;

// Buffer helper functions
void terminal_buffer_write(uint8_t c);
void terminal_buffer_backspace();
void terminal_buffer_clear();

// Terminal functions
void terminal_clear();
void terminal_init();
void terminal_print_cp2();

// Syscalls
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
