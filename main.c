//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
//
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
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
void GPIO_init(void);
void buzz_init(void);
void Buzz(int number);
void gpio_display(int s);
uint16_t show_number(int s);

// excercies 3
void ex3_1(void);
void ex3_2(void);
void ex3_3(void);

// excercise 4
void ex4_1(void);
void ex4_2(void);
void ex4_3(void);

//function in ex4_1
void sevseg_display_modify(uint16_t value);
void backthehellup(void);
void cleargpio(void);
void plus_num(void); 

// function in ex4_2
void Init_GPIO_RGB(void);
void display_light_blue(void);
void display_light_green(void);
void display_light_red(void);
bool is_prime(int number);
void all_off(void);
void led_light_determination(int num);

// function ex4_3
void chasing_light(void);
void Init_EXTINT(void);
void rev_chasing_light(void);
void rgb_key(int key);

// function for ex5_1
void ex5_1(void);
void ex5_2(void);

// function for ex3_3
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
	ShowSevenSegment(3,value); 
	CLK_SysTickDelay(5000);
}

void GPIO_init(void)
{
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
}

void buzz_init(void)
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
	int last_s;
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
		int i = 0;
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

//================================================
// 4 start here 
// function 

void sevseg_display_modify(uint16_t value)
{
		uint8_t i, digit;
	if (value == 0)                 // BLANK
	{
		CloseSevenSegment();
		CLK_SysTickDelay(5000);
		return;
	}

		for (i = 0; i < 4 && value > 0; i++)
		{
			CloseSevenSegment();
			digit = value % 10;
			ShowSevenSegment(i,digit);
			value /= 10; //move another digit
			CLK_SysTickDelay(5000);
		}
}


#define BLANK 0

//main work
void ex4_1(void)
{	
	/*
	1 2 3
	4 5 6 
	B C +
	B : back the number as x|1|2|3  x|x|1|2 
	C : clear all back to beginning
	+ : sum up the number input 1 + 2 +  output 3
	and output must be 1234 + 1234 = 2468 (4 digit all be display after calcualation )
	*/
	int sum = 0;
	int num1 = 0;
	int numbers = 0; 
	int state = 0;
	int input = 0;
	int display_value = BLANK;
	int step = 0;
	
	while(1)
	{
		input = ScanKey();
		if (input != 0)
			state = input;
		else 
		{
			switch (state)
			{
				case 1: case 2: case 3:
				case 4: case 5: case 6: 
					if (step < 2)
					{
						numbers = numbers * 10 + state;
						numbers %= 1000;
						display_value = numbers;
					}
					break;
				case 7: // back
					if (step < 2) 	
					{	
						numbers /= 10;
						display_value = (numbers == 0) ? BLANK : numbers;			
					}
					break;
					
				case 8: //  clear
						numbers = 0;
						step = 0;	
						num1  = 0;
						display_value = BLANK;
						break;
				
				case 9:  // read this part agian
					if (step == 0 && numbers > 0)
					{
						num1 = numbers; // save value to num1 
						numbers = 0; // set numbers back to no
						display_value = BLANK; 
						step = 1;
					} 
					else if (step == 1 && numbers > 0) 	
					{
							display_value = num1 + numbers;
							step = 2;
					} 
					
					break;
			}
			state = 0;
		}
		// display 
		sevseg_display_modify(display_value);
	}
}

void Init_GPIO_RGB(void)
{
	GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
	PA12 = 1; PA13 = 1;PA14 = 1; //ALL LIGHT OFF
}
void display_light_blue(void)
{
	PA12 = 0;
}

void display_light_green(void)
{
	PA13 = 0;
}

void display_light_red(void)
{
	PA14 = 0;
}

void all_off(void)
{
	PA12 = 1; PA13 = 1;PA14 = 1; //all off
}

bool is_prime(int number)
{
	if (number <= 1)
				return false;

		// Check divisibility from 2 to n-1
		for (int i = 2; i < number; i++)
				if (number % i == 0)
						return false;

		return true;
}

void led_light_determination(int num)
{
		bool two_dv = (num % 2 == 0);
		bool three_dv = (num % 3 == 0);
		bool is_pr = (is_prime(num));
	
		if (two_dv) display_light_blue();
		if (three_dv) display_light_green();
		if (is_pr)
		{
			if (two_dv) display_light_blue();
			else if	(three_dv) display_light_green();
			display_light_red();
			Buzz(1);
		}
}

void ex4_2(void)
{
	int s;
	int number = 0;
	//int counter = 0;
	bool is_held = false;
	while(1)
	{
		s = ScanKey();
		if (s == 9)
		{
			is_held = true;
			// counter++;
			CloseSevenSegment();
			CLK_SysTickDelay(5000);
			all_off();
		}
		else if(is_held && s == 0) 
		{
			//if button on hold nothing display
			//display after hold 
			is_held = false;
			
			//srand(counter);
			number = rand() % 100;
			led_light_determination(number);
		}
		else if(s == 8)
		{
			number = 0;
			all_off();
		}
		// if not held so display 
		if(!is_held)sevseg_display_modify(number);
	}
}

void Init_EXTINT(void)
{
	// Configure EINT0 pin and enable interrupt by falling edge trigger
	//  GPIO_SetMode(PB, BIT14, GPIO_MODE_INPUT);
	//  GPIO_EnableEINT0(PB, 14, GPIO_INT_FALLING);
	//  NVIC_EnableIRQ(EINT0_IRQn);

	// Configure EINT1 pin and enable interrupt by rising and falling edge trigger
	GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
	GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
	NVIC_EnableIRQ(EINT1_IRQn);

	// Enable interrupt de-bounce function and select de-bounce sampling cycle time
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);

	//  GPIO_ENABLE_DEBOUNCE(PB, BIT14);
	GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

volatile int g_dir = 0;

void EINT1_IRQHandler(void)
{
	GPIO_CLR_INT_FLAG(PB , BIT15);
	g_dir = 1; 
}


void SetMode_IQRINT1(void)
{
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
}

void chasing_light(void)
{
	static int pos = 0;
	int i, key, last = 0;
	volatile uint32_t *arr[4] = {&PC12 , &PC13, &PC14 , &PC15};

	*arr[pos] = 0;
	for (i = 0; i < 100; i++)        
	{
		key = ScanKey();
		if (key >= 1 && key <= 3) last = key;     
		else if (key == 0 && last != 0)           
		{
			rgb_key(last);                        
			last = 0;
		}
		CLK_SysTickDelay(5000);
	}
	*arr[pos] = 1;

	if (g_dir == 0) pos = (pos + 1) % 4;
	else            pos = (pos + 3) % 4;
}

	
	// no use 
void lgb_display(int s)
{
	while(s != 0)
	{
			if (s == 1) display_light_red();
			else if (s == 2) display_light_green();
			else if (s == 3) display_light_blue();
			else continue;
	}
}

void rgb_key(int key)
{
	static int now = 0;              // màu dang sáng
	if (now == key) now = 0;         // b?m cùng phím l?n 2 thì t?t
	else
	{
		if (key == 1) display_light_red();
		else if (key == 2) display_light_green();
		else if (key == 3) display_light_blue();
		now = key;
	}
}

void ex4_3(void)
{
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
	while (1)
	{
		chasing_light();             // phím dã du?c quét bên trong
	}
}

int main(void)
{
	int times = 0;
		// we can do as press 1 to play the frist/ 2 to second / 3 to play the thrid 
		//intital function 
	SYS_Init();
	OpenSevenSegment(); // for 7-segment
	OpenKeyPad();       // for keypad
	buzz_init();
	GPIO_init();
	Init_EXTINT();
	// SetMode_IQRINT1();
	// RGB led 
	//Init_KEY();
	Init_GPIO_RGB();	
	while(1)
	{
		times = ScanKey();

		if (times == 1) ex3_1();
		else if (times == 2) ex3_2();
		else if (times == 3 )ex3_3();
		if (times == 4)
		{
			while(ScanKey() != 0);
			ex4_1();
		}
		else if (times == 5) ex4_2();
		else if (times == 6) ex4_3();
		//else if (times == 7) ex5_1();
		//else if (times == 8) ex5_2();
		else continue;
	} 
}

