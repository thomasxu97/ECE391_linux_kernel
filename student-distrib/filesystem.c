#include "filesystem.h"
#include "pcb.h"
#include "rtc.h"
#include "terminal.h"

/* global variable of filesystem address */
boot_block_t* global_filesystem_address;

/* temporary file descriptor array structure
   (will move to PCB after it is finished) */
file_descriptor_t global_file_descriptor[MAX_OPENNING_FILE];

/* global file operation jump tables */
file_operation_jump_table_t regular_file_operation_jump_table;
file_operation_jump_table_t directory_file_operation_jump_table;
file_operation_jump_table_t rtc_jump_table;
file_operation_jump_table_t std_in_jump_table;
file_operation_jump_table_t std_out_jump_table;

/* filesystem_init
 * initialize the address of file system and initialize all global structures
 * Input: filesystem_address - address of the in-memory filesystem 
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: global structures are initialized
 */
int32_t filesystem_init(boot_block_t* filesystem_address) {
    // check the filesystem_address not NULL
    int i;
    if (filesystem_address == NULL)
        return -1;
    // Set address of file system
    global_filesystem_address = filesystem_address;
    // Initialize global file descriptor
    for (i=0; i<MAX_OPENNING_FILE; i++)
    {
        global_file_descriptor[i].file_operations_table_pointer = NULL;
        global_file_descriptor[i].index_node_number = 0;
        global_file_descriptor[i].file_position = 0;
        global_file_descriptor[i].flags = 0;
    }
    /* initialize global file operation jump tables */
    regular_file_operation_jump_table.open = &file_open;
    regular_file_operation_jump_table.close = &file_close;
    regular_file_operation_jump_table.read = &file_read;
    regular_file_operation_jump_table.write = &file_write;

    directory_file_operation_jump_table.open = &directory_open;
    directory_file_operation_jump_table.close = &directory_close;
    directory_file_operation_jump_table.read = &directory_read;
    directory_file_operation_jump_table.write = &directory_write;

    rtc_jump_table.open = &rtc_open;
    rtc_jump_table.close = &rtc_close;
    rtc_jump_table.read = &rtc_read;
    rtc_jump_table.write = &rtc_write;

    std_in_jump_table.open = &terminal_open;
    std_in_jump_table.close = &terminal_close;
    std_in_jump_table.read = &terminal_read;
    std_in_jump_table.write = NULL;

    std_out_jump_table.open = &terminal_open;
    std_out_jump_table.close = &terminal_close;
    std_out_jump_table.read = NULL;
    std_out_jump_table.write = &terminal_write;

    return 0;
}

/* allocate_file_descriptor
 * allocate a not used file descriptor and return it
 * Input: none
 * Output: none
 * Return: allocated file descriptor if success, -1 if failure
 * Side Effect: none
 */
int32_t allocate_file_descriptor ()
{
    int i;
    for (i=2; i<MAX_OPENNING_FILE; i++)
        if (global_file_descriptor[i].flags == 0)
            return i;
    return -1;
}

/* file_open
 * open a regular file using its filename
 * Requirement: the file exists, is regular file, is not opened, 
 *              number of openned file does not exceed 8
 * Input: filename - char pointer to the file name
 * Output: none
 * Return: fd for success, -1 for failure
 * Side Effect: the file is opened, no other file can be opened simutanously
 */
int32_t file_open(const uint8_t* filename) {
    
    directory_entry_t file_to_open;
    uint32_t fd;
    // check all the cases that can result to failure
    if (read_dentry_by_name(filename, &file_to_open) == -1)
        return -1;
    if (file_to_open.file_type != REGULAR_FILE_TYPE)
        return -1;
    fd = allocate_file_descriptor();
    if (fd == -1)
        return -1;
    // open success, set file descriptor structure
    global_file_descriptor[fd].file_operations_table_pointer = &regular_file_operation_jump_table;
    global_file_descriptor[fd].index_node_number = file_to_open.index_node_number;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 1;
    
    return fd;
}

/* file_close
 * close an opened file
 * Input: fd - the file descriptor of the opened file
 * Requirement: the file to close must be a regular file and has been opened
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: the file is closed
 */
int32_t file_close(int32_t fd) {
    // check all the cases that can result to failure
    if (fd < 2 || fd >= MAX_OPENNING_FILE)
        return -1;
    // restore default settings
    global_file_descriptor[fd].file_operations_table_pointer = NULL;
    global_file_descriptor[fd].index_node_number = 0;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 0;
    return 0;
}

/* file_read
 * read nbytes bytes from an opened file, and store the content into a buffer
 * the progress of what have been read is stored
 * stop reading if reaches the end of the file
 * Input: fd - the file descriptor of the opened file
 *        buf - the address of the buffer
 *        nbytes - number of bytes want to read
 * Requirements: buffer size need to be larger than nbytes
 *               file must be opened
 * Output: none
 * Return: number of bytes read for success, -1 for failure
 * Side Effect: content stored in buffer
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    int32_t bytes_read;
    // check file is opened and is regular file
    if (fd < 0 || fd >= MAX_OPENNING_FILE)
        return -1;
    if (global_file_descriptor[fd].flags == 0)
        return -1;
    // Call read data to read file
    bytes_read = read_data(global_file_descriptor[fd].index_node_number, 
        global_file_descriptor[fd].file_position, (uint8_t*) buf, nbytes);
    // Update read progress
    global_file_descriptor[fd].file_position += bytes_read;
    return bytes_read;
}

/* file_write
 * write nbytes bytes from buffer to file data blocks
 * currently all files are read-only so the function do nothing and return -1
 * Input: fd - the file descriptor of the opened file
 *        buf - the address of the buffer
 *        nbytes - number of bytes want to write
 * Requirements: file must be opened
 * Output: none
 * Return: -1 for failure
 * Side Effect: content written to file data blocks
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}


/* directory_open
 * open a directory file using its filename
 * Requirement: no file is being opened, file to open must be a directory file and exists
 * Input: filename - char pointer to the file name
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: the file is opened, no other file can be opened simutanously
 */
int32_t directory_open(const uint8_t* filename) {
    directory_entry_t directory_to_open;
    uint32_t fd;
    // check that no file is opened, target file exists and is a directory
    if (read_dentry_by_name(filename, &directory_to_open) == -1)
        return -1;
    if (directory_to_open.file_type != DIRECTORY_FILE_TYEP)
        return -1;
    fd = allocate_file_descriptor();
    if (fd == -1)
        return -1;
    // open success, set file descriptor structure
    global_file_descriptor[fd].file_operations_table_pointer = &directory_file_operation_jump_table;
    global_file_descriptor[fd].index_node_number = 0;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 1;
    return fd;
}

/* directory_close
 * close an opened directory file
 * Requirement: the file to close must be a directory file and has been opened
 * Input: fd - the file descriptor of the opened file
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: the file is closed
 */
int32_t directory_close(int32_t fd) {
    // check all the cases that can result to failure
    if (fd < 2 || fd >= MAX_OPENNING_FILE)
        return -1;
    if (global_file_descriptor[fd].flags == 0)
        return -1;
    // restore default settings
    global_file_descriptor[fd].file_operations_table_pointer = NULL;
    global_file_descriptor[fd].index_node_number = 0;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 0;
    return 0;
}

/* directory_read
 * In order, store a filename (at most nbytes) of a file from the opened directory to a buffer 
 * store nothing and return -1 if filename is longer than nbytes
 * the progress of which filename has been read is stored
 * Input: fd - the file descriptor of the opened file
 *        buf - the address of the buffer
 *        nbytes - number of bytes at most to read
 * Requirements: buffer size need to be larger than nbytes
 *               directory must be opened
 * Output: none
 * Return: number of bytes read for success, -1 for failure
 * Side Effect: content stored in buffer
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) {
    int32_t i, j, fname_length;
    // check directory is opened
    if (fd < 0 || fd >= MAX_OPENNING_FILE)
        return -1;
    if (global_file_descriptor[fd].flags == 0)
        return -1;
    // calculate index of next file in the directory and check it within range
    i = global_file_descriptor[fd].file_position;
    if (i > MAX_NUM_DIRECTORY_ENTRIES - 1)
        return -1;
    // check file name size within range
    for (fname_length = 0; fname_length < FILE_NAME_MAX_LENGTH; fname_length++)
        if (global_filesystem_address->dir_entries[i].file_name[fname_length] == '\0')
            break;
    if (nbytes < fname_length)
        return 0;
    // copy file name into buffer
    for (j = 0; j < fname_length; j++)
        *((int8_t*)buf + j) = global_filesystem_address->dir_entries[i].file_name[j];
    // update progress of file name reading
    global_file_descriptor[fd].file_position++;
    return fname_length;
}


/* get_current_file_type_helper
 * get the type of a file in order in a directory
 * do not remember whether a file has its type returned
 * must be used togeter with directory read
 * Requirements: directory must be opened
 *               directory read is being called
 * Input: fd - file descriptor of the opened directory
 * Output: none
 * Return: the file type
 * Side effect: none
 */
int32_t get_current_file_type_helper(int32_t fd) {
    int32_t i;
    // check directory is opened
    if (fd < 0 || fd >= MAX_OPENNING_FILE)
        return -1;
    if (global_file_descriptor[fd].flags == 0)
        return -1;
    // calculate file offset within range
    i = global_file_descriptor[fd].file_position;
    if (i > MAX_NUM_DIRECTORY_ENTRIES - 1)
        return -1;
    return global_filesystem_address->dir_entries[i].file_type;
}

/* get_current_file_size_helper
 * get the size of a file in order in a directory
 * do not remember whether a file has its size returned
 * must be used togeter with directory read
 * Requirements: directory must be opened
 *               directory read is being called
 * Input: fd - file descriptor of the opened directory
 * Output: none
 * Return: the file size
 * Side effect: none
 */
int32_t get_current_file_size_helper(int32_t fd) {
    int32_t i, j;
    index_nodes_t* file_address;
    // check directory is opened
    if (fd < 0 || fd >= MAX_OPENNING_FILE)
        return -1;
    if (global_file_descriptor[fd].flags == 0)
        return -1;
    // calculate file offset within range
    i = global_file_descriptor[fd].file_position;
    if (i > MAX_NUM_DIRECTORY_ENTRIES - 1)
        return -1;
    // calculate inode within range
    j = global_filesystem_address->dir_entries[i].index_node_number;
    if (j > global_filesystem_address->num_index_nodes)
        return -1;
    // find inode address
    file_address = (index_nodes_t*)((uint8_t*)global_filesystem_address + BYTES_BLOCK_TOTAL * (j + 1));
    return file_address->length;
}

/* directory_write
 * write information into the directory file
 * currently unsupported 
 * Input: fd - the file descriptor of the opened directory
 *        buf - the address of the buffer
 *        nbytes - number of bytes want to write
 * Requirements: directory must be opened
 * Return: 0 for success, -1 for failure
 * Side Effect: none
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* read_dentry_by_name
 * find the wanted directory entry structure by file name
 * copy the directory entry structure from file system to user space
 * Input: fname - char pointer to the filename
 *        dentry - pointer to the target address of structure copying
 * Requirements: fname must exist in filesystem
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: directory entry structure is copied to user space
 */
int32_t read_dentry_by_name(const uint8_t* fname, directory_entry_t* dentry) {
    int32_t length = 0, i;
    // check pointer is valid
    if (fname == NULL || dentry == NULL)
        return -1;
    // check file name within range
    for (i = 0; *(fname + i) != '\0'; length++, i++);
    if (length == 0)
        return -1;
    // search file name and copy structure
    for (i = 0; i < MAX_NUM_DIRECTORY_ENTRIES; i++)
    {
        if (strncmp((int8_t*)fname, (int8_t*)global_filesystem_address->dir_entries[i].file_name, FILE_NAME_MAX_LENGTH) == 0)
        {
            (void)memcpy((void*)dentry, (void*)&global_filesystem_address->dir_entries[i], BYTES_DIRECTORY_ENTRY_TOTAL);
            return 0;
        }
    }
    return -1;
}

/* read_dentry_by_index
 * find the wanted directory entry structure by index in the bootblock
 * copy the directory entry structure from file system to user space
 * Input: index - index of directory entry structure in bootblock
 *        dentry - pointer to the target address of structure copying
 * Requirements: index must within the range
 * Output: none
 * Return: 0 for success, -1 for failure
 * Side Effect: directory entry structure is copied to user space
 */
int32_t read_dentry_by_index(uint32_t index, directory_entry_t* dentry) {
    // check pointer valid and index within range
    if (dentry == NULL)
        return -1;
    if (index >= global_filesystem_address->num_directory_entries)
        return -1;
    // copy structure
    (void)memcpy((void*)dentry, (void*)&global_filesystem_address->dir_entries[index], BYTES_DIRECTORY_ENTRY_TOTAL);
    return 0;
}

/* read_data
 * read length bytes data in file data blocks and store in the buffer
 * terminate reading if file end is reached
 * Input: inode - index of inode block 
 *        offset - byte offset to read
 *        buf - address to store the content
 *        length - number of bytes at most to read
 * Requirements: inode within range
 * Output: none
 * Return: number of bytes copied
 * Side Effect: content written to buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    int32_t i, j, byte_copied = 0;
    index_nodes_t* file_address;
    uint8_t* data_address;
    // check that pointer is valid and index node within range
    if (buf == NULL)
        return -1;
    if (inode >= global_filesystem_address->num_index_nodes)
        return -1;
    // calculate datablock index and offset of corresponding data block
    i = offset/BYTES_4KB;
    j = offset%BYTES_4KB;
    // calculate corresponding data block address
    file_address = (index_nodes_t*)((uint8_t*)global_filesystem_address + BYTES_BLOCK_TOTAL * (inode + 1));
    data_address = (uint8_t*)global_filesystem_address + BYTES_BLOCK_TOTAL * (global_filesystem_address->num_index_nodes + 1);
    // check file end
    if (file_address->length <= offset)
        return byte_copied;
    if (file_address->length < offset + length)
        length = file_address->length - offset;
    // copy data one byte by one byte until file end reaches / maximum bytes reaches
    for (; byte_copied < length; byte_copied++)
    {
        if (file_address->data_blocks[i] >= global_filesystem_address->num_data_blocks)
            return -1;
        *(buf + byte_copied) = *(data_address + file_address->data_blocks[i]*BYTES_4KB + j);
        j++;
        if (j >= BYTES_4KB)
        {
            j = 0;
            i++;
        }
    }
    return byte_copied;
}
