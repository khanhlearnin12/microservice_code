//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
//
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"           // Device header

// display an integer on four 7-segment LEDs
void Display_7seg(uint16_t value)
{
  uint8_t digit;
	digit = value / 1000;
	CloseSevenSegment();
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
			
	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}

void Display_7seg_digit(uint16_t value)
{
	// digit 
	CloseSevenSegment();
	ShowSevenSegment(0,value); 
	CLK_SysTickDelay(5000);
}

void GPIO_init()
{
  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
}

void buzz_init()
{
	// initiale for buzz 
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); // for Buzzer
}

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(100000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(100000);	 // Delay 
	}
}

void gpio_display(int s)
{
	PC15 = (s & 0x01) ? 0 : 1; 
	PC14 = (s & 0x02) ? 0 : 1;
	PC13 = (s & 0x04) ? 0 : 1; 
	PC12 = (s & 0x08) ? 0 : 1;  
}

uint16_t show_number(int s)
{
	uint8_t bit0 = (s >> 0) & 1; 
	uint8_t bit1 = (s >> 1) & 1; 
	uint8_t bit2 = (s >> 2) & 1;
	uint8_t bit3 = (s >> 3) & 1; 
	uint16_t sum = (bit3 * 1000)+ (bit2 * 100)+ (bit1 * 10)+ (bit0*1);
	return sum;
}

void ex3_1(void)
{
	while(1) 
	{
		int s = ScanKey();
		int studentID[7] = {1,2,6,6,3,3,4};
		if (s != 0)
		{
			if(s == 8 || s == 9) continue;
			else
			{
				Display_7seg_digit(studentID[s-1]);
				gpio_display(studentID[s-1]);
			}
		}
	}
}

void ex3_2(void)
{
	int last_s = 0;
	uint16_t result = 0;	
	while(1)
	{
		int s = ScanKey();
		// read into 
		if(s != 0 && s != 7 && s != 8 && s != 9)
		{
			if ( s != 0)
			{
				Buzz(s); //already loop in the buzzer
				result = show_number(s);
			}
		}
		Display_7seg(result);
	}
}


int main(void)
{
		//intital function 
   SYS_Init();
   OpenSevenSegment(); // for 7-segment
	 OpenKeyPad();       // for keypad
   buzz_init();
	 GPIO_init();
 	 ex3_1();
	 //ex3_2();
	
}
