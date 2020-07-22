#include "terminal.h"
// #include "keyboard.h"
#include "system_call.h"
#include "lib.h"
#include "schedule.h"

terminal_t global_terminal[NUM_TERMINAL];

volatile uint32_t terminal_show;          // current showing terminal

/* terminal_clear()
 * Inputs: none
 * Outputs: none
 * Return value: none
 * Function: calls functions in lib.c to clear terminal and reset cursor
 */
void terminal_clear() {
    clear();            // clear screen
    set_cursor(0, 0);   // reset cursor to (0, 0)
}

/* terminal_init()
 * Initialize the terminal structure
 * Input: none
 * Output: none
 * Return: none
 * Side effect: terminal stucture is initialized
 */
void terminal_init() {
    int32_t i;          // iteration variable
    int32_t j;          // ^

    terminal_show = 0;

    // initialize terminal structure
    for (i = 0; i < NUM_TERMINAL; i++)
    {
        global_terminal[i].term_id = i;
        global_terminal[i].last_pid = 0;
        global_terminal[i].num_task = 0;
        global_terminal[i].kbuf.curr_size = 0;
        global_terminal[i].kbuf.max_size = MAX_KBUF_SIZE;

        for (j = 0; j < MAX_KBUF_SIZE; j++) {
            global_terminal[i].kbuf.buffer[j] = '\0';
        }

        global_terminal[i].kbuf.buffer[MAX_KBUF_SIZE - 1] = '\n';
        global_terminal[i].kbuf.curr_size = 0;
        global_terminal[i].kbuf.max_size = MAX_KBUF_SIZE;

        // global_terminal[i].screen_x = 0;
        // global_terminal[i].screen_y = 0;
    }
}

/* terminal_open()
 * Inputs: none
 * Outputs: none
 * Function: Opens a terminal
 */
int32_t terminal_open(const uint8_t* filename) {
    // clear();               // clear screen
    // set_cursor(0, 0);      // init cursor

    return 0;
}

/* terminal_close()
 * Inputs: none
 * Outputs: none
 * Function: Closes a terminal
 */
int32_t terminal_close(int32_t fd) {
    return 0;
}

/* terminal_read()
 * Inputs: fd, buf, nbytes
 * Outputs: # of bytes read
 * Function: read whatever in keyboard buffer to buf, newline terminated
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    // Wait for keyboard
    while (1)
    {
        if (global_terminal[terminal_show].kbuf.curr_size > 0 && global_terminal[terminal_show].kbuf.buffer[global_terminal[terminal_show].kbuf.curr_size-1] == '\n')
            break;
    }

    // Validate things
    if (!buf)
        return -1;
    if (nbytes > global_terminal[terminal_show].kbuf.curr_size)
    {
        if (global_terminal[terminal_show].kbuf.curr_size == MAX_KBUF_SIZE - 1)
            nbytes = MAX_KBUF_SIZE;
        else
            nbytes = global_terminal[terminal_show].kbuf.curr_size;
    }

    // Copy from kbuf to buf
    int i = 0;
    int j = 0;
    for (i = 0; i < nbytes; i++)
    {
        if (global_terminal[terminal_show].kbuf.buffer[i] == '\n')
        {
            // all char besides last are not '\n'
            if (i == MAX_KBUF_SIZE - 1)
            {
                for (j = 0; j < MAX_KBUF_SIZE; j++)
                {
                    global_terminal[terminal_show].kbuf.buffer[j] = '\0';
                }
                global_terminal[terminal_show].kbuf.buffer[MAX_KBUF_SIZE - 1] = '\n';
                global_terminal[terminal_show].kbuf.curr_size = 0;
            }
            // char before last is '\n'
            else
            {
                int prev_size = global_terminal[terminal_show].kbuf.curr_size;
                for (j = 0; j < prev_size; j++)
                {
                    if (j < prev_size - i - 1)
                        global_terminal[terminal_show].kbuf.buffer[j] = global_terminal[terminal_show].kbuf.buffer[j + i + 1];
                    else
                        global_terminal[terminal_show].kbuf.buffer[j] = '\0';
                }
                global_terminal[terminal_show].kbuf.curr_size = prev_size - i - 1;
            }
            return i;
        }
        else
            ((char *)buf)[i] = global_terminal[terminal_show].kbuf.buffer[i];
    }

    // Return bytes read
    return -1;
}

/* terminal_write()
 * Inputs: fd, buf, nbytes
 * Outputs: # bytes written
 * Function: writes whatever in buf to terminal, nbytes total
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	// init bytes read
	int i;
	// print to screen
	if (terminal_execute == terminal_show)
	{
		for (i = 0; i < nbytes; i++){
			smart_putc(((char *)buf)[i]);
			set_cursor(get_screen_x(), get_screen_y());
		}
	}
	else
	{
		for (i = 0; i < nbytes; i++){
			smart_putc_inactive(terminal_execute, ((char *)buf)[i]);
			set_cursor_inactive(terminal_execute, get_screen_x_inactive(terminal_execute), get_screen_y_inactive(terminal_execute));
		}
	}
    /* Adds extra newline
	if (get_screen_x() != 0)
		smart_putc('\n');
    */
	// return bytes read
	return i;
}
