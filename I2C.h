
#define SDA     RC4
#define SCL     RC3
#define SCL_DIR     TRISC4
#define SDA_DIR     TRISC3

#define I2C_SPEED 100000 //bps
void I2C_init();
void I2C_start();
__bit I2C_Write_Byte(unsigned char dat);
void I2C_Stop();

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

