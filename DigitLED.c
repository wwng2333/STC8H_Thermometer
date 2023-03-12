#include "STC8H.h"
#include "DigitLED.h"

void DigitLED_Init(void)
{
    P_SW2 |= 0x80;
    COMEN = 0x83;
    SEGENH = 0xFF;
    LEDCKS = 7;
    COM0_DC_L = 0xFF;
    COM0_DC_H = 0xFF;
    LEDCTRL = 0x80;
}