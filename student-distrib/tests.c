#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "filesystem.h"
#include "keyboard.h"
#include "rtc.h"
#include "system_call.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* keyboard test
 *
 * enable keyboard irq
 * Inputs: none
 * Outputs: none
 * Side Effects: Enters while(1); loop and wait for keyboard interrupts
 */
int keyboard_test() {
	TEST_HEADER;
	enable_irq(1);
	while(1);
	return 0;
}


/* rtc test
 *
 * enable rtc irq
 * Inputs: none
 * Outputs: none
 * Side Effects: Enters while(1); loop and wait for rtc interrupts
 */
int rtc_test() {
	TEST_HEADER;
	// initilize all the required stuff
	// enable_irq(8);
	// rtc_open();
	// int8_t buffer[20];
	// uint32_t counter=2;
	// uint32_t copy;
	// uint8_t	digit;
	// int i;
	// int delay=counter;
	// clear();
	// set_cursor(0,0);
	// while(1){
	// 	// wait for interrupt by calling rtc_read
	// 	// printf("start of while loop\n");
	// 	rtc_read(0,NULL,0);
	// 	copy=counter;
	// 	// clear buffer
	// 	for(i=0;i<20;i++){
	// 		buffer[i]='\0';
	// 	}
	// 	// read the buffer and change it to int
	// 	i=0;
	// 	while(copy>0){
	// 		digit=(uint8_t)(copy%10);
	// 		copy=copy/10;
	// 		buffer[i]=digit+48;
	// 		i++;
	// 	}

	// 	strrev(buffer);
	// 	// printf("In the rtc test frequency is %s and counter is %d\n",buffer,counter);
	// 	// printf("buffer is %s and length is %d with start of 2 hz",buffer,strlen(buffer));   // not support 1 HZ !
	// 	rtc_write(0,buffer,strlen(buffer));
	// 	// printf("counter is %d\n", counter);
	// 	// largest rate I can set is 8192
	// 	// delay to show the desired frequency for a while and then change it to next frequency to show
	// 	if(counter<8192 && delay==0){
	// 		clear();
	// 		set_cursor(0,0);
	// 		counter*=2;
	// 		delay=counter;
	// 	// printf("end of while loop\n");
	// 	}
	// 	delay--;
	// }
	return 0;
}

/* paging test
 * 
 * Asserts that pointers to video memory dereference successfully
 * Asserts that pointers to kernel memory dereference successfully
 * Asserts that pointers pointing to any other memory cause page fault;
 * Inputs: testcase index
 * Outputs: print out pointer, expected result, and real result
 * Side Effects: None
 * Coverage: paging
 * Files: paging.h/S
 * Constraints: can only test one testcase each time. If page fault happens
 *				then the system needs to be restarted.
 */

int paging_test(int testcaseNum){
	clear();
	TEST_HEADER;

	uint32_t destination;	// place to hold the dereference value
	uint32_t* test_ptr;		// Pointer to be dereferenced
	
	switch (testcaseNum) {
		case 0:
			// Dereference NULL pointer, should fail
			test_ptr = NULL;
			printf("Dereference pointer 0x%x, should fail: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 1:
			// Dereference 0xB8F9E in video memory, should success
			test_ptr = (uint32_t*) 0xB8F9E;
			printf("Dereference pointer 0x%x, should success: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 2:
			// Dereference 0xB9880 outside video memory, should fail
			test_ptr = (uint32_t*) 0xB9880;
			printf("Dereference pointer 0x%x, should fail: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 3:
			// Dereference 0xB6000 outside video memory, should fail
			test_ptr = (uint32_t*) 0xB6000;
			printf("Dereference pointer 0x%x, should fail: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 4:
			// Dereference 0xFFFFFFFF, should fail
			test_ptr = (uint32_t*) 0xFFFFFFFF;
			printf("Dereference pointer 0x%x, should fail: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 5:
			// Dereference 0x400000 in kernel memory, should success
			test_ptr = (uint32_t*) 0x400000;
			printf("Dereference pointer 0x%x, should success: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 6:
			// Dereference 0x7FFFFC in kernel memory, should success
			test_ptr = (uint32_t*) 0x7FFFFC;
			printf("Dereference pointer 0x%x, should success: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 7:
			// Dereference 0x7FFFFD partially outside kernel memory, should fail
			test_ptr = (uint32_t*) 0x7FFFFD;
			printf("Dereference pointer 0x%x, should fail: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 8:
			// Dereference 0x5A43F6 in kernel memory, should success
			test_ptr = (uint32_t*) 0x5A43F6;
			printf("Dereference pointer 0x%x, should success: \n", test_ptr);
			destination = *test_ptr;
			break;
		case 9:
			// Dereference 0x6C7FF0 in kernel memory, should success
			test_ptr = (uint32_t*) 0x6C7FF0;
			printf("Dereference pointer 0x%x, should success: \n", test_ptr);
			destination = *test_ptr;
			break;
		default:
			printf("Not dereferencing anything. Please check testcase. \n");
			return 0;
	}
	// If page fault happens, then we will never reach here.
	printf("Dereference success!\n");
	return 1;
}

/* Checkpoint 2 tests */

/* filesystem_test_ls_all
 * Print a list of all files in the filesystem
 * Input: none
 * Output: file name, file type, file size
 * return: 0 for failure, 1 for sucess
 * Side effect: none
 */
int filesystem_test_ls() {
	clear();
	set_cursor(0, 0);
	TEST_HEADER;
	int32_t i;
	int32_t fd, type, size, fname_length;
	uint8_t buf[FILE_NAME_MAX_LENGTH+1];
	const uint8_t fname[FILE_NAME_MAX_LENGTH] = ".";
	fd = directory_open(fname);
	while (1)
	{
		type = get_current_file_type_helper(fd);
		if (type == -1)
			break;
		size = get_current_file_size_helper(fd);
		if (size == -1)
			break;
		fname_length = directory_read(fd, buf, FILE_NAME_MAX_LENGTH);
		if (fname_length <= 0)
			break;
		buf[fname_length] = '\0';
		printf("file_name: ");
		for (i = FILE_NAME_MAX_LENGTH - fname_length; i > 0; i--)
			printf(" ");
		printf("%s, file_type: %d, file_size: %d\n", buf, type, size);
	}
	directory_close(fd);
	return 1;
}

/* filesystem_test_file_by_index
 * print out the content of certain file given its index
 * Input: index - index of dentry in bootblock
 * Output: file name, file size, file content if applicable
 * return: 0 for failure, 1 for sucess
 * Side effect: none
 */
int test_file_by_index(int32_t index) {
	clear();
	set_cursor(0, 0);
	TEST_HEADER;
	int32_t i, ret, fd, bytes_read;
	uint8_t fname[FILE_NAME_MAX_LENGTH+1];
	uint8_t content[MAX_KBUF_SIZE];
	directory_entry_t dentry;
	ret = read_dentry_by_index(index, &dentry);
	if (ret != 0)
	{
		printf("index = %d:\n", index);
		printf("Invalid index!\n");
		return 0;
	}
	else
	{
		for (i = 0; i < FILE_NAME_MAX_LENGTH; i++)
			fname[i] = dentry.file_name[i];
		fname[FILE_NAME_MAX_LENGTH] = '\0';
		printf("index = %d, file_name = %s:\n", index, fname);
		fd = file_open(fname);
		if (fd == -1)
		{
			printf("Not a regular file!\n");
			return 0;
		}
		while (1)
		{
			bytes_read = file_read(fd, content, MAX_KBUF_SIZE);
			if (bytes_read <= 0)
				break;
			for (i=0; i<bytes_read; i++)
				smart_putc(content[i]);
		}
		file_close(fd);
	}
	return 1;
}

/* filesystem_test_file_by_name
 * print out the content of certain file given its name
 * Input: fname - pointer to file name
 * Output: file name, file size, file content if applicable
 * return: 0 for failure, 1 for sucess
 * Side effect: none
 */
int test_file_by_name(const char* file_name) {
	clear();
	set_cursor(0, 0);
	TEST_HEADER;
	int32_t i, ret, fd, bytes_read;
	uint8_t fname[FILE_NAME_MAX_LENGTH+1];
	uint8_t content[MAX_KBUF_SIZE];
	directory_entry_t dentry;
	ret = read_dentry_by_name((uint8_t*)file_name, &dentry);
	if (ret != 0)
	{
		printf("file_name = %s:\n", file_name);
		printf("File not found!\n");
		return 0;
	}
	else
	{
		for (i = 0; i < FILE_NAME_MAX_LENGTH; i++)
			fname[i] = dentry.file_name[i];
		fname[FILE_NAME_MAX_LENGTH] = '\0';
		printf("file_name = %s:\n", fname);
		fd = file_open(fname);
		if (fd == -1)
		{
			printf("Not a regular file!\n");
			return 0;
		}
		while (1)
		{
			bytes_read = file_read(fd, content, MAX_KBUF_SIZE);
			if (bytes_read <= 0)
				break;
			for (i=0; i<bytes_read; i++)
				smart_putc(content[i]);
		}
		file_close(fd);
	}
	return 1;
}

void launch_tests_ckpt_2() {
	disable_irq(1);
	disable_irq(8);
	// TEST_OUTPUT("keyboard_test", keyboard_test());
	// TEST_OUTPUT("rtc_test", rtc_test());


	/* Below is test cases for filesystem
	 * Run the program in gdb and use breakpoint to see the result */
	
	filesystem_test_ls();
	// i from 0 - 16 should show something
	// i > 16 should report file not found
	int32_t i;
	for (i = 0; i < MAX_NUM_DIRECTORY_ENTRIES; i++)
		test_file_by_index(i);
	test_file_by_name("");										// should fail
	test_file_by_name("\n");									// should fail
	test_file_by_name("cat");									// should success
	test_file_by_name("counter");								// should success
	test_file_by_name("fish");									// should success
	test_file_by_name("frame0.txt");							// should success
	test_file_by_name("frame1.txt");							// should success
	test_file_by_name("grep");									// should success
	test_file_by_name("hello");									// should success
	test_file_by_name("ls");									// should success
	test_file_by_name("pingpong");								// should success
	test_file_by_name("shell");									// should success
	test_file_by_name("sigtest");								// should success
	test_file_by_name("verylargetextwithverylongname.tx");      // should success
	test_file_by_name("verylargetextwithverylongname.txt");     // should fail
	
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
	//disable_irq(1);
	// disable_irq(8);
	
	// clear initial boot information
	clear();
	while(1)
		execute((const uint8_t*)"shell");
}
