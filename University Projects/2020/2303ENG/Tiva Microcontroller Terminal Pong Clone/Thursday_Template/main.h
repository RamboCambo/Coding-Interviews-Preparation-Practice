#include "stdint.h"
#include "TM4C123.h"
#include "Lib_2.h"
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

#define SYSCTL_RCGCGPIO (*((uint32_t*) 0x400FE608))
#define SYSCTL_PRGPIO (*((uint32_t*) 0x400FEA08))

#define GPIO_PORTA_DEN (*((uint32_t*) 0x4000451C))
#define GPIO_PORTA_AFSEL (*((uint32_t*) 0x40004420))
#define GPIO_PORTA_PCTL (*((uint32_t*) 0x4000452C))

#define SYSCTL_RCGUART (*((uint32_t*) 0x400FE618))
#define SYSCTL_PRUART (*((uint32_t*) 0x400FEA18))

#define UART0_CTL (*((uint32_t*) 0x4000C030))
#define UART0_IBRD (*((uint32_t*) 0x4000C024))
#define UART0_FBRD (*((uint32_t*) 0x4000C028))
#define UART0_LCRH (*((uint32_t*) 0x4000C02C))

#define UART0_FR (*((uint32_t*) 0x4000C018))
#define UART0_DR (*((uint32_t*) 0x4000C000))

#define NVIC_ST_CTRL (*((uint32_t volatile*) 0xE000E010))
#define NVIC_ST_RELOAD (*((uint32_t volatile*) 0xE000E014))
#define NVIC_ST_CURRENT (*((uint32_t volatile*) 0xE000E018))
#define NVIC_SYS_PRI3 (*((uint32_t volatile*) 0xE000ED20))

void Setup_Potentiometers(void);
void Setup_UART(void);
void U0_GpioSetup();
void U0_Setup();
char U0_ReadChar();
void U0_WriteChar(char c);
void Setup_SysTick(uint32_t ticks);
void U0_Write_Hex(uint32_t num);
void Write_Num(uint32_t number);
void Draw_Left_Paddle(void);
void Draw_Right_Paddle(void);
void Map_Paddle_80p_Range(volatile uint32_t* paddle);
void Setup_Ball(void);
void Draw_Ball(void);
void Print_Winner(int index);
void Draw_Left_Score(void);
void Draw_Right_Score(void);
void Setup_LED_Score(void);
void Print_Message(char* message, int length);
void Print_CSI(void) {
	U0_WriteChar('\e');
	U0_WriteChar('[');
}
void Set_Cursor_Position(int row, int columns) {
	Print_CSI();
	Write_Num(row);
	U0_WriteChar(';');
	Write_Num(columns);
	U0_WriteChar('H');
}


volatile uint32_t left_position = 0;
volatile uint32_t right_position = 0;

int left_score = 0;
int right_score = 0;

volatile uint32_t ball_x = 40;
volatile uint32_t ball_x_diff = -1;

volatile uint32_t ball_y = 12;
volatile uint32_t ball_y_diff = -1;

volatile bool run = true;

char number_0[7][10] = {
	" _______ ",
	"|  _    |",
	"| | |   |",
	"| | |   |",
	"| |_|   |",
	"|       |",
	"|_______|"
};

char number_1[7][10] = {
	"  ____   ",
	" |    |  ",
	"  |   |  ",
	"  |   |  ",
	"  |   |  ",
	"  |   |  ",
	"  |___|  "
};

char number_2[7][10] = {
	" _______ ",
	"|       |",
	"|____   |",
	" ____|  |",
	"| ______|",
	"| |_____ ",
	"|_______|"
};

char number_3[7][10] = {
	" _______ ",
	"|       |",
	"|___    |",
	" ___|   |",
	"|___    |",
	" ___|   |",
	"|_______|"
};

char number_4[7][10] = {
	" _   ___ ",
	"| | |   |",
	"| |_|   |",
	"|       |",
	"|___    |",
	"    |   |",
	"    |___|"
};

char number_5[7][10] = {
	" _______ ",
	"|       |",
	"|   ____|",
	"|  |____ ",
	"|_____  |",
	" _____| |",
	"|_______|"
};

char red_won[7][70] = {
	" ______    _______  ______     _     _  _______  __    _  __         ",
	"|    _ |  |       ||      |   | | _ | ||       ||  |  | ||  |        ",
	"|   | ||  |    ___||  _    |  | || || ||   _   ||   |_| ||  |        ",
	"|   |_||_ |   |___ | | |   |  |       ||  | |  ||       ||  |        ",
	"|    __  ||    ___|| |_|   |  |       ||  |_|  ||  _    ||__|        ",
	"|   |  | ||   |___ |       |  |   _   ||       || | |   | __         ",
	"|___|  |_||_______||______|   |__| |__||_______||_|  |__||__|        "
};

char blue_won[7][70] = {
	" _______  ___      __   __  _______    _     _  _______  __    _  __ ",
	"|  _    ||   |    |  | |  ||       |  | | _ | ||       ||  |  | ||  |",
	"| |_|   ||   |    |  | |  ||    ___|  | || || ||   _   ||   |_| ||  |",
	"|       ||   |    |  |_|  ||   |___   |       ||  | |  ||       ||  |",
	"|  _   | |   |___ |       ||    ___|  |       ||  |_|  ||  _    ||__|",
	"| |_|   ||       ||       ||   |___   |   _   ||       || | |   | __ ",
	"|_______||_______||_______||_______|  |__| |__||_______||_|  |__||__|"
};