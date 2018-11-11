/*
 * talking.cpp
 *
 *  Created on: Nov 11, 2018
 *      Author: daniel
 */

#include <avr/io.h>
#include "inttypes.h"
#include "sdcard/diskio.h"
#include "sdcard/ff.h"
#include "sdcard/integer.h"
#include "timeout.h"
#include <stdlib.h>

FATFS FatFs;    // FatFs work area
FIL *fp;

uint8_t initTalking(){
    TCCR1A |= _BV(WGM10)|_BV(COM1A1);  //8 bit FAST PWM set on OCCR1A
    OCR1A = 126;
    TCCR1B |= _BV(CS10);               //1 prescaler

    TCCR0A |= _BV(WGM01);              //CTC mode, TOP = OCR0A
    OCR0A = 61;                        //8kHz sample freqency
    TIMSK0 |= _BV(OCIE0A);              //interrupt when reaching TOP
    TCCR0B |= _BV(CS01);               //8 prescaler

    disk_initialize (0);
    f_mount(0, &FatFs);
    fp = (FIL *)malloc(sizeof (FIL));
    return 0;
}
/*
 * INPUT: takes pointer to buffer of 40 uint8_t where we want to store data
 * FUNCTION: gets 10 bytes of data from SD card
 * OUTPUT: void, only manipulates buffer we give it
 */
void unpack(uint8_t* unpackedDataBuffer){
    uint8_t dataBuffer[10];
    UINT br;
    uint8_t mask[40] = {0b00000011,0b00001100,0b00110000,0b11000000};
    f_read(fp,dataBuffer,10,&br);           //will load 10 bytes from SD card at once
    for(uint8_t i =0;i<10;i++){
        for(uint8_t j = 0;j<4;j++){
            unpackedDataBuffer[(i+1)*j] = (dataBuffer & mask[j])>>(j*2);
        }
    }
}





/*
 * INPUT: filename in string format
 * FUNCTION: opens file for work
 * OUTPUT: 1-error, 0-OK
 */
uint8_t openFile(char* fileName){
    if(f_open(fp, fileName, FA_READ ) == FR_OK) return 0;
    else return 1;
}
/*
 * FUNCTION: closes curr. file
 */
void closeFile(){
    f_close(fp);
}

