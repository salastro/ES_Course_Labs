#ifndef PWM_PRIVATE_H
#define PWM_PRIVATE_H

#define CCPR1L (*((volatile u8 *)0x15))
#define CCPR2L (*((volatile u8 *)0x1B))

#define CCP1CON (*((volatile u8 *)0x17))
#define CCP2CON (*((volatile u8 *)0x1D))

#define PR2 (*((volatile u8 *)0x92))

#define T2CON (*((volatile u8 *)0x12))
#define TMR2 (*((volatile u8 *)0x11))

#define PIR1 (*((volatile u8 *)0x0C))
#define PIE1 (*((volatile u8 *)0x8C))

#define CCP1M3 3
#define CCP1M2 2
#define CCP1M1 1
#define CCP1M0 0

#define T2CKPS1 5
#define T2CKPS0 4
#define TMR2ON 2
#define TOUTPS3 6
#define TOUTPS2 5
#define TOUTPS1 4
#define TOUTPS0 3

#define PWM_INPUT_FREQ 1000000UL

#endif
