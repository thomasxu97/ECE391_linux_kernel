#ifndef ASM
#include "types.h"
// initialize for rtc, deal with register and write value
void rtc_init();

// interrupt handler for rtc, deal with register
void rtc_interrupt_handler();

// open rtc with default 2HZ
int rtc_open(const uint8_t* filename);

// close rtc without any action 
int rtc_close(int32_t fd);

// write the buf as HZ into rtc and set rtc with corresponding rate
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

// wait for interrupt 
int rtc_read(int32_t fd, void* buf, int32_t nbytes);

// sets rtc rate
void rtc_set_rate(int32_t frequency);

#endif
