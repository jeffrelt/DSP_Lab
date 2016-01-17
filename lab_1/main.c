#include "dsk6713_aic23.h"

#define PART 3

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011


// Don' forget to add following files to your project. Add only one of them!
//	Vectors_intr.asm -> for interrupt
//	Vectors_poll.asm -> for polling

// You should implement your interrupt service routine (ISR) in here if your program uses interrupt.
/*
interrupt void c_int11()
{
	// your interrupt service routing
}
*/

void part1()
{
	comm_poll(); // this program does not use interrupt, so we added Vectors_poll.asm to the project.
	// this is a basic test program that toggles the leds on the board.
	unsigned int i = 0;

	while(1) {
		output_left_sample(i);
		i+=100;
		if( i >= 65535 )
			i = 0;
	}
}

void part2()
{
	comm_poll(); // this program does not use interrupt, so we added Vectors_poll.asm to the project.
	short sinewave[101] = {0,1028,2053,3069,4074,5062,6030,6975,7892,8778,9629,10442,11214,11942,12623,13254,13832,14356,14823,15232,15581,15868,16092,16253,16350,16383,16350,16253,16092,15868,15581,15232,14823,14356,13832,13254,12623,11942,11214,10442,9629,8778,7892,6975,6030,5062,4074,3069,2053,1028,0,-1028,-2053,-3069,-4074,-5062,-6030,-6975,-7892,-8778,-9629,-10442,-11214,-11942,-12623,-13254,-13832,-14356,-14823,-15232,-15581,-15868,-16092,-16253,-16350,-16383,-16350,-16253,-16092,-15868,-15581,-15232,-14823,-14356,-13832,-13254,-12623,-11942,-11214,-10442,-9629,-8778,-7892,-6975,-6030,-5062,-4074,-3069,-2053,-1028};
	unsigned int i = 0;
	// this is a basic test program that toggles the leds on the board.
	while(1) {
		output_left_sample(sinewave[i]);
		i+=1;
		if( i >= 100 )
			i = 0;
	}
}

void part3()
{
	comm_poll(); // this program does not use interrupt, so we added Vectors_poll.asm to the project.
	// this is a basic test program that toggles the leds on the board.
	unsigned int i = 65535;

	while(1) {
		unsigned int bit15 = i&(1<<15);
		unsigned int bit14 = i&(1<<14);
		unsigned int bit12 = i&(1<<12);
		unsigned int bit3  = i&(1<<3);
		unsigned int x = bit15^(bit14<<1)^(bit12<<3)^(bit3<<12);

		if(x == 0)
			output_left_sample(-10000);
		else
			output_left_sample(10000);

		i = (i>>1)|x;
	}
}

void main()
{
#if PART == 1
	part1();
#else
#if PART == 2
	part2();
#endif
	part3();
#endif

}


