//
//  dsk6713_aic23.h
//  lab4_simulation
//
//  Created by Jeffrey Thompson on 3/3/16.
//  Copyright © 2016 Jeffrey Thompson. All rights reserved.
//

#ifndef dsk6713_aic23_h
#define dsk6713_aic23_h

#define DSK6713_AIC23_FREQ_8KHZ 1

typedef unsigned int Uint32;
typedef unsigned short Uint16;

void comm_poll();

int load(const char* file_name);

short input_left_sample();

void output_left_sample(short sample);


#endif /* dsk6713_aic23_h */
