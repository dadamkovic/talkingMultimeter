/*
    fpe:       board128.cpp
    Version:    0.1.0
    Date:       Feb. 21, 2013
	License:	GPL v2
    
	atmega128 board code
    
    ****************************************************************************
    Copyright (C) 2013 Radu Motisan  <radu.motisan@gmail.com>

	http://www.pocketmagic.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ****************************************************************************
 */

#include <avr/io.h>
#include "timeout.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/power.h>
#include "sdcard/diskio.h"
//#include "led.h"
//#include "lcd/5110.h"
#include "uart/uart.h"
#include "sdcard/ff.h"
#include "sdcard/integer.h"

DSTATUS disk_initialize (
    BYTE drv        /* Physical drive nmuber (0) */
);
// atmega128 has two leds connected to PA0 and PA1

UART uart0;
FATFS FatFs;	// FatFs work area 
FIL *fp;		// fpe object 


/*---------------------------------------------------------*/
/* User Provided RTC Function called by FatFs module       */
/* Used to provide a Timestamp for SDCard files and folders*/
DWORD get_fattime (void)
{
	// Returns current time packed into a DWORD variable 
	return	  ((DWORD)(2013 - 1980) << 25)	// Year 2013 
	| ((DWORD)8 << 21)				// Month 7 
	| ((DWORD)2 << 16)				// Mday 28 
	| ((DWORD)20 << 11)				// Hour 0..24
	| ((DWORD)30 << 5)				// Min 0 
	| ((DWORD)0 >> 1);				// Sec 0
}



int main(void)
{
	// reboot delay
    clock_prescale_set(clock_div_2);    //8Mhz
	_delay_ms(200);
	
	// init serial link
	uart0.Init(0,1200, false); //uart0:
	
	// init leds
//	led1.Init(&DDRA, &PORTA, 0); //DDRA, PORTA, PA0 // D4 on atmega128 board
//	led2.Init(&DDRA, &PORTA, 1); //DDRA, PORTA, PA1 // D3
	
	// init LCD
//	lcd.init(&PORTC, PC0, &PORTC, PC1, &PORTC, PC2, &PORTC, PC3, &PORTC, PC4);
	
	// init sdcard
	UINT bw;
	disk_initialize (0);
	f_mount(0, &FatFs);		// Give a work area to the FatFs module
	// open file
	fp = (FIL *)malloc(sizeof (FIL));
	if (f_open(fp, "file.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	// Create a file 
		char *text = "Hello World! SDCard support up and running!\r\n";
		f_write(fp, text, strlen(text), &bw);	// Write data to the file 
		f_close(fp);// Close the file 
		if (bw == strlen(text)) { //we wrote the entire string
//			led1.Set(1); // Lights LED if data written well (D4 led on atmega128 board)
//			lcd.goto_xy(0,0); lcd.send_format_string("Created:%dB\n", f_size(fp));
            DDRC |= _BV(PC5);
            PORTC |= _BV(PC5);
            _delay_ms(5000);
            PORTC ^= _BV(PC5);
		}
		//else led2.Set(1);
	}
	// test append
	if (f_open(fp, "file.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {	// Open existing or create new file
		if (f_lseek(fp, f_size(fp)) == FR_OK) 
			{
			char *text2 = "This is a new line, appended to existing file!\r\n";
			f_write(fp, text2, strlen(text2), &bw);	// Write data to the file
			if (bw == strlen(text2)) { //we wrote the entire string
//				lcd.send_format_string("Appended:%dB\n", f_size(fp));
			    DDRC |= _BV(PC5);
			    PORTC |= _BV(PC5);
			    _delay_ms(5000);
			    PORTC ^= _BV(PC5);
			}
			
		}	
		f_close(fp);// Close the file		
	}	
	 char str[12];

	
	// get card volume
	char szCardLabel[12] = {0};
	DWORD sn = 0;
	if (f_getlabel("", szCardLabel, &sn) == FR_OK) {
//		lcd.send_format_string("%s SN:%X\n", szCardLabel, sn);
	}			
	
	// read from file
	if (f_open(fp, "file.txt", FA_READ ) == FR_OK) {	// Create a file
		char text[255]; 
		UINT br;
		f_read(fp, text, 255, &br); 
		f_close(fp);// Close the file
		// cut text the easy way
		text[10] = 0;
//		lcd.send_format_string("Read:%s", text);
	}	

	
	// signaling led
//	led2.Set(1);
	
	// loop
	int i = 0;
	while(1)
	{
		i++;
        PORTC |= _BV(PC5);
        _delay_ms(1000);
        PORTC ^= _BV(PC5);
		_delay_ms(1000);
	    //fp = (FIL *)malloc(sizeof (FIL));
	    f_open(fp, "file.txt", FA_WRITE | FA_CREATE_ALWAYS);// Create a file
	        char *text = "Hello World! SDCard support up and running!\r\n\0";
	        f_puts(text, fp);   // Write data to the file
	        f_close(fp);// Close the file
	        if (bw == strlen(text)) { //we wrote the entire string
	//          led1.Set(1); // Lights LED if data written well (D4 led on atmega128 board)
	//          lcd.goto_xy(0,0); lcd.send_format_string("Created:%dB\n", f_size(fp));
	            DDRC |= _BV(PC5);
	            PORTC |= _BV(PC5);
	            _delay_ms(5000);
	            PORTC ^= _BV(PC5);
	        }
	        //else led2.Set(1);

	}
}
