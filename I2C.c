#include <xc.h>
#include "I2C.h"
#include "config.h"


void I2C_init()
{
    SDA_DIR = 1;
    SCL_DIR = 1;
    
    SSPADD = ((_XTAL_FREQ/4000)/I2C_SPEED) -1 ;
    
    SSPSTAT = 0x80;
    SSPCON  = 0X28;
}

void I2C_start()
{
    SEN = 1;
    while(!SSPIF);
    SSPIF=0;
}

__bit I2C_Write_Byte(unsigned char dat)
{
    SSPBUF = dat;
    while(!SSPIF);
    SSPIF = 0;
    
    return ACKSTAT;
}
void I2C_Stop()
{
    PEN = 1;
    while(!SSPIF);
    SSPIF = 0;
}
