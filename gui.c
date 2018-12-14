#include "gui.h"

void write_bad_card()
{
    write_4bit(WRITE_B);
    write_4bit(WRITE_a);
    write_4bit(WRITE_d);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_C);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
}
void write_timeout()
{
    write_4bit(WRITE_C);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_T);
    write_4bit(WRITE_i);
    write_4bit(WRITE_m);
    write_4bit(WRITE_e);
    write_4bit(WRITE_o);
    write_4bit(WRITE_u);
    write_4bit(WRITE_t);
}
void write_checking_card()
{
    clear_display();
    write_4bit(WRITE_C);
    write_4bit(WRITE_h);
    write_4bit(WRITE_e);
    write_4bit(WRITE_c);
    write_4bit(WRITE_k);
    write_4bit(WRITE_i);
    write_4bit(WRITE_n);
    write_4bit(WRITE_g);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_c);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_DOT);
    command_4bit(SET_DDRAM_ADDR_40H);
}
void write_slot_empty()
{
    clear_display();
    write_4bit(WRITE_S);
    write_4bit(WRITE_l);
    write_4bit(WRITE_o);
    write_4bit(WRITE_t);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_e);
    write_4bit(WRITE_m);
    write_4bit(WRITE_p);
    write_4bit(WRITE_t);
    write_4bit(WRITE_y);
}
void write_voltage(int from, int to)
{
    int from1[9] = {2,2,2,3,3,3,3,3,3};
    int from2[9] = {7,8,9,0,1,2,3,4,5};
    int to1[9] = {2,2,3,3,3,3,3,3,3};
    int to2[9] = {8,9,0,1,2,3,4,5,6};

    clear_display();
    write_4bit(WRITE_V);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_r);
    write_4bit(WRITE_a);
    write_4bit(WRITE_n);
    write_4bit(WRITE_g);
    write_4bit(WRITE_e);
    write_4bit(WRITE_COLON);
    command_4bit(SET_DDRAM_ADDR_40H);
    write_4bit(WRITE_0+from1[from]);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_0+from2[from]);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_V);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_t);
    write_4bit(WRITE_o);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_0 + to1[to]);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_0 + to2[to]);
    write_4bit(WRITE_V);
}
