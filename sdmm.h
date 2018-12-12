#ifndef SDMM_H
#define SDMM_C
#include "msp.h"

typedef uint8_t BYTE;

#define CS_H() {\
    P2->OUT |= BIT6;\
}
#define CS_L() {\
    P2->OUT &= ~BIT6;\
}
#define DL_H() {\
    P2->OUT |= BIT5;\
}
#define DL_L() {\
    P2->OUT &= ~BIT5;\
}

#endif
