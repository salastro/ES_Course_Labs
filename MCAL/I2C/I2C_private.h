#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

#define SSPBUF (*((volatile u8 *)0x13))
#define SSPCON (*((volatile u8 *)0x14))
#define SSPCON2 (*((volatile u8 *)0x91))
#define SSPSTAT (*((volatile u8 *)0x94))
#define SSPADD (*((volatile u8 *)0x93))

#define PIR1 (*((volatile u8 *)0x0C))
#define PIE1 (*((volatile u8 *)0x8C))
#define INTCON (*((volatile u8 *)0x0B))

#define SSPM3 3
#define SSPM2 2
#define SSPM1 1
#define SSPM0 0
#define SSPEN 5
#define SSPOV 6
#define WCOL 7

#define SEN 0
#define RSEN 1
#define PEN 2
#define RCEN 3
#define ACKEN 4
#define ACKDT 5
#define ACKSTAT 6
#define GCEN 7

#define BF 0
#define UA 1
#define RW 2
#define S 3
#define P 4
#define D_A 5
#define CKE 6
#define SMP 7

#define SSPIF 3

#define SSPI 3

#define I2C_FOSC 16000000UL

#endif
