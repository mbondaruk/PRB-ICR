/*
 * delay.c
 *
 *  Created on: Sep 28, 2018
 *      Author: jpark64
 */
#include "delay.h"

void delay_us(int microseconds) {
   int time = microseconds * 100;
   int i;
   int clock = CS->CTL0 & CS_CTL0_DCORSEL_MASK;

   if (clock == freq1_5mhz) {
       for (i = time; i > 3400; i = i - 662) {
       }
   }
   else if (clock == freq3mhz) {
       for (i = time; i > 1700; i = i - 331) {
          }
   }
   else if (clock == freq6mhz) {
       for (i = time; i > 850; i = i - 165) {
          }
   }
   else if (clock == freq12mhz) {
       for (i = time; i > 425; i = i - 83) {
          }
   }
   else if (clock == freq24mhz) {
       for (i = time; i > 212; i = i - 41) {
          }
   }
   else if (clock == freq48mhz) {
       for (i = time; i > 106; i = i -20) {
          }
   }
}


