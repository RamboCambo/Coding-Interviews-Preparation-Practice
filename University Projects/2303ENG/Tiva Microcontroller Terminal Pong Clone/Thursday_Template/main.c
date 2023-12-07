#include "main.h"

void SysTick_Handler() {
	Print_CSI();
	U0_WriteChar('2');
	U0_WriteChar('J');
	
	Draw_Left_Score();
	Draw_Right_Score();
	Draw_Left_Paddle();
	Draw_Right_Paddle();
	Draw_Ball();

	if(!run) {
		if(left_score == 5) {
			Print_Winner(1);
			while(U0_ReadChar() != ' ');
			left_score = 0;
			right_score = 0;
			run = true;
			ball_x = 40;
			ball_y = 12;
			ball_x_diff = -1;
			ball_y_diff = -1;
		}
		else if(right_score == 5) {
			Print_Winner(0);
			while(U0_ReadChar() != ' ');
			left_score = 0;
			right_score = 0;
			run = true;
			ball_x = 40;
			ball_y = 12;
			ball_x_diff = -1;
			ball_y_diff = -1;
		}
		else {
			while(U0_ReadChar() != ' ');
			run = true;
			ball_x = 40;
			ball_y = 12;
			ball_x_diff *= -1;
			ball_y_diff = -1;
		}
	}
}

void TIMER0A_Handler() {
	if(run) {
		ADC0->PSSI |= 0x2;
		
		while((ADC0->RIS & 0x2) == 0);
		
		left_position = ADC0->SSFIFO1 & 0xFFF;
		Map_Paddle_80p_Range(&left_position);
		left_position = (uint32_t) ((double) left_position / (double) 0xFFF * (double) 20.0);
		
		right_position = ADC0->SSFIFO1 & 0xFFF;
		Map_Paddle_80p_Range(&right_position);
		right_position = (uint32_t) ((double) right_position / (double) 0xFFF * (double) 20.0);
		
		ADC0->ISC |= 0x2;
	}
		
	TIMER0->ICR |= 0x1;
}

void TIMER1A_Handler() {
	if(run) {
		ball_x += ball_x_diff;
		ball_y += ball_y_diff;
		
		if(ball_x == 2 && (left_position <= ball_y && ball_y <= left_position + 4)) {
			ball_x_diff = 1;
			if(ball_y == left_position || ball_y == left_position + 1)
				ball_y_diff = -1;
			if(ball_y == left_position + 2)
				ball_y_diff = 0;
			if(ball_y == left_position + 3 || ball_y == left_position + 4)
				ball_y_diff = 1;
		}
		if(ball_x == 79 && (right_position <= ball_y && ball_y <= right_position + 4)) {
			ball_x_diff = -1;
			if(ball_y == right_position || ball_y == right_position + 1)
				ball_y_diff = -1;
			if(ball_y == right_position + 2)
				ball_y_diff = 0;
			if(ball_y == right_position + 3 || ball_y == right_position + 4)
				ball_y_diff = 1;
		}
		
		if(ball_x == 80) {
			left_score++;
			run = false;
		}
		if(ball_x == 1) {
			right_score++;
			run = false;
		}
		
		if(ball_y == 24)
			ball_y_diff = -1;
		if(ball_y == 1)
			ball_y_diff = 1;
	}
	
	TIMER1->ICR |= 0x1;
}


void TIMER2A_Handler() {
	GPIOE->DATA |= 0x1F;
	// turn off the columns before we switch the rows so there isn't any residual light coming from pins which shouldn't be lit
	GPIOA->DATA ^= 0x3 << 6;
	
	// if it is red's turn
	if((GPIOA->DATA & 0x1 << 6) == 0x1 << 6) {
		GPIOE->DATA &= ~(0x1F >> (5 - left_score));
	}
	// else it's blue's turn
	else {
		GPIOE->DATA &= ~(0x1F >> (5 - right_score));
	}
	
	TIMER2->ICR |= 0x1;
}

int main() {
	SetSysClock_80MHz();
	
	__disable_irq();
	Setup_Potentiometers();
	
	//Initialise_Uart0_GPIO
	U0_GpioSetup();
	//Initialise_UART0() ;
	U0_Setup();
	// 10 fps
	Setup_SysTick(8000000);
	Setup_Ball();
	Setup_LED_Score();
	
	// clear screen
	Print_CSI();
	U0_WriteChar('2');
	U0_WriteChar('J');
	
	Set_Cursor_Position(1, 1);
	
	// hide cursor
	Print_CSI();
	U0_WriteChar('?');
	Write_Num(25);
	U0_WriteChar('l');
	
	// set background to bright white
	Print_CSI();
	Write_Num(107);
	U0_WriteChar('m');
	
	// set foreground colour to black
	Print_CSI();
	Write_Num(30);
	U0_WriteChar('m');
	
	// wait until the user presses space
	char* start_message = "Press SPACE to begin the game!";
	Print_Message(start_message, 30);
	
	while(U0_ReadChar() != ' ');
	
	// set initial LED state
	GPIOA->DATA |= 0x1 << 7;
	GPIOE->DATA |= 0x1F;
	
	__enable_irq();
	
	while(1) {
		asm("nop");
	}
	
	//return 0;
}

void Setup_Potentiometers(void) {		
	// Enable GPIOB
	SYSCTL->RCGCGPIO |= 0x2;
	while((SYSCTL->PRGPIO & 0x2) == 0);
	
	// Setup PB4, PB5 for analog input
	GPIOB->DIR &= ~(0x3 << 4);
	GPIOB->AFSEL |= 0x3 << 4;
	GPIOB->DEN &= ~(0x3 << 4);
	GPIOB->AMSEL |= 0x3 << 4;
	
	// Enable ADC0
	SYSCTL->RCGCADC |= 0x1;
	while((SYSCTL->PRADC & 0x1) == 0);
	
	// Wait 3 clock cycles
	for(int i = 0; i < 3; i++);
	
	// Setup ADC0
	ADC0->ACTSS &= ~0x2;
	ADC0->EMUX &= ~(0xF << 4);
	ADC0->IM |= 0x2;
	ADC0->SSMUX1 &= ~0xFF;
	ADC0->SSMUX1 |= 0xAB;
	ADC0->SSCTL1 |= 0x3 << 5;
	ADC0->ACTSS |= 0x2;
	
	// Enable Timer0
	SYSCTL->RCGCTIMER |= 0x1;
	while((SYSCTL->PRTIMER & 0x1) == 0);
	
	// Setup Timer0
	NVIC->IP[19] &= ~(0x7 << 5);
	TIMER0->CTL &= ~0x1;
	TIMER0->CFG = 0x00000000;
	TIMER0->TAMR = 0x2;
	TIMER0->TAILR = 8000000;
	TIMER0->IMR |= 0x1;
	TIMER0->CTL |= 0x1;
	NVIC->ISER[0] |= 0x1 << 19;
}

void U0_GpioSetup() {
	// enable port A
	SYSCTL_RCGCGPIO |= 0x1;
	// wait for port a to stabilise
	while((SYSCTL_PRGPIO & 0x1) == 0);
	
	// enable the pin for digital
	GPIO_PORTA_DEN |= 0x3;
	
	// enable pins 0 and 1 for alternate function
	GPIO_PORTA_AFSEL |= 0x3;
	
	// clear alterate functions for 
	GPIO_PORTA_PCTL &= ~0xFF;
	
	// enable alternate function of UART for pins 0 and 1
	GPIO_PORTA_PCTL |= 0x11;
}

void U0_Setup() {
	// enable UART 0
	SYSCTL_RCGUART |= 0x1;
	// wait for UART 0 to stabilise
	while((SYSCTL_PRUART & 0x1) == 0);
	
	// CTL diable uart
	UART0_CTL &= ~0x1;
	
	// clear IBRD
	UART0_IBRD &= ~0xFFFF;
	
	// set IBRD to 86 = 0x56
	UART0_IBRD |= 0x56;
	
	// clear FBRD
	UART0_FBRD &= ~0x3F;
	
	// set FBRD to 52 = 0x34
	UART0_FBRD |= 0x34;
	
	// clear LCRH
	UART0_LCRH &= ~0xFF;
	
	// set LCRH to 8 bits word length, odd parity
	UART0_LCRH |= 0x62;
	
	// CTL enable UART, RX, TX, prescalar of ÷8
	UART0_CTL |= 0x321;
}

char U0_ReadChar() {
	// wait for data to be available to read
	while((UART0_FR & 0x10) == 0x10);
	
	// mask the content bits and return as a char
	return (char) (UART0_DR & 0xFF);
}

void U0_WriteChar(char c) {
	// wait till the buffer is not full
	while((UART0_FR & 0x20) == 0x20);
	
	// set the data to the char with the mask of 8 bits
	UART0_DR = (c & 0xFF);
}

void Setup_SysTick(uint32_t ticks) {
	NVIC_ST_CTRL &= (uint32_t) ~0x1;
	NVIC_ST_RELOAD = ticks;
	NVIC_ST_CURRENT &= (uint32_t) ~0xFFFFFF;
	NVIC_ST_CTRL |= 0x6;
	NVIC_SYS_PRI3 &= ~((uint32_t) 0x7 << 29);
	NVIC_SYS_PRI3 |= 0x2 << 29;
	NVIC_ST_CTRL |= 0x1;
}

void U0_Write_Hex(uint32_t num) {
	while((UART0_FR & 0x20) == 0x20);
	
	// set the data to the char with the mask of 8 bits
	UART0_DR |= (num & 0xFF);
}

void Write_Num(uint32_t number) {
	char chars[11];
	
	int i = 0;
	while(number != 0) {
		chars[i] = (number % 10) + 0x30;
		number /= 10;
		i++;
	}
	
	--i;
	while(i >= 0) {
		U0_WriteChar(chars[i]);
		--i;
	}
}

void Draw_Left_Paddle(void) {
	Set_Cursor_Position(left_position, 0);
	
	// set colour to blue
	Print_CSI();
	Write_Num(31);
	U0_WriteChar('m');
	
	U0_WriteChar('|');
	for(int i = 0; i < 4; i++) {
		Print_CSI();
		U0_WriteChar('1');
		U0_WriteChar('E');
		U0_WriteChar('|');
	}
}
void Draw_Right_Paddle(void) {

	Set_Cursor_Position(right_position, 80);
	
	// set colour to red
	Print_CSI();
	Write_Num(34);
	U0_WriteChar('m');
	
	U0_WriteChar('|');
	for(int i = 0; i < 4; i++) {
		Print_CSI();
		U0_WriteChar('1');
		U0_WriteChar('B');
		U0_WriteChar('|');
	}
}

void Map_Paddle_80p_Range(volatile uint32_t* paddle) {
	if(*paddle < 0x19A)
		*paddle = 0x19A;
	if(*paddle > 0xE65)
		*paddle = 0xE65;
	*paddle = (int) (1.2504 * (double) *paddle - 512.664);
}

void Setup_Ball(void) {
	SYSCTL->RCGCTIMER |= 0x2;
	while((SYSCTL->PRTIMER & 0x2) == 0);
	
	NVIC->IP[21] &= ~(0x7 << 5);
	NVIC->IP[21] |= 0x1 << 5;
	TIMER1->CTL &= ~0x1;
	TIMER1->CFG = 0x00000000;
	TIMER1->TAMR = 0x2;
	TIMER1->TAILR = 4000000;
	TIMER1->IMR |= 0x1;
	TIMER1->CTL |= 0x1;
	NVIC->ISER[0] |= 0x1 << 21;
}

void Draw_Ball(void) {
	Set_Cursor_Position(ball_y, ball_x);
	
	// set colour to black
	Print_CSI();
	Write_Num(30);
	U0_WriteChar('m');
	
	U0_WriteChar('0');
}

void Print_Winner(int index) {
	char (*message)[7][70] = &blue_won;
	
	if(index == 0) {
		// set colour to blue
		Print_CSI();
		Write_Num(34);
		U0_WriteChar('m');
	}
	if(index == 1) {
		// set colour to red
		Print_CSI();
		Write_Num(31);
		U0_WriteChar('m');
		message = &red_won;
	}
	
	for(int i = 0; i < 7; i++) {
		Set_Cursor_Position(10 + i, 7);
		
		Print_Message((*message)[i], 69);
	}	
}

void Draw_Left_Score(void) {
	char (*num)[7][10] = &number_0;
	
	switch(left_score) {
		case 1:
			num = &number_1;
			break;
		case 2:
			num = &number_2;
			break;
		case 3:
			num = &number_3;
			break;
		case 4:
			num = &number_4;
			break;
		case 5:
			num = &number_5;
			break;
	}
	
	// set colour to red
	Print_CSI();
	Write_Num(31);
	U0_WriteChar('m');
	
	for(int i = 0; i < 7; i++) {
		Set_Cursor_Position(2 + i, 21);
		
		Print_Message((*num)[i], 9);
	}
}

void Draw_Right_Score(void) {
	char (*num)[7][10] = &number_0;
	
	switch(right_score) {
		case 1:
			num = &number_1;
			break;
		case 2:
			num = &number_2;
			break;
		case 3:
			num = &number_3;
			break;
		case 4:
			num = &number_4;
			break;
		case 5:
			num = &number_5;
			break;
	}
	
	// set colour to blue
	Print_CSI();
	Write_Num(34);
	U0_WriteChar('m');
	
	for(int i = 0; i < 7; i++) {
		Set_Cursor_Position(2 + i, 49);
		
		Print_Message((*num)[i], 9);
	}
}

void Setup_LED_Score(void) {
	SYSCTL->RCGCGPIO |= 0x11;
	// wait for port A and E to enable
	while((SYSCTL->PRGPIO & 0x11) != 0x11);
	
	// set port e pins from 0-4, and a 6-7, to output, digital, 4 mA
	GPIOE->DIR |= 0x1F;
	GPIOE->AFSEL &= ~0x1F;
	GPIOE->DR4R |= 0x1F;
	GPIOE->DEN |= 0x1F;
	GPIOE->AMSEL &= ~0x1F;
	
	GPIOA->DIR |= 0x3 << 6;
	GPIOA->AFSEL &= ~(0x3 << 6);
	GPIOA->DR4R |= 0x3 << 6;
	GPIOA->DEN |= 0x3 << 6;
	GPIOA->AMSEL &= ~(0x3 << 6);
	
	SYSCTL->RCGCTIMER |= 0x4;
	while((SYSCTL->PRTIMER & 0x4) == 0);
	
	NVIC->IP[23] &= ~(0x7 << 5);
	// 3rd priority
	NVIC->IP[23] |= 0x3 << 5;
	TIMER2->CTL &= ~0x1;
	TIMER2->CFG = 0x00000000;
	TIMER2->TAMR = 0x2;
	TIMER2->TAILR = 100000;
	TIMER2->IMR |= 0x1;
	TIMER2->CTL |= 0x1;
	NVIC->ISER[0] |= 0x1 << 23;
}

void Print_Message(char* message, int length) {
	for(int i = 0; i < length; i++) {
		U0_WriteChar(message[i]);
	}
}
