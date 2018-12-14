#ifndef PINS_H
#define PINS_H

#include "msp.h"

typedef uint8_t BYTE;

void write_spi(BYTE d);
BYTE receive_spi();

void ground_unused_io();
void initialize_spi();
void uninitialize_spi();
void init_pins();

void initialize_timer3(); //TA3 - overall card timeout
void initialize_timer2(); //TA2 - one response timeout
void reset_timer3();
void reset_timer2();
void start_timer3();
void stop_timer3();
void start_timer2();
void stop_timer2();

#endif
