//
//  lru_policy.c
//  Project2
//
//  Created by Salma Eid on 3/3/19.
//  Copyright © 2019 Salma Eid. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lru_policy.h"

//number of nodes in the list, does not exceed number of frames
long int lru_count;
//head points at the most recently used frame
struct lru_frame* mru_head;
//tail points at the least recently used frame
struct lru_frame* lru_tail;
//temp pointer:
struct lru_frame* temp;
//STATE: 1 = print, 0 = quiet
int lru_STATE;
//Number of times page was found in memory
long int lru_HITS;

//A miss and R or W, increment
long int total_disk_reads;
//If page being replaced is a W, increment
long int total_disk_writes;
//The amount of addresses that is dealt with
long int events_in_trace;


//Fumction: checks if frame in memory: return 1
//If not, return 0
int lru_in_memory(int long frame_num){
    
    if(mru_head == NULL)
        return 0;
    else{
        
        temp = mru_head;
        while(temp->num != frame_num ){
            temp = temp->next;
            if(temp == NULL)
                return 0;
        }
        return 1;
    }
}

//Function: brings a frame already in the list to the front
void lru_bring_to_front(char rw){
    
    //If not head, remove
    if(temp != mru_head){
        
        //If last node update tail
        if(temp == lru_tail)
            lru_tail = lru_tail->back;
        
        //If last or middle node
        temp->back->next = temp->next;
        
        //If NOT last node
        if(temp->next != NULL)
            temp->next->back = temp->back;
        
        
        //Null  and re initialize:
        temp ->next = NULL;
        temp ->back = NULL;
        //add to front, unpdate head no increase in count
        temp->next = mru_head;
        mru_head->back = temp;
        mru_head = temp;
    }
    
    //Re intializing even the head
    if( rw == 'W')
        temp->dirty = 1;
    else if((rw == 'R')&& (temp->dirty != 1))
        temp->dirty = 0;
}

//Function: removes least recently used frame = tail
void lru_remove_tail(){
    
    struct lru_frame* remove = lru_tail;
    
    //If removed frame = dirty, increment disk writes
    if(remove->dirty == 1){
        
        total_disk_writes++;
    }
    
    //If only node, null head and tail pointers
    if(lru_count == 1){
        
        lru_tail = NULL;
        mru_head = NULL;
        
    }
    else{
        //If not only node, update tail
        lru_tail = lru_tail->back;
        lru_tail->next = NULL;
    }
    
    //Remove frame
    free (remove);
    //decrement frame count
    lru_count--;
}

void lru_add_to_front(int long frame_num, char rw){
    
    //create a new frame
    struct lru_frame* new_frame = (struct lru_frame*)malloc(sizeof(struct lru_frame));
    
    //Intialize everything:
    new_frame->num = frame_num;
    //If W set dirty bit to 1
    //If R set dirty bit to 0
    if( rw == 'W')
        new_frame->dirty = 1;
    else if(rw == 'R')
        new_frame->dirty = 0;
    
    new_frame->next = NULL;
    new_frame->back = NULL;
    
    //add to front
    //if NOT empty:
    //or count != 0
    if(mru_head != NULL){
        
        //update head pointer
        new_frame->next = mru_head;
        mru_head->back = new_frame;
        mru_head = new_frame;
        
    }
    
    else{
        //If list empty, new node is the head and the tail of the list
        mru_head = new_frame;
        lru_tail = new_frame;
    }
    //Increment frame count
    lru_count++;
}

//Print the whole list from tail to head: lru to mru for debugging
void lru_print_list(){
    
    struct lru_frame* print = lru_tail;
    
    printf("Count: %ld\n",lru_count);
    while(print != NULL){
        
        printf("Frame no: %x Dirty bit: %d\n",print->num, print->dirty);
        print = print->back;
        
    }
    printf("Events in Trace: %ld\n", events_in_trace);
    printf("Total disk reads: %ld\n", total_disk_reads);
    printf("Total disk writes %ld\n\n", total_disk_writes);
}

//Function: main function calls everything else
void lru(char* file, long int no_frames, char* dq){
    
    //Intialization global variables:
    mru_head = NULL;
    lru_tail = NULL;
    lru_count = 0;
    total_disk_reads = 0;
    total_disk_writes = 0;
    events_in_trace = 0;
    lru_HITS =0;
    
    //Set the STATE
    if(strcmp(dq,"debug") == 0){
        lru_STATE = 1;
    }
    else if(strcmp(dq,"quiet") == 0){
        lru_STATE = 0;
    }
    
    unsigned addr;
    char rw;
    unsigned frame_num;
    FILE *fp;
    
    //read file
    fp = fopen(file, "r");
    if(fp == NULL){
        printf("Error opening file\n");
    }
    //if(fp){
        //!feof(fp) && !ferror(fp)
    
        while((fscanf(fp, "%x %c", &addr, &rw) != EOF)){

            
            //if (fscanf(fp, "%x %c", &addr, &rw) == EOF)
                //break;
    
            //Get frame number by shiftting addr to the right by 12
            //Get R or W
            if (lru_STATE == 1) {
                printf("\nTrace address: %x\t<R/W>: %c\n", addr, rw);
                printf("Virtual Page Number: %d\n", addr>>12);
            }
            
            frame_num = addr >> 12;
            //Increment events
            events_in_trace++;
            
            int in_memory = lru_in_memory(frame_num);
            //If frame in memory:
            if (in_memory == 1) {
                lru_HITS++;
                //make that frame mru
                lru_bring_to_front(rw);
                if(lru_STATE == 1){
                    printf("In memory\n");
                    lru_print_list();
                }
                
                
            }
            else if( in_memory == 0){ //If not in memory
                
                //A page fault
                total_disk_reads++;
                
                //If memory not full:
                if( lru_count < no_frames){
                    
                    //add as a mru
                    lru_add_to_front(frame_num, rw);
                    if(lru_STATE == 1){
                        printf("Not in memory, memory is NOT full\n");
                        lru_print_list();
                    }
                }
                
                else{
                    
                    //If memory is full:
                    //Remove lru
                    lru_remove_tail();
                    if(lru_STATE == 1){
                        printf("Not in memory, memory IS full\n");
                        lru_print_list();
                    }
                    //Add new frame as mru
                    lru_add_to_front(frame_num, rw);
                    if(lru_STATE == 1)
                        lru_print_list();
                    //Incrementing variable
                }
    
            }
            
        }
    
    //}
    //Print variables if quiet
    if(lru_STATE == 0){
        printf("Total memory frames: %ld\n", no_frames);
        printf("Events in Trace: %ld\n", events_in_trace);
        printf("Total disk reads: %ld\n", total_disk_reads);
        printf("Total disk writes %ld\n", total_disk_writes);
        //printf("Total Hits %ld\n", lru_HITS);
    }
    fclose(fp);
}




