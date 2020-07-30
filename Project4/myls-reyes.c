/*	Project 4 ls 
 *	Julie Reyes
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>


void print_ls(char*,int);

int main(int argc, char *argv[]){
	/* variables */
	char dir_name[1024];
	int print_long = 0; //0 = only print filename 1 = long print

	/* read in directory */
	if(argc == 1){
		/* if no directory get working directory */
		getcwd(dir_name, sizeof(dir_name));
		print_ls(dir_name, print_long);
	}
	
	/* read if -l in command line */
	else if(argc == 2){
		if(strcmp(argv[1],"-l") == 0){
			print_long = 1;	
			getcwd(dir_name, sizeof(dir_name));
            		print_ls(dir_name, print_long);
		}
            	else{
               		print_ls(argv[1], print_long);
            	}
        }
	/* read all commands */
        else if(argc == 3){
            if(strcmp(argv[1], "-l") == 0){
                print_long = 1;
                print_ls(argv[2], print_long);
            }
            else{
                printf("error\n");
            }
        }			
	
	return 0;				
}

/* print ls */
void print_ls(char *dir_name, int print_long){
        /* directory variables */
        DIR* dir;
        struct dirent* dir_struct;
        struct passwd* pwd;
        struct group* grp;
        struct stat st;
	char _file[1024];	

        /* check if directory opened */
        if((dir = opendir(dir_name))== NULL){
            printf("Error opening directory:, %s\n", dir_name);
            return;
        }

        /* open directory and print*/
        while((dir_struct = readdir(dir)) != NULL){
         	if (strcmp(dir_struct->d_name, ".") == 0 || strcmp(dir_struct->d_name, "..") == 0)
			continue;
	
		/* use stat() function on path name*/
		strcpy(_file, dir_name);
            	strcat(_file, "/");
            	strcat(_file, dir_struct->d_name);;
                stat(_file, &st);
                /* if -l argument entered */
                if(print_long == 1){
                        /*print file permissions*/
                        printf( (S_ISDIR(st.st_mode)) ? "d" : "-");
                        printf((st.st_mode & S_IRUSR) ? "r" : "-");
                        printf((st.st_mode & S_IWUSR) ? "w" : "-");
                        printf((st.st_mode & S_IXUSR) ? "x" : "-");
                        printf((st.st_mode & S_IRGRP) ? "r" : "-");
                        printf((st.st_mode & S_IWGRP) ? "w" : "-");
                        printf((st.st_mode & S_IXGRP) ? "x" : "-");
                        printf((st.st_mode & S_IROTH) ? "r" : "-");
                        printf((st.st_mode & S_IWOTH) ? "w" : "-");
                        printf((st.st_mode & S_IXOTH) ? "x" : "-");
			printf("\t");
                        /* Number of links */
                        printf("%lld\t", (long long)st.st_nlink);

                        /* Print out owner */
                        if((pwd = getpwuid(st.st_uid)) != NULL)
                                printf("%s\t", pwd->pw_name);
                        else
                                printf("%d\t", st.st_uid);

                        /* Print out group name */
                        if ((grp = getgrgid(st.st_gid)) != NULL)
                                printf("%s\t", grp->gr_name);
                        else
                                printf("%d\t", st.st_gid);

                        /* file size */
                        printf("%lld bytes\t", (long long)st.st_size);

                        printf("%lld\t", (long long)st.st_ino);

                        /* print file name */
                        printf("%s\n",dir_struct->d_name);
                }
                else{
                        /* print file name */
			printf("%s\t",dir_struct->d_name);
            	}
	}
	if(print_long == 0)
		printf("\n");
        
        closedir(dir);
	return;
}
   



