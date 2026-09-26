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
// excercies 
void ex3_1(void);
void ex3_2(void);
void ex3_3(void);
//function in ex3_3
bool playmusic_and_displaygpio(int tone[], int song[], int pitch[], int length, volatile uint32_t *ligthpin[],int lightlen);
void playlilbee(int s);
void playpolicehorn(int s);
void alldown(void);
void pauseall(void);
bool checkControl(void);
//gpio movement
void gpio_movement(volatile uint32_t *pin ,int length);



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
		//wait for the next 8 to resume 
		while (ScanKey() == 8)
		{
			CLK_SysTickDelay(10000);
		}
		
		while(1) 
		{
			CLK_SysTickDelay(10000);
			s = ScanKey();
			if(s == 9)
			{
				alldown();
				return true;
			}
			if (s == 8)
			{
				while (ScanKey() == 8) CLK_SysTickDelay(10000);
				break;
			}
		}
	}
	return false;
}


bool playmusic_and_displaygpio(int tone[], int song[], int pitch[], int length, volatile uint32_t *lightpin[], int lightlen)
{
		int i , j , count = 0;
	
		for(i = 0; i < length; i++)
		{
        if (checkControl()) return true;
				if (lightlen > 0) *lightpin[i  % lightlen] = 0;
				
				count=pitch[i]/(2*tone[song[i]-1]);  //calcuelate the number of periods for the beat
        
				for(j=0; j<count; j++)
				{
						if ((j%20) == 0)
						{
							if (checkControl())
							{
								if (lightlen > 0) *lightpin[i  % lightlen] = 1;
								return true;
							}
						}
						PB11=0;
            CLK_SysTickDelay(tone[song[i]-1]);
            PB11=1;
            CLK_SysTickDelay(tone[song[i]-1]);
        }
				
				if (lightlen > 0) *lightpin[i  % lightlen] = 1;
        CLK_SysTickDelay(1000); // short pause between notes
    }
		return false;
}

void playlilbee(int s)
{
		// another song just change this one is good 
		int tone[7]={956, 851, 758, 716, 637, 568, 506};
		int song[13]={5, 3, 3, 4, 2, 2, 1, 2, 3, 4, 5, 5, 5};
		int pitch[13]={P250ms, P250ms, P500ms, P250ms, P250ms, P500ms,
               P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P500ms};
		
		//length of the song 
		int length = sizeof(song) / sizeof(song[0]);
		
		// ligth and the lightlen things 
    volatile uint32_t *lightpin[4] = {&PC15, &PC14, &PC13, &PC12};
		int lightlen = sizeof(lightpin) / sizeof(lightpin[0]);
		// song played 
		int i = 0;
		while (1)
		{
			if(playmusic_and_displaygpio(tone, song, pitch, length, lightpin, lightlen))
				return;
		}
}

void playpolicehorn(int s)
{
	int tone[] = {625,500};
	int song[] = {1,2,1,2,1,2,1,2};
	int pitch[] ={P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms};
	
	int length = sizeof(song) / sizeof(song[0]);
	
	volatile uint32_t *lightpin[4] = {&PC12, &PC13, &PC14, &PC15};
	int lightlen = sizeof(lightpin) / sizeof(lightpin[0]);
	
	// song play 
	while (1)
	{
		if(playmusic_and_displaygpio(tone, song, pitch, length, lightpin, lightlen))
			return;
	}
}

void playambulance(int s)
{
	int tone[] = {833, 625};
	int song[] = {1, 2, 1, 2, 1, 2, 1, 2};
	int pitch[] = {P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms};
	
	// light gpio 
	volatile uint32_t *lightpin[2] = {&PC12, &PC15};
	int lightlen = sizeof(lightpin) / sizeof(lightpin[0]);
	
	//length of song 
	int length = sizeof(song) / sizeof(song[0]);
	// song play 
	while (1)
	{
		if(playmusic_and_displaygpio(tone, song, pitch, length, lightpin , lightlen))
			return;
	}
}

void pauseall(void)
{ 
	PB11 = 1;
	CLK_SysTickDelay(50000);
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

		if ( s == 4) playambulance(s);
		else if (s == 5) playpolicehorn(s);
		else if (s == 6) playlilbee(s);
		else if (s == 8) pauseall();
		else if (s == 9) alldown();
	}
}

int main(void)
{
	// we can do as press 1 to play the frist/ 2 to second / 3 to play the thrid 
		//intital function 
   SYS_Init();
   OpenSevenSegment(); // for 7-segment
	 OpenKeyPad();       // for keypad
   buzz_init();
	 GPIO_init();
	
	int times = 0;
	while(1)
	{
		times = ScanKey();
		if (times == 1) ex3_1();
		else if (times == 2) ex3_2();
		else if (times == 3 )ex3_3();
		else continue;
	} 
}