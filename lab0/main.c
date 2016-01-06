#include "dsk6713_aic23.h"
#include "dsk6713_led.h"

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011
#define DELAY 150
#define PART 6

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011

/* add any global variables here */
//unsigned int i = 0;


// Don' forget to add following files to your project. Add only one of them!
//	Vectors_intr.asm -> for interrupt
//	Vectors_poll.asm -> for polling

// You should implement your interrupt service routine (ISR) in here if your program uses interrupt.
/*
interrupt void c_int11()
{
	// your interrupt service routing
	output_left_sample(input_left_sample());
}
*/

void part_1(){

}
void part_2(){

}
void part_3(){

}
void part_4(){

}
void part_5(){
	Uint32 samples[DELAY];
	int i;

	while(1) {
		if(i == DELAY*2){
			i = DELAY;
		}
		int mod = i%DELAY;


		if(i >= DELAY){
			output_sample(samples[mod]);
		}
		samples[mod] = input_sample();
		++i;

	}
}
void part_6(){
	Uint32 samples[DELAY];
	unsigned long long i;

	while(1) {
		int mod = i%DELAY;
		Uint32 sample = 0;
		if(i >= DELAY){
			sample = samples[mod];
		}
		samples[mod] = input_left_sample();

		if(sample){
			output_right_sample(sample);
			sample = (0.5*sample+ 0.5*samples[mod]);
			output_left_sample(sample);
		}
		++i;

	}
}

void main()
{
	comm_poll(); // this program does not use interrupt, so we added Vectors_poll.asm to the project.
	//comm_intr();
	DSK6713_LED_init();

	switch(PART){
		case 1: part_1(); break;
		case 2: part_2(); break;
		case 3: part_3(); break;
		case 4: part_4(); break;
		case 5: part_5(); break;
		case 6: part_6(); break;
		default: break;
	}
}


//
//void main()
//{
//	comm_poll(); // this program does not use interrupt, so we added Vectors_poll.asm to the project.
//	//comm_intr();
//	DSK6713_LED_init();
//
//	Uint32 samples[150];
//	unsigned int i;
//	int loaded = 0;
//
//	// this is a basic test program that toggles the leds on the board.
//	while(1) {
//
//		DSK6713_LED_toggle(0);
//		DSK6713_LED_toggle(2);
//
//		samples[i] = input_sample();
//		i= (i+1)%150;
//		if (loaded != 1){
//			if( i == 148){
//				loaded = 1;
//			}
//		}
//		else{
//
//			output_sample(samples[(i+149)%150]);
//
//		}
//	}
//}


