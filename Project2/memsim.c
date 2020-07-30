//
//  memsim.c
//  Project2
//
//  Created by Salma Eid on 3/4/19.
//  Copyright © 2019 Salma Eid. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <fstream>
#include "lru_policy.h"
#include "fifo_policy.h"
int main(int argc, const char * argv[]) {
    // insert code here...
    //printf("Hello, World!\n");
    if(argc < 4){
        printf("ERROR: INPUT <USAGE> <tracefile> <nframes> <lru|fifo|vms> <debug|quiet> \n");
        return 1;
    }
    
    const char *file;
    file = argv[1]; //filename
    int nframes = atoi(argv[2]); //nframes
    const char *policy = argv[3];
    const char *dq = argv[4];
    
    if(strcmp(policy,"lru") == 0){
        lru(file, nframes, dq);
        //perform lru function
    }
    else if(strcmp(policy,"fifo") == 0){
        fifo(file, nframes, dq);
    }
    else if(strcmp(policy,"vms") == 0){
        vms(file, nframes, dq);
    }
    else
    {
        printf("Invalid policy entered\n");
        return 1;
    }
    return 0;
}
