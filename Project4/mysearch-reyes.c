#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

void recPrint(const char*);

int main(int argc, char *argv[]) {
	/* variabl */
	char dir_name[PATH_MAX];
    	
	/* call recursive  */
	if(argc == 2)
		recPrint(argv[1]);
    	else
		getcwd(dir_name, sizeof(dir_name));

	recPrint(dir_name);
    return 0;
}


void recPrint(const char *filename){
    	/* variables */
	DIR *dir;
    	struct dirent *dir_struct;
	
	/* base case */
	if (!(dir = opendir(filename)))
        	return;

	/* recursive step */
    	while ((dir_struct = readdir(dir)) != NULL) {
        	if (dir_struct->d_type == DT_DIR) {
            		char temp[1024];
            		if (strcmp(dir_struct->d_name, ".") == 0 || strcmp(dir_struct->d_name, "..") == 0) continue;
            		strcpy(temp, filename);
			strcat(temp, "/");
			strcat(temp, dir_struct->d_name);
			printf("%s  ", dir_struct->d_name);
            		recPrint(temp);
        	} 
		else {
			if (strcmp(dir_struct->d_name, ".") == 0 || strcmp(dir_struct->d_name, "..") == 0) continue;
            		printf("%s\n", dir_struct->d_name); 
    		}
	}
    
	closedir(dir);
}

