#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

#define ADCON0 (*((volatile u8 *)0x1F))
#define ADCON1 (*((volatile u8 *)0x9F))
#define ADRESH (*((volatile u8 *)0x1E))
#define ADRESL (*((volatile u8 *)0x9E))

#define PIR1 (*((volatile u8 *)0x0C))
#define PIE1 (*((volatile u8 *)0x8C))
#define INTCON (*((volatile u8 *)0x0B))

#define ADON 0
#define GO 2
#define CHS2 5
#define CHS1 4
#define CHS0 3
#define ADCS1 7
#define ADCS0 6

#define PCFG3 3
#define PCFG2 2
#define PCFG1 1
#define PCFG0 0

#define ADIF 6

#define ADIE 6

#endif
