/*
 * delay.h
 *
 *  Created on: Sep 28, 2018
 *      Author: jpark64
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "msp.h"
#include <stdio.h>
#define freq1_5mhz CS_CTL0_DCORSEL_0 //0.662 us/cycle or 662 ns/cycle
#define freq3mhz CS_CTL0_DCORSEL_1   //0.331 us/cycle or 331 ns/cycle
#define freq6mhz CS_CTL0_DCORSEL_2   //0.165 us/cycle or 165 ns/cycle
#define freq12mhz CS_CTL0_DCORSEL_3  //0.083 us/cycle or 83 ns/cycle
#define freq24mhz CS_CTL0_DCORSEL_4  //0.041 us/cycle or 41 ns/cycle
#define freq48mhz CS_CTL0_DCORSEL_5  //0.020 us/cycle or 20 ns/cycle

void delay_us(int microseconds);

#endif /* DELAY_H_ */
