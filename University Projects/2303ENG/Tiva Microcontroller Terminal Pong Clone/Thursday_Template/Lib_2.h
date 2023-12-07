#ifndef LIB_2_H
#define LIB_2_H

// Serial
extern void Initialise_UART0(void) ;
extern void Initialise_UART0_GPIO(void) ;
extern char Read_UART0(void) ;
extern void Write_UART0(char) ;

// clock speed setting
extern void SetSysClock_80MHz(void) ;




#endif /* Lib_2.h */
