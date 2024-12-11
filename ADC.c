#include <xc.h>
#include <stdlib.h>
#include "ADC.h"
#define _XTAL_FREQ 20000000
void ADC_init()
{
    ADCON0 = 0x00;
    ADCON1 = 0xCE;    
}
int ADC_read()
{
  
    ADCON0bits.CHS = 0;  
    ADON = 1; //turn on ADC
    __delay_ms(1);  
    GO_DONE = 1;
    unsigned int ADC_val = (ADRESH<<8) + ADRESL;
    return ADC_val;
}


