//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
//
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"           // Device header

// define sound
#define P125ms 125000
#define P250ms 250000
#define P500ms 500000
#define P1S 	1000000

// function initiate

void Display_7seg(uint16_t value);
void Display_7seg_digit(uint16_t value);
void GPIO_init();
void buzz_init();
void Buzz(int number);
void gpio_display(int s);
uint16_t show_number(int s);
void ex3_1(void);
void ex3_2(void);
void playmusic(int tone[], int song[], int pitch[], int length);
void playlilbee(int s);
void playpolicehorn(int s);
void alldown(void);
void pauseall(void);
bool checkControl(void);


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
	// calculate decimal to binary
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

bool checkControl(void)
{
	int s = ScanKey();
	if (s == 9)
	{
		alldown();
		return true;
	}
	if (s == 8)
	{
		pauseall();
		while(1) 
		{
			CLK_SysTickDelay(10000);
			if(s == 9)
			{
				alldown();
				return true;
			}
			if (s == 8)
			{
				CLK_SysTickDelay(300000);
				return false;
			}
		}
	}
	return false;
}


void playmusic(int tone[], int song[], int pitch[], int length)
{
		int i , j , count = 0;
	
		for(i = 0; i < length; i++)
		{
        if (checkControl()) return;
			
				count=pitch[i]/(2*tone[song[i]-1]);  //calcuelate the number of periods for the beat
        
				for(j=0; j<count; j++)
				{
						if ((j%20) == 0)
						{
							if (checkControl()) return;
						}
						PB11=0;
            CLK_SysTickDelay(tone[song[i]-1]);
            PB11=1;
            CLK_SysTickDelay(tone[song[i]-1]);
        }
        CLK_SysTickDelay(1000); // short pause between notes
    }
}

void playlilbee(int s)
{
		int tone[7]={956, 851, 758, 716, 637, 568, 506};
		int song[13]={5, 3, 3, 4, 2, 2, 1, 2, 3, 4, 5, 5, 5};
		int pitch[13]={P250ms, P250ms, P500ms, P250ms, P250ms, P500ms,
               P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P500ms};
		
		int length = sizeof(song) / sizeof(song[0]);
    // volatile uint32_t *lightpin[4] = {&PC12, &PC13, &PC14, &PC15};
		
		// song played 
		playmusic(tone, song, pitch, length);
}

void playpolicehorn(int s)
{
	int tone[] = {625,500};
	int song[] = {1,2,1,2,1,2,1,2};
	int pitch[] ={P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms};
	
	int length = sizeof(song) / sizeof(song[0]);
	// left to right gpio display 
	// volatile uint32_t *lightpin[4] = {&PC12, &PC13, &PC14, &PC15};
	// song play 
	playmusic(tone, song, pitch, length);
}


void playambulance(int s)
{
	int tone[] = {833, 625};
	int song[] = {1, 2, 1, 2, 1, 2, 1, 2};
	int pitch[] = {P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms};
	
	// display with the sound 
	//volatile uint32_t *lightpin[4] = {&PC12, &PC15};
	
	//length of song 
	int length = sizeof(song) / sizeof(song[0]);
	// song play 
	playmusic(tone, song, pitch, length);
	//gpio_display(lightpin);
}

void pauseall(void)
{
	PB11 = 1;
}

void alldown(void)
{
	PB11 = 1; //buzz all down 
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1; // gpio return 0;
}

void ex3_3(void)
{
	while (1)
	{
		int s = ScanKey();
		if (s != 0 && s == 4 || s == 5 || s == 6)
		{
				if ( s == 4) playambulance(s);
				else if (s == 5) playpolicehorn(s);
				else if (s == 6) playlilbee(s);
				else if (s == 8) pauseall();
				else if (s == 9) alldown();
				else continue;
		}
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
 	 //ex3_1();
	 //ex3_2();
		ex3_3();
}
