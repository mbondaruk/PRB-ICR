/*
 * setDCO.c
 *
 *  Created on: Sep 28, 2018
 *      Author: jpark64
 */
#include "msp.h"
#include "setDCO.h"


void setDCO(int freq) {
    CS->KEY = CS_KEY_VAL;  //Unlock CS registers
    CS->CTL0 = 0;       //clear register CTL0
    CS->CTL1 = (CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3);

    if (freq == freq48mhz) {
        /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
           PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
           /* Configure Flash wait-state to 1 for both banks 0 & 1 */
           FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK))
                   | FLCTL_BANK0_RDCTL_WAIT_1;
           FLCTL->BANK1_RDCTL  = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK1_RDCTL_WAIT_MASK))
                   | FLCTL_BANK1_RDCTL_WAIT_1;
       CS->CTL0 = freq48mhz;
    }
    else {
        CS->CTL0 = freq;
    }
    CS->KEY = 0; //Lock CS
}



