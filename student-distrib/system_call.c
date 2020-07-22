#include "system_call.h"
#include "filesystem.h"
#include "types.h"
#include "rtc.h"
#include "lib.h"
#include "pcb.h"
#include "terminal.h"
#include "paging.h"
#include "x86_desc.h"
#include "schedule.h"

static int32_t execute_state = 0;

/* read
 * Description: systemcall read
 *              read nbytes into buf from file fd
 * Input: fd - file descriptor
 *        buf - pointer to buffer
 *        nbytes - number of bytes to read
 * Output: none
 * Return: bytes read if success, -1 if fail
 * SideEffect: none
 */
asmlinkage int32_t read(int32_t fd, void* buf, int32_t nbytes){
    int32_t ret;
    int32_t i;
    int32_t flag = 0;
    // check for situations that lead to failure
    if (fd < 0 || fd > 7 || nbytes < 0 || buf == NULL)//||buf>(void*)KERNEL_BOTTOM||buf<(void*)KERNEL_TOP)
        return -1;
    
    pcb_t* current=(pcb_t *)(KERNEL_BOTTOM - BLOCK_8KB*(global_terminal[terminal_execute].last_pid+1));
    // copy the current->file descriptor into global file descriptor
    for(i=0;i<MAX_OPENNING_FILE;i++){
        global_file_descriptor[i]=current->fd_table[i];
    }
    // if successful call the file system read driver function
    if(global_file_descriptor[fd].flags!=0){
        if (global_file_descriptor[fd].file_operations_table_pointer->read!=NULL){
        flag=1;
        ret=global_file_descriptor[fd].file_operations_table_pointer->read(fd,buf,nbytes);    
        }
    }
    //restore back
    for(i=0;i<MAX_OPENNING_FILE;i++){
        current->fd_table[i]=global_file_descriptor[i];
        global_file_descriptor[i].file_operations_table_pointer = NULL;
        global_file_descriptor[i].index_node_number = 0;
        global_file_descriptor[i].file_position = 0;
        global_file_descriptor[i].flags = 0;
    }
    // return appropriate value
    if(flag==1)
        return ret;
    else
        return -1;
}


/* write
 * Description: system call write
 *              write nbytes of data from buf into file fd
 * Input: fd - file descriptor
 *        buf - pointer to buffer
 *        nbytes - number of bytes to write
 * Output: none
 * Return: bytes written if success, -1 if fail
 * SideEffect: none
 */
asmlinkage int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    int32_t ret;
    int32_t i;
    int32_t flag=0;
    // check for situations that lead to failure
    if (fd < 0 || fd > 7 || nbytes < 0 || buf == NULL)//||buf>(void*)KERNEL_BOTTOM||buf<(void*)KERNEL_TOP)
        return -1;  

    pcb_t* current=(pcb_t *)(KERNEL_BOTTOM - BLOCK_8KB*(global_terminal[terminal_execute].last_pid+1));
    // copy the current->file descriptor into global file descriptor
    for(i=0;i<MAX_OPENNING_FILE;i++){
        global_file_descriptor[i]=current->fd_table[i];
    }
    // if successful call the file system write driver function
    if(global_file_descriptor[fd].flags!=0){
        if (global_file_descriptor[fd].file_operations_table_pointer->write!=NULL){
        flag=1;
        ret=global_file_descriptor[fd].file_operations_table_pointer->write(fd,buf,nbytes);
        }
    }
    //restore back
    for(i=0;i<MAX_OPENNING_FILE;i++){
        current->fd_table[i]=global_file_descriptor[i];
        global_file_descriptor[i].file_operations_table_pointer = NULL;
        global_file_descriptor[i].index_node_number = 0;
        global_file_descriptor[i].file_position = 0;
        global_file_descriptor[i].flags = 0;
    }
    // return appropriate value
    if(flag==1)
        return ret;
    else
        return -1;
}


/* open
 * Description: system call open
 *              open file by file name, allocate file descriptor
 * Input: fd - file descriptor
 *        buf - pointer to buffer
 *        nbytes - number of bytes to write
 * Output: none
 * Return: fd if success, -1 if fail
 * SideEffect: none
 */
asmlinkage int32_t open(const uint8_t* filename){
    int32_t flag=0;
    int32_t i;
    int32_t ret;
    if (filename==NULL)
        return -1;
    directory_entry_t file;

    pcb_t* current=(pcb_t *)(KERNEL_BOTTOM - BLOCK_8KB*(global_terminal[terminal_execute].last_pid+1));

    // if filename is std_in
    if (!strncmp((int8_t*)filename, (int8_t*)"std_in", FILE_NAME_MAX_LENGTH))
    {
        if (current->fd_table[0].flags == 1)
        {
            return -1;
        }
        else
        {
            current->fd_table[0].flags = 1;
            current->fd_table[0].file_position = 0;
            current->fd_table[0].index_node_number = 0;
            current->fd_table[0].file_operations_table_pointer = &std_in_jump_table;
            ret = terminal_open((uint8_t*)"std_in");
            return ret;
        }
    }

    // if filename is std_out
    if (!strncmp((int8_t*)filename, (int8_t*)"std_out", FILE_NAME_MAX_LENGTH))
    {
        if (current->fd_table[1].flags == 1)
        {
            return -1;
        }
        else
        {
            current->fd_table[1].flags = 1;
            current->fd_table[1].file_position = 0;
            current->fd_table[1].index_node_number = 0;
            current->fd_table[1].file_operations_table_pointer = &std_out_jump_table;
            ret = terminal_open((uint8_t*)"std_out");
            return ret;
        }
    }

    // read dentry
    if (read_dentry_by_name(filename, &file) == -1)
        return -1;

    for(i=0;i<MAX_OPENNING_FILE;i++){
        global_file_descriptor[i]=current->fd_table[i];
    }
    
    // rtc file
    if(file.file_type==0){
        ret = rtc_open(filename);
        if(ret != -1)
            flag = 1;
    }

    // directory file
    else if (file.file_type==1){
        ret = directory_open(filename);
        if(ret != -1)
            flag = 1;
    }

    // regular file
    else if (file.file_type==2){
        ret = file_open(filename);
        if(ret != -1)
            flag = 1;
    }

    // restore all
    for(i=0;i<MAX_OPENNING_FILE;i++){
        current->fd_table[i]=global_file_descriptor[i];
        global_file_descriptor[i].file_operations_table_pointer = NULL;
        global_file_descriptor[i].index_node_number = 0;
        global_file_descriptor[i].file_position = 0;
        global_file_descriptor[i].flags = 0;
    }

    // return appropriate value
    if(flag==0) 
        return -1;
    else 
        return ret;
}


/* close
 * Description: system call close
 *              close file directed by fd
 * Input: fd - file descriptor
 * Output: none
 * Return: 0 if success, -1 if fail
 * SideEffect: file is closed
 */
asmlinkage int32_t close(int32_t fd){
    int32_t flag=0;
    int32_t i;
    int32_t ret;
    // check for failure situation
    if(fd>MAX_OPENNING_FILE-1)
        return -1;
    // check negative fd
    if (fd < 2)
        return -1;

    // find latest pcb
    pcb_t* current=(pcb_t *)(KERNEL_BOTTOM - BLOCK_8KB*(global_terminal[terminal_execute].last_pid+1));
    // copy to global_file_descriptor
    for(i=0;i<MAX_OPENNING_FILE;i++){
        global_file_descriptor[i]=current->fd_table[i];
    }
    // find file to close and call close driver function
    if(global_file_descriptor[fd].flags!=0)
    {
        if (global_file_descriptor[fd].file_operations_table_pointer->close!=NULL){
        flag=1;
        ret=global_file_descriptor[fd].file_operations_table_pointer->close(fd);
      }
    }
    // restore all
    for(i=0;i<MAX_OPENNING_FILE;i++){
        current->fd_table[i]=global_file_descriptor[i];
        global_file_descriptor[i].file_operations_table_pointer = NULL;
        global_file_descriptor[i].index_node_number = 0;
        global_file_descriptor[i].file_position = 0;
        global_file_descriptor[i].flags = 0;
    }
    // return appropriate value
    if(flag==1)
        return 0;
    else 
        return -1;
}

/* halt
 * Description: system call halt
 *              halt the program after execution
 * Input: status - execution status
 * Output: none
 * Return: status
 * SideEffect: context switch is performed
 */
asmlinkage int32_t halt(uint8_t status){
    // while (1)
    //     if (terminal_execute == terminal_show)
    //         break;
    cli();      // disable interrupt
    int32_t ret=(int32_t)status;
    int32_t i;

    // find current pcb
    int32_t halt_pid = global_terminal[terminal_execute].last_pid;
    pcb_t* current=(pcb_t *)(KERNEL_BOTTOM - BLOCK_8KB*(halt_pid+1));

    // restore esp0 in tss
    tss.esp0 = KERNEL_BOTTOM - (uint32_t)current->parent_pid * BLOCK_8KB;

    // close all files
    for (i=0; i<MAX_OPENNING_FILE; i++)
    {
        if (current->fd_table[i].flags == 1)
            close(i);
    }
    // close pcb, restore paging, update terminal information
    memset((void*) current->arg_buffer, 0, ARG_BUFFER_SIZE);
    current->in_use=0;
    current->terminal->last_pid = current->parent_pid;
    global_terminal[terminal_execute].last_pid = current->parent_pid;
    // start a new shell if no program is running on this terminal
    current->terminal->num_task--;

    if (current->terminal->num_task == 0)
    {
        sti();
        execute((uint8_t*)"shell");
    }

    create_user_level_program_paging(current->terminal->last_pid);
    execute_state = ret;

    // restore stack pointer before user level is executed
    // store return value in %eax
    asm volatile(
        "movl %0,%%eax;"
        "movl %1,%%esp;"
        "movl %2,%%ebp;" 
        "jmp return_label;"                        
        :
        :"r" ((uint32_t)status), "r" (current->parent_esp0), "r" (current->parent_ebp0)
        :"eax", "esp", "ebp"
    );
    return ret;    
}


/* execute
 * Description: system call execute
 *              execution program: allocate pcb, allocate paging, perform context switching
 *              copy program into user space, allocate user stack and kernel stack
 * Input: command - string of command to execute
 * Output: none
 * Return: -1 for failure, 0-255 for normal exit, 256 for exit with exception
 * SideEffect: context switch is performed
 */
asmlinkage int32_t execute(const uint8_t* command) {
    while (1)
        if (terminal_execute == terminal_show){
            cli();
            break;
        }
    uint8_t fname[FILE_NAME_MAX_LENGTH];
    int32_t i;
    directory_entry_t dentry;
    pcb_t* current_pcb;
    int32_t pcb_index;
    index_nodes_t* file_address;
    int32_t* entry_address = (int32_t*)USER_PROGRAM_ENTRY_ADDRESS;
    int32_t entry;
    uint32_t esp0;
    uint8_t file_arguments[ARG_BUFFER_SIZE];


    uint8_t first_four_bytes[MAGIC_NUMBER_LENGTH];
    uint8_t executable_bytes[MAGIC_NUMBER_LENGTH] = 
        {MAGIC_NUMBER_FIRST, MAGIC_NUMBER_SECOND, MAGIC_NUMBER_THIRD, MAGIC_NUMBER_FOURTH};


    // the first word in command is regarded as the executable file name
    // if first word is longer than 32, the first 32 will be regard as file name
    // file name and arguemnts placed in respective buffers
    parse_command(command, fname, file_arguments);
    
    // check the magic number at start of file to verify it is an executable file
    if (read_dentry_by_name(fname, &dentry) == -1)
        return -1;

    read_data(dentry.index_node_number, 0, first_four_bytes, MAGIC_NUMBER_LENGTH);
    file_address = (index_nodes_t*)((uint8_t*)global_filesystem_address + BYTES_BLOCK_TOTAL * (dentry.index_node_number + 1));

    for (i = 0; i < MAGIC_NUMBER_LENGTH; i++)
        if (first_four_bytes[i] != executable_bytes[i])
            return -1;

    // allocate and initialize pcb for the program
    pcb_index = allocate_pcb();
    if (pcb_index < 0){
        printf("cannot allocate more pcb\n");
        // while(1);
        return -1;
    }
    current_pcb = get_pcb_by_pid(pcb_index);

    // set parent pid field in pcb structure and update latest process in terminal
    if (current_pcb->terminal->num_task == 0)
    {
        current_pcb->parent_pid = 0;
        current_pcb->terminal->last_pid = current_pcb->pid;
        current_pcb->terminal->num_task++;
    }
    else
    {
        current_pcb->parent_pid = current_pcb->terminal->last_pid;
        current_pcb->terminal->last_pid = current_pcb->pid;
        current_pcb->terminal->num_task++;
    }

    // current_pcb->esp=_8MB-_8KB*pcb_index-4;
    // current_pcb->ebp=_8MB-_8KB*pcb_index-4;
    // copy file arguments to current process
    strcpy((int8_t*)current_pcb->arg_buffer, (int8_t*)file_arguments);

    // open std_in and std_out for the program
    open((uint8_t*)"std_in");
    open((uint8_t*)"std_out");

    // create paging for the user program
    create_user_level_program_paging(pcb_index);

    // copy the program to appropriate offset in the user space
    read_data(dentry.index_node_number, 0, (uint8_t*)USER_PROGRAM_COPY_ADDRESS, file_address->length);
    entry = (uint32_t) (*entry_address);

    // allocate user stack
    esp0 = KERNEL_BOTTOM - BLOCK_8KB * pcb_index - INT_SIZE;
    // esp0 = KERNEL_BOTTOM - BLOCK_8KB * (pcb_index + 1) - INT_SIZE;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = esp0;

    // store current ebp and esp to parent_esp0 and parent_ebp0 field of pcb
    // will be restored in halt function
    asm volatile ("             \n\
        movl    %%esp, %0       \n\
        movl    %%ebp, %1       \n\
        "
                :"=r" (current_pcb->parent_esp0), "=r" (current_pcb->parent_ebp0) 
                :
                :"memory");


    // perform context swtich
    // USER_DS          0x2B
    // USER_PROGRAM_ESP 0x083ffffc = 128MB + 4MB - 4
    // orl 0x200 set interrupt flag
    // USER_CS          0x23
    // return label: place halt function will return to
    asm volatile ("             \n\
        movw    %0, %%ax        \n\
        movw    %%ax, %%ds      \n\
        pushl   %0              \n\
        pushl   %1              \n\
        pushfl                  \n\
        popl    %%eax           \n\
        orl     %2, %%eax       \n\
        pushl   %%eax           \n\
        pushl   %3              \n\
        pushl   %4              \n\
        iret                    \n\
        return_label:           \n\
        "
                :
                : "g"(USER_DS), "r"(USER_PROGRAM_ESP), "r"(STI_MASK), "r"(USER_CS), "r"(entry)
                : "eax", "memory");

    return execute_state;
}



/* getargs
 * DESCRIPTION: copies command line arugments to user-level buffer
 * INPUT: buf - user-level buffer
 *        nbytes - number of bytes
 * OUTPUT: NONE
 * RETURN: -1 on failure, 0 on success
 * SIDE EFFECT: writes to user level buffer
 */
asmlinkage int32_t getargs(uint8_t* buf, int32_t nbytes){  
    // get current process structure
    pcb_t* pcb = get_pcb_by_pid(global_terminal[terminal_execute].last_pid);

    if (buf == NULL|| strlen((int8_t*)pcb->arg_buffer) > nbytes)
        return -1;

    // for debugging argument buffer
    // printf("%s\n", (int8_t*)pcb->arg_buffer); 
    // copy current process's arguments over to buffer
    strcpy((int8_t*)buf, (int8_t*)pcb->arg_buffer);

    return 0;
}

/* vidmap
 * DESCRIPTION: map video memory to specific virtual address
 * INPUT: screen_start - where screen starts
 * OUTPUT: NONE
 * RETURN: -1 failure, 0 success
 */
asmlinkage int32_t vidmap(uint8_t** screen_start){
    /* check for valid location */
    // [128MB, 132MB)
    if (screen_start == NULL|| screen_start < (uint8_t**)_128MB || screen_start >= (uint8_t**)_132MB)
        return -1;

    // create new page
    create_vidmap_paging();

    // 128MB + 4MB = 132MB
    *(screen_start) = (uint8_t*)(_132MB);

    return 0;
}

/* extra credit function */
asmlinkage int32_t set_handler(int32_t signum, void* handler_address){
    return -1;
}
/* extra credit function */
asmlinkage int32_t sigreturn(void){
    return -1;
}

/* parse_command
 * DESCRIPTION: parses the command line argument into filename and file arguments
 * INPUT: command - command line argument
 *        file_name - buffer for file name
 *        arguments - buffer for arguments
 * OUTPUT: NONE
 * RETURN: NONE
 * SIDE EFFECT: Writes to respective buffers
 */
void parse_command(const uint8_t* command, uint8_t* file_name, uint8_t* arguments){
    int32_t i = 0, j = 0;

    // parsing file name
    for (i = 0; i < FILE_NAME_MAX_LENGTH; i++)
    {
        if (command[i] != ' ' && command[i] != '\0' && command[i] != '\n')
            file_name[i] = command[i];
        else
            break;
    }
    if (i < FILE_NAME_MAX_LENGTH)
        file_name[i] = '\0';

    i++;
    //parsing arguments
    while (command[i] != '\0' && command[i] != '\n' && j < ARG_BUFFER_SIZE){
        arguments[j] = command[i];
        j++;
        i++;
    }
    
    arguments[j] = '\0';
}

