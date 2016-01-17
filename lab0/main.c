#include "dsk6713_aic23.h"

// Switch to select which part to run
#define PART 6

// set the delay (in number of samples) for part 5 and 6.
#define DELAY 150

// Don't forget to add following files to your project. Add only the one used in the part selected.
//	Vectors_intr.asm -> for interrupt
//	Vectors_poll.asm -> for polling

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011

#if PART == 3
Uint32 fs = DSK6713_AIC23_FREQ_96KHZ; // 7
#else
Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
#endif

Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011



// You should implement your interrupt service routine (ISR) in here if your program uses interrupt.
#if PART == 2
interrupt void c_int11()
{
	// your interrupt service routing
	output_left_sample(input_left_sample());
}
#endif


void part_1(){
    /*
     Perform audio input and output using the polling method. In polling, the program should loop continuously and
     fetch input data and output data after amplification. Set the sampling frequency to 8 kHz. [[10]]
    */
    
    // this program uses polling so Vectors_poll.asm should be added to the project.
    comm_poll();
    
    while(1) {
        // sample the left input and output it to the left.
        output_left_sample( input_left_sample() );
    }
}
void part_2(){
    /*
     Perform audio input and output using the interrupt method. Use an interrupt service routine that will be
     executed every time an input is given to the DSK. Set the sampling frequency to 8 kHz. [[10]]
    */
    
    // this program uses interrupts so Vectors_intr.asm should be added to the project.
    comm_intr();
    
    // the action is in the ISR c_int11()
}
void part_3(){
    /*
     Do problem (2) by setting the sampling frequency to 96 KHz and see the quality improvement on the sound [[5]]
    */
    // Due to problems with problem 2 we based this problem on part 1
    
    // this program uses polling so Vectors_poll.asm should be added to the project.
    comm_poll();
    
    while(1) {
        // sample the left input and output it to the left.
        output_left_sample( input_left_sample() );
    }
}
void part_4(){
    /*
     Do problem (2) by setting the sampling frequency to 1 kHz and see the effect of aliasing on the sound [[5]]
    */
    // Due to problems with problem 2 we based this problem on part 1
    
    // this program uses polling so Vectors_poll.asm should be added to the project.
    comm_poll();
    
    unsigned int i = 0;
    short sample;
    
    while(1) {
        //
        if( i&8 == 0){
            // overwrite sample with the new signal
            sample = input_left_sample()
        }
        else{
            // call the function (for timing reasons) but do not save it
            input_left_sample()
        }
        // output the sample
        output_left_sample(sample);
        ++i
    }
}
void part_5(){
    /*
     Do problem (2) by storing the data in a buffer of length 150. [[5]]
    */
    // Due to problems with problem 2 we based this problem on part 1
    
    short samples[DELAY];
    short sample;
    unsigned int i;
    
    // this program uses polling so Vectors_poll.asm should be added to the project.
    comm_poll();
    
    while(1) {
        // modulo the counter (i) by DELAY to get the value within the samples array
        unsigned int mod = i%DELAY;
        
        // grab the signal DELAY samples ago before we replace it
        sample = samples[mod];
        samples[mod] = input_left_sample();
        
        // wait until our buffer (signals) is full before we output anything
        if(i >= DELAY){
            // output the 'live' signal in the right ear for reference
            output_right_sample(samples[mod]);
            // output the delayed signal in the left ear
            output_left_sample(sample);
        }
        // increment our count then set the counter to DELAY if needed
        // this is to avoid overflowing the counter and having gaps in the output
        if(++i == DELAY*2){
            i = DELAY;
        }
    }
}
void part_6(){
    /*
     Using the buffers of problems (5), generate a false echo by giving the output from the previous 
     buffer into the input of the current buffer. [[15]]
    */
    // Due to problems with problem 2 we based this problem on part 1
    
    short samples[DELAY];
    short sample;
    unsigned int i;
    
    // this program uses polling so Vectors_poll.asm should be added to the project.
    comm_poll();
    
	while(1) {
        // modulo the counter (i) by DELAY to get the value within the samples array
		unsigned int mod = i%DELAY;
        
        // grab the signal DELAY samples ago before we replace it
        sample = samples[mod];
        samples[mod] = input_left_sample();
        
        // wait until our buffer (signals) is full before we output anything
		if(i >= DELAY){
            // output the 'live' signal in the right ear for reference
            output_right_sample(samples[mod]);
            // mix the 'live' and delayed signal and output them to the left ear
            sample = (sample>>1 + samples[mod]>>1);
            output_left_sample(sample);
		}
        // increment our count then set the counter to DELAY if needed
        // this is to avoid overflowing the counter and having gaps in the output
        if(++i == DELAY*2){
            i = DELAY;
        }
	}
}

void main()
{
	switch(PART){
		case 1: part_1(); break;
		case 2: part_2(); break;
		case 3: part_3(); break;
		case 4: part_4(); break;
		case 5: part_5(); break;
		default:part_6(); break;
	}
}
