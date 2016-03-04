#include "dsk6713_aic23.h"

#define DSK6713_AIC23_INPUT_MIC 0x0015
#define DSK6713_AIC23_INPUT_LINEIN 0x0011

#include <stdio.h>
#include <stdlib.h>

Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; // 1
Uint16 inputsource = DSK6713_AIC23_INPUT_LINEIN; // 0x011

#define SIMULATION

// 512 sample moving average

#define N 400

#define WORD_COUNT 5
#define SAMPLE_RATE 8000.0f

#define MAG_THRESHOLD 2000
#define TIME_THRESHOLD 50

void isolate(short* word_durations){

	short s_count = 0;
	short w_count = 0;
	long time = 0;
	short hist[N];
	long accumulator = 0; // moving average * N
	short state = 0;
	long start;

	printf("Sampling!\n");
	fflush(stdout);
	while(1) {
		unsigned short base;
		if(s_count < N){
			q_index = s_count;
			// do not remove samples from accumulator until the queue (hist) is full
		}
		else{
			q_index = s_count - N;
			// remove the oldest sample
			accumulator -= hist[q_index];
		}
		// get the new sample, rectify it
		short sample = input_left_sample();
		if( sample < 0 )
			sample = -sample;
		//save to accumulator and the hist
		accumulator += sample;
		hist[q_index] = sample;

		// state machine
		switch(state)
		{
		case 0:{
			if( accumulator < MAG_THRESHOLD )
				break;
			// our average value is greater than the threshold, set starting time, change state
			state = 1;
			start = time;
		}
		default:{
			if( accumulator >= MAG_THRESHOLD )
				break;
			// we are no longer exceding the threshold, change state and find duration
			state = 0;
			short duration = time-start;
			// check if this was long enough to be considered a word
			if( duration > TIME_THRESHOLD ){
				// if so save it
				int ms = duration*1000/SAMPLE_RATE
				printf("Word duration: %i\n", ms)
				fflush(stdout);
				word_durations[w_count] = ms;
				++w_count;
				if( w_count == WORD_COUNT ){
					printf("Isolated %d words\n", w_count);
					return;
				}

			}
		}
		}


		++time;
		if(++s_count >= N*2){
			s_count -= N;
		}

	}
}



void main()
{
	comm_poll();

		//printf("******* SPEAKER RECOGNITION *******\n");
		//fflush(stdout);
		//return 0;
		while(1) {


			//output_left_sample(input_left_sample());

			printf("******* SPEAKER RECOGNITION *******\n");
			printf("1 - Training\n");
			printf("2 - Testing\n");
			printf("Please enter your choice:\n");

			int choice = 0;

			scanf("%d",&choice);

			short word_durations[WORD_COUNT];
			isolate(word_durations);

			if(choice == 1) {

				int user = 0;
				printf("For which user do you wanna train (1-3): \n");
				scanf("%d",&user);

				int ready = 0;
				do {
					printf("Please provide the training sound, enter 1 when it is ready \n");
					scanf("%d",&ready);
				} while(ready != 1);

				printf("Training sound is sampling...\n");

				// Sample Voice
				// Isolate
				// Train

			} else if(choice == 2) {

				int ready = 0;
				do {
					printf("Please provide the test sound, enter 1 when it is ready \n");
					scanf("%d",&ready);
				} while(ready != 1);

				printf("Test sound is sampling... \n");

				// Sample Voice
				// Isolate
				// Test

			}
		}

}


