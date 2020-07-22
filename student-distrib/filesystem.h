#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "lib.h"

/* constants */

#define BYTES_BOOT_BLOCK_RESERVE        52
#define BYTES_DIRECTORY_ENTRY_RESERVE   24
#define BYTES_BLOCK_TOTAL               4096
#define BYTES_DIRECTORY_ENTRY_TOTAL     64
#define MAX_NUM_DIRECTORY_ENTRIES       63
#define MAX_NUM_DATA_BLOCKS             1023
#define FILE_NAME_MAX_LENGTH            32
#define MAX_OPENNING_FILE               8

#define RTC_FILE_TYPE                   0
#define DIRECTORY_FILE_TYEP             1
#define REGULAR_FILE_TYPE               2

#define BYTES_4KB                       4096

/* directory entry structure */
typedef struct directory_entry {
    uint8_t file_name[FILE_NAME_MAX_LENGTH];
    uint32_t file_type;
    uint32_t index_node_number;
    uint8_t reserved[BYTES_DIRECTORY_ENTRY_RESERVE];
}directory_entry_t;

/* boot block stucture */
typedef struct boot_block {
    uint32_t num_directory_entries;
    uint32_t num_index_nodes;
    uint32_t num_data_blocks;
    uint8_t reserved[BYTES_BOOT_BLOCK_RESERVE];
    directory_entry_t dir_entries[MAX_NUM_DIRECTORY_ENTRIES];
}boot_block_t;

/* index nodes structure */
typedef struct index_nodes {
    uint32_t length;
    uint32_t data_blocks[MAX_NUM_DATA_BLOCKS];
}index_nodes_t;

/* file operation jump table structure */
typedef struct file_operation_jump_table {
    int32_t (*open) (const uint8_t*);
    int32_t (*close) (int32_t);
    int32_t (*read) (int32_t, void*, int32_t);
    int32_t (*write) (int32_t, const void*, int32_t);
}file_operation_jump_table_t;

/* file descriptor structure */
typedef struct file_descriptor {
    file_operation_jump_table_t* file_operations_table_pointer;
    int32_t index_node_number;
    int32_t file_position;
    int32_t flags;
}file_descriptor_t;

extern int32_t allocate_file_descriptor ();
/* initialize the address of file system and initialize all global variables */
extern int32_t filesystem_init(boot_block_t* filesystem_address);

/* open a regular file using its filename */
extern int32_t file_open(const uint8_t* filename);

/* close an opened file */
extern int32_t file_close(int32_t fd);

/* read nbytes bytes from an opened file, and store the content into a buffer
 * the progress of what have been read is stored
 * stop reading if reaches the end of the file */
extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

/* write nbytes bytes from buffer to file data blocks
 * currently all files are read-only so the function do nothing and return -1 */
extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

/* open a directory file using its filename */
extern int32_t directory_open(const uint8_t* filename);

/* close an opened directory file */
extern int32_t directory_close(int32_t fd);

/* In order, store a filename (at most nbytes) of a file from the opened directory to a buffer 
 * store nothing and return -1 if filename is longer than nbytes
 * the progress of which filename has been read is stored */
extern int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);

/* get the type of a file in order in a directory
 * do not remember whether a file has its type returned
 * must be used togeter with directory read */
extern int32_t get_current_file_type_helper(int32_t fd);

/* get the size of a file in order in a directory
 * do not remember whether a file has its size returned
 * must be used togeter with directory read */
extern int32_t get_current_file_size_helper(int32_t fd);

/* write information into the directory file
 * currently unsupported */
extern int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);

/* find the wanted directory entry structure by file name
 * copy the directory entry structure from file system to user space */
extern int32_t read_dentry_by_name (const uint8_t* fname, directory_entry_t* dentry);

/*find the wanted directory entry structure by index in the bootblock
 * copy the directory entry structure from file system to user space */
extern int32_t read_dentry_by_index (uint32_t index, directory_entry_t* dentry);

/* read length bytes data in file data blocks and store in the buffer
 * terminate reading if file end is reached
 * Input: inode - index of iode block */
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* global variable of filesystem address */
extern boot_block_t* global_filesystem_address;

/* temporary file descriptor array structure
   (will move to PCB after it is finished) */
extern file_descriptor_t global_file_descriptor[MAX_OPENNING_FILE];

/* global file operation jump tables */
extern file_operation_jump_table_t regular_file_operation_jump_table;
extern file_operation_jump_table_t directory_file_operation_jump_table;
extern file_operation_jump_table_t rtc_jump_table;
extern file_operation_jump_table_t std_in_jump_table;
extern file_operation_jump_table_t std_out_jump_table;

#endif
