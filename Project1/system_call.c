#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

//how to use rdtsc
//reference: https://www.linuxquestions.org/questions/programming-9/how-to-measure-cpu-time-spent-on-a-code-block-with-c-or-c-894790/
uint64_t rdtsc(void);

uint64_t rdtsc()
{
	uint32_t lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ( ((uint64_t)hi) << (uint32_t)32 )
	     | ( ((uint64_t)lo) );
}


int main (){  
  //variables 
  uint64_t s, f; 
  uint64_t total_ticks = 0;
  int it = 10000; //iteration
  for(int i = 0; i < it; i++){
     s = rdtsc(); //start counter
        getuid(); //syscall
     f = rdtsc(); //end counter   
     uint64_t temp = f - s; //total cycles for syscall
     total_ticks += temp; //sum of cycles for syscall
  } 
  
  double ave = total_ticks/ it; //average time 
  printf("Total nanoseconds: %f\n",((ave/3392092000.0))*100000000);  

  return 0;
}

