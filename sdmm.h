#ifndef SDMM_H
#define SDMM_C
#include "msp.h"
#include "delay.h"
#include "periph.h"
#include <stdio.h>

#define CS_H() {\
    P2->OUT |= BIT6;\
}

#define CS_L() {\
    P2->OUT &= ~BIT6;\
}

#define SCK_H() {\
    P1->OUT |= BIT5;\
}

#define SCK_L() {\
    P1->OUT &= ~BIT6;\
}

#define DI_H() {\
    P1->OUT |= BIT6;\
}

#define DI_L() {\
    P1->OUT &= ~BIT6;\
}

#define DO_L() {\
    P1->OUT &= ~BIT7;\
}

#define DO_H() {\
    P1->OUT |= BIT7;\
}

#define VCC_ON() {\
    P2->OUT |= BIT4;\
}

#define VCC_OFF() {\
    P2->OUT &= ~BIT4;\
}
#define DL_H() {\
    P2->OUT |= BIT5;\
}

#define DL_L() {\
    P2->OUT &= ~BIT5;\
}
int poll_sd(BYTE *cmd, int respbytes);
void send_cmd(BYTE *cmd);
void get_response(int n);
void print_response(int cmd, int n);

void disable_reader();
void enable_reader();

int from_vrange();
int to_vrange();

int init_sequence2();
void initialize_spi_mode0();
void initialize_spi_mode2();
void initialize_spi_mode3();
#endif
