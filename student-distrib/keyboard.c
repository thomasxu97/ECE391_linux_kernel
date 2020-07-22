#include "keyboard.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "system_call.h"
#include "pcb.h"
#include "schedule.h"

static int mode;            // 0 for ncns, 1 for ncs, 2 for cns, 3 for cs
static int mode_caps;       // 0 for not active
static int mode_ctrl;       // 0 for not pressed
static int mode_alt;        // 0 for not pressed

volatile int flag_execute = 0;

static uint8_t scancode2char[4][128] =
{
    // no capslock, no shift
    {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'},
    // no capslock, shift
    {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'},
    // capslock, no shift
    {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', '\0', 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'},
    // capslock, shift
    {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'}
};

/* keyboard_buffer_init()
 * Inputs: none
 * Outputs: none
 * Function: init keyboard buffer
 */
void keyboard_buffer_init() {
    int i = 0;
    int j = 0;
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < MAX_KBUF_SIZE; i++) {
            global_terminal[j].kbuf.buffer[i] = '\0';
        }

        global_terminal[j].kbuf.buffer[MAX_KBUF_SIZE - 1] = '\n';
        global_terminal[j].kbuf.curr_size = 0;
        global_terminal[j].kbuf.max_size = MAX_KBUF_SIZE;

        mode      = 0;
        mode_caps = 0;
        mode_ctrl = 0;
        mode_alt  = 0;
    }
}

/* keyboard_buffer_write(uint8_t c)
 * Inputs: ASCII char
 * Outputs: none
 * Function: writes c to keyboard buffer
 */
void keyboard_buffer_write(uint8_t c) {
    if (global_terminal[terminal_show].kbuf.curr_size < global_terminal[terminal_show].kbuf.max_size - 1)
    {
        global_terminal[terminal_show].kbuf.buffer[global_terminal[terminal_show].kbuf.curr_size] = c;
        global_terminal[terminal_show].kbuf.curr_size++;
    }
}

/* terminal_buffer_backspace()
 * Inputs: none
 * Outputs: none
 * Function: delete one character in keyboard buffer
 */
void keyboard_buffer_backspace() {
    // pop char from buffer
    if (global_terminal[terminal_show].kbuf.curr_size > 0)
    {
        global_terminal[terminal_show].kbuf.buffer[global_terminal[terminal_show].kbuf.curr_size - 1] = '\0';
        global_terminal[terminal_show].kbuf.curr_size--;
        if (get_screen_x() == 0)
        {
            if (get_screen_y() == 0)
            {
                return;
            }
            set_cursor(NUM_COLS, get_screen_y() - 1);
        }
        set_cursor(get_screen_x() - 1, get_screen_y());
        smart_putc(' ');
        set_cursor(get_screen_x() - 1, get_screen_y());
    }
}

/* keyboard_buffer_clear()
 * Inputs: none
 * Outputs: none
 * Function: clears keyboard buffer
 */
void keyboard_buffer_clear() {
    int i = 0;
    for (i = 0; i < global_terminal[terminal_show].kbuf.max_size; i++)
    {
        global_terminal[terminal_show].kbuf.buffer[i] = '\0';
    }
    global_terminal[terminal_show].kbuf.curr_size = 0;
}

/* int not_control_char();
 * Inputs: uint8_t s
 * Outputs: int is_char
 * Function: checks if a scancode corresponds to a printable character
 */
int not_control_char(uint8_t s) {
    int ret = 0;
    // '1' ~ '0'
    if (s >= 0x02 && s <= 0x0B)
        ret |= 1;
    // 'Q' ~ ']'
    else if (s >= 0x10 && s <= 0x1B)
        ret |= 1;
    // 'A' ~ '`'
    else if (s >= 0x1E && s <= 0x29)
        ret |= 1;
    // '\' ~ '/'
    else if (s >= 0x2B && s <= 0x35)
        ret |= 1;
    // '*' and ' '
    else if (s == 0x37 || s == 0x39)
        ret |= 1;
    // return
    return ret;
}

/* void keyboard_init();
 * Description: initialization of keyboard
 * Inputs:  none
 * Return Value: none
 * Function: call function enable_irq() in i8259.c */
int32_t keyboard_init() {
    enable_irq(KEYBOARD_IRQ);
    keyboard_buffer_init();
    return 0;
}

/* void keyboard_handler();
 * Description: interrupt handler of keyboard
 * Inputs:  none
 * Return Value: none
 * Function: call function cli() in lib.h
 *           call function inb() in lib.h
 *           call function printf() in lib.c
 *           call function send_eoi() in i8259.c
 *           call function sti() in lib.h */
void keyboard_handler() {
    int exec_term = -1;
    uint8_t scancode;
    cli();
    scancode = inb(0x60);
    switch (scancode)
    {
        // LSHIFT/RSHIFT DOWN
        case 0x2A:
        case 0x36:
            mode += 1;
            break;
        // LSHIFT/RSHIFT UP
        case 0xAA:
        case 0xB6:
            mode -= 1;
            break;
        // CAPSLOCK
        case 0x3A:
            if (mode_caps) {
                mode_caps -= 1;
                mode -= 2;
            }
            else {
                mode_caps += 1;
                mode += 2;
            }
            break;
        // Ctrl
        case 0x1D:
            mode_ctrl += 1;
            break;
        case 0x9D:
            mode_ctrl -= 1;
            break;
        // Alt
        case 0x38:
            mode_alt = 1;
            break;
        case 0xB8:
            mode_alt = 0;
            break;
        // Backspace
        case 0x0E:
            keyboard_buffer_backspace();
            // terminal_write(0, (char *)kbuf.buffer, MAX_KBUF_SIZE);
            break;
        // Enter
        case 0x1C:
            keyboard_buffer_write(scancode2char[mode][scancode]);
            smart_putc(scancode2char[mode][scancode]);
            break;
        // Other keys
        default:
            if (scancode >= 0x80)
                break;
            if (mode_ctrl == 0 && mode_alt == 0) {
                // if a char is not control char (ctrl, shift, backspace, capslock, enter)
                if (not_control_char(scancode))
                {
                    if (global_terminal[terminal_show].kbuf.curr_size >= MAX_KBUF_SIZE - 1)
                        break;
                    keyboard_buffer_write(scancode2char[mode][scancode]);
                    smart_putc(scancode2char[mode][scancode]);
                    set_cursor(get_screen_x(), get_screen_y());
                }
            }
            else if (mode_ctrl == 1 && mode_alt == 0) {
                // CTRL + L
                if (scancode == 0x26) {
                    keyboard_buffer_clear();
                    terminal_clear();
                }
                // CTRL + D ARROW
                else if (scancode == 0x50) {
                    terminal_scroll();
                }
            }
            else if (mode_ctrl == 0 && mode_alt == 1) {
                // ALT + F1, switch to terminal 0
                if (scancode == 0x3B && terminal_show != 0) {
                    set_screen(0);
                    terminal_show = 0;
                    exec_term = 0;
                }
                // ALT + F2, switch to terminal 1
                else if (scancode == 0x3C && terminal_show != 1) {
                    set_screen(1);
                    terminal_show = 1;
                    exec_term = 1;
                }
                // ALT + F3, switch to terminal 2
                else if (scancode == 0x3D && terminal_show != 2) {
                    set_screen(2);
                    terminal_show = 2;
                    exec_term = 2;
                }
            }
            else if (mode_ctrl == 1 && mode_alt == 1) { }
	  }
    if (exec_term != -1 && global_terminal[exec_term].num_task == 0){
        flag_execute = exec_term;
    }
    sti();
    send_eoi(KEYBOARD_IRQ);
    return;
}
