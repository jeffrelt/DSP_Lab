#include "dsk6713_aic23.h"

// switch to select which part of the lab to run
#define PART 3

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011

// Don' forget to add following file to the project!
//	Vectors_poll.asm -> for polling


/*
	Generate a ramp/sawtooth wave using the polling method to generate output. Set the output
	frequency of the AIC23 codec to 8 kHz. Document parameters like the frequency of the
	generated output and peak to peak value, and include these values in your report. Check these
	values on an oscilloscope or using the Graph Tool in CCS. [[20]]
*/
void part1()
{
	comm_poll(); // this program uses polling
	
	unsigned int i = 0; // unsigned int so we can check for overflow and reset to 0

	while(1) {
		output_left_sample(i);
		i+=100;
		if( i >= 65535 )
			i = 0; // neatly cycle back to 0
	}
}


/*
	Generate a sine wave using a lookup table. The lookup table can be generated in MATLAB (using
	the sin function scaling the output to remove the floating point values), and then included as a
	header file. Include the MATLAB file(s) you use to generate this table in your report. Set the
	output frequency of the AIC23 codec to 16 kHz. Keep in mind that a full cycle of a sine wave has
	to be stored in the table to generate the correct output sequence You must choose your output
	frequency so that there is no under sampling. Check the waveform on an oscilloscope or using
	the Graph Tool in CCS. [[30]]
*/
void part2()
{
	comm_poll(); // this program uses polling
	
	// one complete sinewave. Generated with the following python code:
	// print('{'+''.join('{},'.format(int(sin(x*pi/50)*(2**14-1))) for x in range(100))+'};' )
	short sinewave[100] = {0,1028,2053,3069,4074,5062,6030,6975,7892,8778,9629,10442,11214,11942,12623,
	13254,13832,14356,14823,15232,15581,15868,16092,16253,16350,16383,16350,16253,16092,15868,15581,15232,
	14823,14356,13832,13254,12623,11942,11214,10442,9629,8778,7892,6975,6030,5062,4074,3069,2053,1028,0,
	-1028,-2053,-3069,-4074,-5062,-6030,-6975,-7892,-8778,-9629,-10442,-11214,-11942,-12623,-13254,-13832,-14356,-14823,-15232,
	-15581,-15868,-16092,-16253,-16350,-16383,-16350,-16253,-16092,-15868,-15581,-15232,-14823,-14356,-13832,-13254,
	-12623,-11942,-11214,-10442,-9629,-8778,-7892,-6975,-6030,-5062,-4074,-3069,-2053,-1028};
	
	unsigned short i = 0;

	while(1) {
		output_left_sample(sinewave[i]);
		i++;
		if( i >= 100 )
			i = 0; // loop back to 0 when i gets to 100 for a nice smooth sine wave
	}
}


/*
	Generate a pseudo random sequence using maximum length sequences, and capture the
	waveform using an oscilloscope or the Graph Tool in CCS. A maximum length sequence uses a
	circular shift register with feedback to generate a repetitive bit sequence with certain
	properties. The feedback values are given by the coefficients of a polynomial that is a prime
	factor of (Xn +1) under modulo two addition and multiplication. You can directly use one of the
	polynomials in [1]. It is advisable to use a polynomial of length greater than 10 so that the
	repetition time for the sequence is sufficiently high. Implementations in [4] can be directly
	translated to code with the values changed for a different polynomial. [[50]]
*/
void part3()
{
	comm_poll(); // this program uses polling
	
	unsigned short i = 65535; // seed the random generator

	while(1) {
		// grab the bits with masks
		unsigned short bit15 = i&(1<<15);
		unsigned short bit14 = i&(1<<14);
		unsigned short bit12 = i&(1<<12);
		unsigned short bit3  = i&(1<<3);
		// shift all over to the 15th bit and xor them all into x
		unsigned short x = bit15^(bit14<<1)^(bit12<<3)^(bit3<<12);

		if(x == 0)
			output_left_sample(-10000);
		else
			output_left_sample(10000);
		
		//shift the prior value right with x going to the 15th bit
		i = (i>>1)|x;
	}
}

void main()
{
// select which part to run
#if PART == 1
	part1();
#else
	#if PART == 2
		part2();
	#endif
	part3();
#endif

}
