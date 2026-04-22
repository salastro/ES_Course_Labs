#ifndef UART_PRIVATE_H
#define UART_PRIVATE_H

#define TXREG (*((volatile u8 *)0x19))
#define RCREG (*((volatile u8 *)0x1A))
#define SPBRG (*((volatile u8 *)0x99))

#define RCSTA (*((volatile u8 *)0x18))
#define TXSTA (*((volatile u8 *)0x98))

#define PIR1 (*((volatile u8 *)0x0C))
#define PIE1 (*((volatile u8 *)0x8C))
#define INTCON (*((volatile u8 *)0x0B))

#define RX9D 0
#define OERR 1
#define FERR 2
#define ADDEN 3
#define CREN 4
#define SREN 5
#define RX9 6
#define SPEN 7

#define TX9D 0
#define TRMT 1
#define BRGH 2
#define SYNC 4
#define SENDB 3
#define TX9 6
#define TXEN 5

#define RCIF 5
#define TXIF 4

#define RCIE 5
#define TXIE 4

#define UART_FOSC 16000000UL

#endif
