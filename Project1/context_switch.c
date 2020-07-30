#define _GNU_SOURCE
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include<sched.h>
 
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
 
#include<time.h>
#include<sys/time.h>
#include<stdlib.h>
 
#include<string.h>
 
uint64_t rdtsc()
{
	uint32_t lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ( ((uint64_t)hi) << (uint32_t)32 )
	     | ( ((uint64_t)lo) );
}


int main(){
	//Set CPU to 0
	cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(0, &mask);
        sched_setaffinity(0, sizeof(cpu_set_t), &mask); 
 
	//variables
        uint64_t s, f;
        char str[] = "Transfered string";
        char read_str[100];
	int it = 10000; 
        int fd[2];
        int min = 0;
        pid_t pid;
 	long long tot = 0;
        
	//check if pipe was successfull
	if(pipe(fd) < 0){
                return 1;
        }
        
	//create parent and child process
        pid = fork();
         
	//check if fork successfull
	if(pid < 0){
                return 1;
        }
        
	//produce for multiple context switches for measuring 
	for(int i = 0; i < it; i++){
		s = rdtsc(); //start timer

		 if(pid == 0){  // In child process 
			close(fd[1]); 
			read(fd[0], read_str, 100);
			
			f = rdtsc(); // end timer
		
                        //save minimum ticks recorded for context switch
  			if(i == 0) min = f-s;
			else if (min > (f-s)) min = f-s; 

			//add time to a total time (excluding initial time due to copy-write)
			if(i != 0) tot += f-s; 
		 }
		 else{	 
			write(fd[1], str, strlen(str)+1);
			close(fd[1]);
			wait(NULL);
		  }
		
	}
 	
        if(pid == 0) { //once for loop completes print out results 
		double ave = (double)tot/(double)(it-1); //avarage amount of ticks in context switch

		printf("Minimum time: %f ns\n",((min/3392092000.0))*100000000); //convert rdtsc to time for min
		printf("Total Average time: %f ns\n",((ave/3392092000.0))*100000000); //convert rdtsc to time for average
		
	}
  return 0;
}
