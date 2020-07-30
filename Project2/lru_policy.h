//
//  lru_policy.h
//  Project2
//
//  Created by Salma Eid on 3/3/19.
//  Copyright © 2019 Salma Eid. All rights reserved.
//

#ifndef lru_policy_h
#define lru_policy_h
#include <stdio.h>

//Structure of the lru list
struct lru_frame{
    int long num;
    int dirty;
    
    struct lru_frame* next;
    struct lru_frame* back;
};

//lru function is the main function, call all other functions
void lru(char* file, long int no_frames, char* dq);
//return 1 if frame in memory, 0 if not
int lru_in_memory(int long frame_num);
//brings a frame already in memory to head of the list, make it mru
void lru_bring_to_front(char rw);
//removes the lru frame
void lru_remove_tail();
//add NEW frame to the head of the list, making it by default mru
void lru_add_to_front(int long frame_num, char rw);
//print private variables if STATE = debug
void lru_print_list();

#endif /* lru_policy_h */
