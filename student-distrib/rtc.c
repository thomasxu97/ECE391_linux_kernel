#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "filesystem.h"

#define	REGISTER_A 				0x0A
#define REGISTER_B 				0x0B
#define REGISTER_C 				0x0C				


#define REGISTER_A_NMI			0x8A
#define REGISTER_B_NMI			0x8B

#define CMOS_PORT 				0X70
#define CMOS_DATA_PORT 			0x71

#define RTC_RATE 				13
#define RTC_RATE_TWO			15
#define PERIODIC_INTERRUPT_MASK 0x40
#define RATE_MASK 				0xF0

#define	RTC_IRQ					8
#define ASCII_ZERO				48 	
#define LIMIT					13			
#define DIGIT					10		
#define BINARY_DIGIT 			2
#define RTC_BASE 				32768
#define REENABLE_MASK			0x7F
#define WRITE_TO_RAM 			0x20
#define SCREEN_COLUMN_LIMIT 	79

/* https://courses.engr.illinois.edu/ece391/sp2019/secure/references/ds12887.pdf */
#define RTC_1024    1024
#define RTC_512     512
#define RTC_256     256
#define RTC_128     128
#define RTC_64      64
#define RTC_32      32
#define RTC_16      16
#define RTC_8       8
#define RTC_4       4
#define RTC_2       2
/* lower 4 bits hold rate */
#define RTC_1024H    0x06
#define RTC_512H     0x07
#define RTC_256H     0x08
#define RTC_128H     0x09
#define RTC_64H      0x0A
#define RTC_32H      0x0B
#define RTC_16H      0x0C
#define RTC_8H       0x0D
#define RTC_4H       0x0E
#define RTC_2H       0x0F

volatile int rtc_interrupt_flag=0;

/*
static int pow(int a, int b){
int ret=1;
for(;b>0;b--){
	ret*=a;
}
return ret;
}
*/
/* void rtc_init();
 * Description: initialization of rtc
 * Inputs:  none
 * Return Value: none
 * Function: call function cli() in i8259.c 
 *			 call function outb() in lib.h
 *			 call function enable_irq() in i8259.c
 *			 call function sti() in lib.h
 */
void rtc_init() {

	cli();

	/* Initialize RTC */
	outb(REGISTER_A_NMI, CMOS_PORT);							// select register A w/ NMI disabled
	outb(WRITE_TO_RAM, CMOS_DATA_PORT);							// write to CMOS/RTC RAM
	outb(REGISTER_B_NMI, CMOS_PORT);							// select register B, and disable NMI

	uint8_t previous = inb(CMOS_DATA_PORT);						// read the value of register 8
	outb(REGISTER_B_NMI, CMOS_PORT);							// set the index again, read sets the index to register D again

	outb(previous | PERIODIC_INTERRUPT_MASK, CMOS_DATA_PORT);	// first OR previous and 0x40, then write. It turns on bit 6 of register B

	/* Set Rate */
	outb(REGISTER_A_NMI, CMOS_PORT);							// select register A
	previous = inb(CMOS_DATA_PORT);
	outb(REGISTER_A_NMI, CMOS_PORT);							// select register A
	outb((previous & RATE_MASK) | RTC_RATE, CMOS_DATA_PORT);	// set rate to register A

	enable_irq(RTC_IRQ);

	sti();
	outb(inb(CMOS_PORT) & REENABLE_MASK, CMOS_PORT);						// re-enable NMI
	
	return;
}



/* void rtc_interrupt_handler();
 * Description: interrupt handler of rtc
 * Inputs:  none
 * Return Value: none
 * Function: call function cli() in lib.h
 *			 call function outb() in lib.h
 *			 call function inb() in lib.h
 *			 call function send_eoi() in i8259.c
 *			 call function sti() in lib.h
 */
void rtc_interrupt_handler() {
	// int flag=0;
	// int old_y;
	// printf("rtc_interrupt_handler called after modify twice\n");
	cli();

	outb(REGISTER_C, CMOS_PORT);								// select register C
	inb(CMOS_DATA_PORT);										// discard the data

	// printf("RTC interrupt received!\n");
	// test_interrupt();
	// set interrupt flag
	rtc_interrupt_flag=1;

	// old_y=get_screen_y();

	// if(get_screen_x()==SCREEN_COLUMN_LIMIT)
	// 	flag=1;
	// printf("1");

	// if (flag==1)
	// 	set_cursor(0,old_y+1);

	send_eoi(RTC_IRQ);
	sti();

	return;
}
/*int rtc_open();
 * Description: open rtc with default 2HZ
 * Inputs: none
 * Return Value: success with 0
 * Function: call function outb() in lib.h
 */
int rtc_open(const uint8_t* filename){
	directory_entry_t file_to_open;
    uint32_t fd;
    // check all the cases that can result to failure
    if (read_dentry_by_name(filename, &file_to_open) == -1)
        return -1;
    if (file_to_open.file_type != RTC_FILE_TYPE)
        return -1;
    fd = allocate_file_descriptor();
    if (fd == -1)
        return -1;
    // open success, set file descriptor structure
    global_file_descriptor[fd].file_operations_table_pointer = &rtc_jump_table;
    global_file_descriptor[fd].index_node_number = -1;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 1;
	// set frequency to 2 HZ
	outb(REGISTER_A_NMI, CMOS_PORT);
	uint8_t previous= inb(CMOS_DATA_PORT);
	outb(REGISTER_A_NMI,CMOS_PORT);
	outb((previous & RATE_MASK) | RTC_RATE_TWO, CMOS_DATA_PORT);
	return fd;
}

/* int rtc_close;
 * Description: close rtc without any action 
 * Inputs: none
 * Return Value: success with 0
 * Function: nothing
 */
int rtc_close(int32_t fd){
	// restore default settings
    global_file_descriptor[fd].file_operations_table_pointer = NULL;
    global_file_descriptor[fd].index_node_number = 0;
    global_file_descriptor[fd].file_position = 0;
    global_file_descriptor[fd].flags = 0;
	return 0;
}

/* int rtc_write(int32_t fd, int8_t* buf, uint32_t nbytes);
 * Description: write the buf as HZ into rtc and set rtc with corresponding rate
 * Inputs: file descriptor, buffer, size of buffer to read
 * Return Value: success with 0 failure with -1
 * Function: call function outb() in lib.h
 */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	if(global_file_descriptor[fd].flags==0)
        return -1;

	/*
	int frequency=0;
	int i;
	int flag=0;
	int rate=1;
	uint8_t previous;
	uint8_t* buf_ref = (uint8_t*)buf;
	*/
	
	//check initial condition
	// printf("buffer is %s and nbytes is %d \n",buf_ref,nbytes);
	if(buf ==NULL || nbytes<= 0 ){
		return -1;
	}

	rtc_set_rate(*((int32_t*)buf));

	/*
	// printf("reached 1 with nbytes %d\n",nbytes);
	for(i=nbytes-1;i>=0;i--,buf_ref++){
		// printf("buf at position is %d and i is %d 10^i is %d the calculation is %d\n",((*buf_ref)-48),i,pow(10,i),((*buf_ref)-48)*pow(10,i));
		frequency+=((*buf_ref)-ASCII_ZERO)*pow(DIGIT,i);
		// printf("for i is %d frequency is %d\n",i,frequency);
	}
	// check power of 2
	// printf("reached 2\n");
	i=0;
	while (i<=LIMIT){
		if(frequency==pow(BINARY_DIGIT,i)){
			flag=1;
			break;
		}
		i++;
	}
	// if it is not power of 2 just return -1, else set the rate
	if(flag==0){
		return -1;
	}
	else{
		// get rate from frequency
		// printf("start to loop given buffer is %s\n",buf_ref);
		while(frequency!= RTC_BASE >> (rate-1)){
			rate++;
			// printf("now rate is %d and frequency is %d given buffer is %s\n",rate,frequency,buf_ref);
		}
		// printf("it is out of loop with rate %d\n", rate);
		// clear();
		outb(REGISTER_A_NMI,CMOS_PORT);
		previous=inb(CMOS_DATA_PORT);
		outb(REGISTER_A_NMI,CMOS_PORT);
		outb((previous & RATE_MASK) | rate, CMOS_DATA_PORT);
	}
	*/
	return 0;
}

/* int rtc_read(int32_t fd, int8_t* buf, uint32_t nbytes);
 * Description: wait for interrupt 
 * Inputs: file descriptor, buffer, size of buffer to read
 * Return Value: success with 0 
 * Function: call function cli() in lib.h
 *			 call function sti() in lib.h
 */

int rtc_read(int32_t fd, void* buf, int32_t nbytes){
// clear flag
	cli();
	rtc_interrupt_flag=0;
	sti();
// wait for interrupt
	while(!rtc_interrupt_flag);
	return 0;
}

/* rtc_set_rate
 *		DESCRIPTION: sets the rate for the rtc to a power of 2 up to max 1024Hz.
				     Defaults to 2Hz if frequency is not valid
 *		INPUTS: frequency - frequency to set RTC to
 *		OUTPUTS: NONE
 *		RETURN: NONE
 *		SIDE EFFECT: changes RTC rate.
 */
void rtc_set_rate(int32_t frequency){
    uint8_t rate;

    outb(REGISTER_A_NMI, CMOS_PORT);
    char prev = inb(CMOS_DATA_PORT);

    switch(frequency){
        case RTC_1024:
            rate = RTC_1024H;
            break;

        case RTC_512:
            rate = RTC_512H;
            break;

        case RTC_256:
            rate = RTC_256H;
            break;

        case RTC_128:
            rate = RTC_128H;
            break;

        case RTC_64:
            rate = RTC_64H;
            break;

        case RTC_32:
            rate = RTC_32H;
            break;

        case RTC_8:
            rate = RTC_8H;
            break;

        case RTC_4:
            rate = RTC_4H;
            break;

        case RTC_2:
            rate = RTC_2H;
            break;

        /* invalid rates */
        default:
            printf("Not a valid freqency, defaulting to 2Hz\n\n");
            return;
        }

    outb(REGISTER_A_NMI, CMOS_PORT);
    outb((prev & RATE_MASK) | rate, CMOS_DATA_PORT);
}
