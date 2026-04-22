#ifndef SPI_PRIVATE_H
#define SPI_PRIVATE_H

#define SSPBUF (*((volatile u8 *)0x13))
#define SSPCON (*((volatile u8 *)0x14))
#define SSPSTAT (*((volatile u8 *)0x94))

#define PIR1 (*((volatile u8 *)0x0C))
#define PIE1 (*((volatile u8 *)0x8C))
#define INTCON (*((volatile u8 *)0x0B))

#define SSPM3 3
#define SSPM2 2
#define SSPM1 1
#define SSPM0 0
#define CKP 4
#define SSPEN 5
#define SSPOV 6
#define WCOL 7

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

#endif
