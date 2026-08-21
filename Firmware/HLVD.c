#include <p24FJ64GC006.h>

void HLVD_init(void)
{
    // Disable HLVD while configuring
    HLVDCONbits.HLVDEN = 0;

    // Falling voltage detection
    HLVDCONbits.VDIR = 0;

    // HLVDL = 0010
    HLVDCONbits.HLVDL0 = 0;
    HLVDCONbits.HLVDL1 = 1;
    HLVDCONbits.HLVDL2 = 1;
    HLVDCONbits.HLVDL3 = 0;

    // Enable HLVD
    HLVDCONbits.HLVDEN = 1;

    // Wait for internal reference to stabilize
    while (!HLVDCONbits.IRVST);

    // Configure interrupt
    IFS4bits.HLVDIF = 0;      // clear old/pending interrupt
    IPC18bits.HLVDIP = 4;     // priority 4
    IEC4bits.HLVDIE = 1;      // enable interrupt
}