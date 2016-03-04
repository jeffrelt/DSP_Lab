//
//  dsk6713_aic23.c
//  lab4_simulation
//
//  Created by Jeffrey Thompson on 3/3/16.
//  Copyright © 2016 Jeffrey Thompson. All rights reserved.
//

#include "dsk6713_aic23.h"
#include <stdio.h>


FILE* current_file = NULL;


int load(const char* file_name){
    if(current_file)
        fclose(current_file);
    current_file = fopen(file_name,"r");
    return current_file != NULL;
}

short input_left_sample(){
    int data;
    while(!fscanf(current_file,"%d",&data))
        rewind(current_file);
    return data;
}

void comm_poll()
{}

void output_left_sample(short sample)
{}