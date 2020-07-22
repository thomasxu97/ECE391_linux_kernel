#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#define asmlinkage __attribute__((regparm(0)))

#include "types.h"

#define MAGIC_NUMBER_LENGTH         4
#define MAGIC_NUMBER_FIRST          0x7F
#define MAGIC_NUMBER_SECOND         0x45
#define MAGIC_NUMBER_THIRD          0x4C
#define MAGIC_NUMBER_FOURTH         0x46
#define INT_SIZE                    4
#define EXCEPTION_RETURN            256

#define USER_PROGRAM_COPY_ADDRESS   0x08048000
#define USER_PROGRAM_ENTRY_ADDRESS  0x08048018

#define NEW_PAGE_OFFSET             0x00048000

#define USER_PROGRAM_ESP            0x083ffffc
#define STI_MASK                    0x200


/* read nbytes into buf from file fd */
asmlinkage int32_t read(int32_t fd, void* buf, int32_t nbytes);

/* write nbytes of data from buf into file fd */
asmlinkage int32_t write(int32_t fd, const void* buf, int32_t nbytes);

/* open file by file name, allocate file descriptor */
asmlinkage int32_t open(const uint8_t* filename);

/* close file directed by fd */
asmlinkage int32_t close(int32_t fd);

/* halt the program after execution */
asmlinkage int32_t halt(uint8_t status);

/* execution program: allocate pcb, allocate paging, perform context switching
 * copy program into user space, allocate user stack and kernel stack */
asmlinkage int32_t execute(const uint8_t* command);

/* reads programm command line arguments */
asmlinkage int32_t getargs(uint8_t* buf, int32_t nbytes);

/* maps text-mode viddeo memory into user space at specific virtual address */
asmlinkage int32_t vidmap(uint8_t** screen_start);

asmlinkage int32_t set_handler(int32_t signum, void* handler_address);

asmlinkage int32_t sigreturn(void);


/* parses command from terminal */
void parse_command(const uint8_t* command, uint8_t* file_name, uint8_t* arguments);

#endif
