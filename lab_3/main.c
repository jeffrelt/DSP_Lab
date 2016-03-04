#include "dsk6713_aic23.h"

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011

// single switch to enable which part of the lab to run
#define PART4

#ifdef PART1
	#include "firlow.h"
#endif

#ifdef PART2
	#include "firhigh.h"
#endif

#ifdef PART3
	#include "firband.h"
#endif

#ifdef PART4
	#define IIR
	#include "iirlow.h"
#endif

#ifdef PART5
	#define IIR
	#include "iirhigh.h"
#endif

#ifdef PART6
	#define IIR
	#include "iirband.h"
#endif

Uint32 fs = DSK6713_AIC23_FREQ_16KHZ; // 2

#define N1 N-1

void main()
{
	comm_poll(); // this program does not use interrupt, so we added only Vectors_poll.asm to the project.

	unsigned short count = 0;

	short x[N]; //history of inputs
#ifdef IIR
	short y[N]; //history of outputs
#endif
	while(1) {
		unsigned short base;
		if(count < N){
			base = count;
			// do not output anything until our buffer is full
		}
		else{
			base = count - N;
			float result = 0; // accumulate our result here
			unsigned short i;
			for(i = 0; i < N; ++i ){
                    // filter is symetrical so direction applied does not matter
                    // di starts from the oldest sample and goes to the most recent
				unsigned short di = (i+base)%N;
                    // convolution
				result += x[di]*b[i];
			
            }
#ifndef IIR     // for FIR we only convolve with the input samples, so we are done
			short sample = (short)result;
#else           // for IIR we also - the convolution with the a filter
                // we have to be carefull to line up the right elements here
			for(i = 0; i < N1; ++i ){
                    // we start with the most recent, which is the last output Y[-1]
				unsigned short di = (N1-i+base)%N;
                    // notice we shift the filter (a) up by 1 to convolve the right elements
				result -= y[di]*a[i+1];
			}
			short sample = (short)result;
			y[base] = sample;
#endif
			output_left_sample(sample); //ouput our filtered result
		}
        // overwrite the oldest sample to setup the next loop
		x[base] = input_left_sample();

            // our count starts from 0 and increments to 2N then loops back to N (over and over)
		if(++count >= N*2)
			count -= N;
	}

}

