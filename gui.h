#ifndef GUI_H
#define GUI_H

#include "lcd.h"

void write_timeout();
void write_checking_card();
void write_bad_card();
void write_checking_card();
void write_slot_empty();
void write_voltage(int from, int to);

#endif
