/* Project 4 Julie Reyes
 * Tail implementation
 *
 * seeks to near the end of the file, reads in a block of data, and then goes
 * backwards until it finds the requested number of lines;
 * print out those lines from beginning to the end of the file
 * mytail -n file, where n is the number of lines at the end of the file to print;
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char *argv[]){
	/* read in file or get file name */
	if(argc < 3){
		printf("Error: Usage: <./> <-n>number of lines  <file name> file name");
  		return 0;
	}
	

	/* parse command */
	int num_length = (int) strlen(argv[1]); //get length of number in command line 
	char str_len[num_length];		//make array of the lenght of number entered
	strncpy(str_len, argv[1]+1,num_length);	//copy number into array 
	int num = atoi(str_len);		//extract number
	
	/* perform stat on file */
	struct stat st;
	if(stat(argv[2], &st) == -1){
		printf("Error in stat() function\n");
		return 0;
	}
	
	/* file size */
	//printf("File size: %-4lld bytes\n", (long long)st.st_size);
	
	/* open file */
	int fd;
	fd = open(argv[2], O_RDONLY);
	if(fd < 0){
		printf("Error opening file\n");
	}
	
	/* lseek */
	int count_lines = 0;
        char c[512];	//buffer 
	lseek(fd, -512, SEEK_END); //go to one block from the end
	int offset = 0;	
	int i;
	int j;


	while(count_lines < num+1){
		read(fd, &c, 512);
		for(j = 512-1; j >= 0; j--){
			offset++;
                	if(c[j] == '\n'){
                		count_lines++;
              		}
			if(count_lines == num+1)
				break;
        	}

		if(count_lines < num+1){
			lseek(fd,-(2*512),SEEK_CUR);
		}	
	}
	
	/* output */
	char x;
	lseek(fd,-(offset-1),SEEK_END);
	for(i = 0; i < offset-1; i++){
		read(fd,&x,1);
		printf("%c",x);
	} 
	
	close(fd);

return 0;
}
