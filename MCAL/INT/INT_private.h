#ifndef INT_PRIVATE_H
#define INT_PRIVATE_H

#define INTCON (*((volatile u8 *)0x0B))
#define OPTION_REG (*((volatile u8 *)0x81))

#define GIE 7
#define PEIE 6
#define T0IE 5
#define INTE 4
#define RBIE 3
#define T0IF 2
#define INTF 1
#define RBIF 0

#define RBPU 7
#define INTEDG 6
#define T0CS 5
#define T0SE 4
#define PSA 3
#define PS2 2
#define PS1 1
#define PS0 0

#define INT_PORT B
#define INT_PIN 0

#endif
