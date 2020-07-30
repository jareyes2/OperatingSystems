/*	Project 4 Julie Reyes
 *	stat function implementation
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
/* read in pathname given */
	if(argc !=2) {
		printf("Error not enough arguments in the command line\n");
		return 0;
	}

/* perform stat on file or directory */
	struct stat st;
	if(stat(argv[1], &st) == -1){
		printf("Error in stat() function\n");
		return 0;
	}
 
	/* file size */
	printf("File size: %lld bytes\n", (long long)st.st_size);

	/* number of blocks */
	printf("Blocks allocated: %lld\n",(long long)st.st_blocks);	

	/* reference link count */
	printf("Links: %lld\n", (long long)st.st_nlink);

	/* file permissions */
	printf("File permissions: ");
	printf((S_ISDIR(st.st_mode)) ? "d" : "-");
    	printf((st.st_mode & S_IRUSR) ? "r" : "-");
    	printf((st.st_mode & S_IWUSR) ? "w" : "-");
    	printf((st.st_mode & S_IXUSR) ? "x" : "-");
    	printf((st.st_mode & S_IRGRP) ? "r" : "-");
    	printf((st.st_mode & S_IWGRP) ? "w" : "-");
    	printf((st.st_mode & S_IXGRP) ? "x" : "-");
    	printf((st.st_mode & S_IROTH) ? "r" : "-");
    	printf((st.st_mode & S_IWOTH) ? "w" : "-");
    	printf((st.st_mode & S_IXOTH) ? "x" : "-");
	printf("\n");

	/* file inode */
	printf("Inode: %-4lld\n",(long long) st.st_ino);
	/* done with print */

	return 0;
}

 
