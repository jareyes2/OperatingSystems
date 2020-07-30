#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <semaphore.h>

#include "cube.h"
#include "wizard.h"

#define DEFAULT_CUBE_SIZE 4
#define DEFAULT_TEAM_SIZE 5
#define DEFAULT_SEED 1
#define HOWBUSY 100000000
#define TRUE 1
#define FALSE 0

pthread_mutex_t lock;

//DONE
void command_line_usage()
{
  fprintf(stderr, "-size <size of cube> -teamA <size of team> -teamB <size of team> -seed <seed value>\n"); 
}

//DONE
void kill_wizards(struct wizard *w)
{
  /* Fill in */
  //terminates threads of wizard passed to function
  pthread_cancel(w->thread_id);  
  return;
}

//DONE
int check_winner(struct cube* cube)
{
    /* Fill in */
    //check all wizards of a team to see if they are frozen
    int i;
    int count = 0;
    //check to see if team A won
    for (i = 0; i < cube->teamA_size; i++)
    {
        if (cube->teamA_wizards[i]->status == 1)
        {
	  count++;
	  continue;
        }
	else break;
    }
    if(count == cube->teamA_size){
        printf("Team B wins the game!\n");
 	return 1; 
    }
    
    //check to see if team B won
    count = 0;
    for (i = 0; i < cube->teamB_size; i++)
    {
        if (cube->teamB_wizards[i]->status == 1)
        {
          count++;
          continue;
        }
        else break;
    }
    if(count == cube->teamB_size){
       printf("Team A wins the game!\n");
       return 2; 
    }
	
    //if no winner return 0
    return 0;
}

//DONE
void print_cube(struct cube *cube)
{
    int i;
    int j;
    int k;
    assert(cube);
    for (i = 0; i < cube->size; i++) 
    {
      	printf("+");
	for (j = 0; j < cube->size; j++) 
	{
	  printf("--+");
	}     	
	printf("\n|");
      	for (j = 0; j < cube->size; j++)
	{
	  /* Print the status of wizards in this room here */
	  	for (k = 0; k < 2; k++)
		{
	      		if (cube->rooms[j][i]->wizards[k] != NULL)
			{
		  		if (cube->rooms[j][i]->wizards[k]->status)
				{
		      			printf("%c", tolower(cube->rooms[j][i]->wizards[k]->team));
		    		}
		  		else
				{
		      			printf("%c", toupper(cube->rooms[j][i]->wizards[k]->team));
		    		}
			}
	      		else {
		  		printf(" ");
			}
	    	} 	
		printf("|");
	} 
	printf("\n");    
    }

    printf("+");
    for (j = 0; j < cube->size; j++)
    {
      printf("--+");
    }
    printf("\n");
    return;
}

//DONE
struct wizard *init_wizard(struct cube* cube, char team, int id)
{
  int x, newx;
  int y, newy;
  int initflag;
  struct wizard *w;

  w = (struct wizard *)malloc(sizeof(struct wizard));
  assert(w);

  initflag = FALSE;
  
  w->team = team;
  w->id = id; 
  w->status = 0;
  w->cube = cube;
  
  x = rand() % cube->size;
  y = rand() % cube->size;
  
    if (cube->rooms[x][y]->wizards[0] == NULL)
    {
      cube->rooms[x][y]->wizards[0] = w;
      w->x = x;
      w->y = y;
    }
    else if (cube->rooms[x][y]->wizards[1] == NULL)
    {
      cube->rooms[x][y]->wizards[1] = w;
      w->x = x;
      w->y = y;
    }
    else
    {
      newx = (x + 1) % cube->size;
      if (newx == 0) newy = (y + 1) % cube->size;
      else newy = y;

        while((newx != x || newy != y) && !initflag)
	{

	    if (cube->rooms[newx][newy]->wizards[0] == NULL)
	    {
	      cube->rooms[newx][newy]->wizards[0] = w;
	      w->x = newx;
	      w->y = newy;
	      initflag = TRUE;
	    }
	    else if (cube->rooms[newx][newy]->wizards[1] == NULL)
	    {
	      cube->rooms[newx][newy]->wizards[1] = w;
	      w->x = newx;
	      w->y = newy;
	      initflag = TRUE;
	    }
	    else
	    {
	      newx = (newx + 1) % cube->size;
	      
 	        if (newx == 0)
		{
		  newy = (newy + 1) % cube->size;
		}
	    }
	  
	}
      if (!initflag){
	free(w);
	return NULL;
      }
    }

  /* Fill in */
  //Wizard semiphore initialized to 0 for each wizard
  sem_init(&w->w_semi, 0, 0);
  //room semiphore
  sem_wait(&cube->rooms[w->x][w->y]->room_lock);

  


  //int u;
  //sem_getvalue(&cube->rooms[w->x][w->y]->room_lock,&u);
  //printf("ROOM semaphore after wziard placed %d\n",u);
  



  return w;
}

//DONE
int interface(void *cube_ref)
{
  struct cube* cube;
  char *line;
  char *command;
  int i;

  cube = (struct cube *)cube_ref;
  assert(cube);

  using_history();
  while (1)
  {
//pthread_mutex_lock(&lock);
      	line = readline("cube> ");
      	if (line == NULL) continue;
      	if (strlen(line) == 0) continue;
      	add_history(line);

      	i = 0;	
      	while (isspace(line[i])) i++;
      
      	command = &line[i];
      	if (!strcmp(command, "exit"))
	{
	  return 0;
      	}
      	else if (!strcmp(command, "show"))
      	{
	  print_cube(cube);
      	}
      	else if (!strcmp(command, "start"))
	{
	    if (cube->game_status == 1)
	    {
	      fprintf(stderr, "Game is over. Cannot be started again\n");
	    }
	    else if (cube->game_status == 0)
	    {
	      fprintf(stderr, "Game is in progress. Cannot be started again\n");
	    }
	    else
	    {
	      cube->game_status = 0;
	      
	      /* Start the game */
//---->
		//* WIZARD THREAD CREATION
		int z;
		pthread_t wizardsA_thread[cube->teamA_size];
		pthread_t wizardsB_thread[cube->teamB_size];	
  		for(z = 0; z < cube->teamA_size; z++)
		{
			pthread_create(&wizardsA_thread[z], NULL ,wizard_func, cube->teamA_wizards[z]);     	
       	  		cube->teamA_wizards[z]->thread_id =  wizardsA_thread[z];
		}
		for(z = 0; z < cube->teamB_size; z++)
		{
                	pthread_create(&wizardsB_thread[z], NULL ,wizard_func, cube->teamB_wizards[z]);       
                	cube->teamB_wizards[z]->thread_id = wizardsB_thread[z];
		}
		//END WIZARD THREADS*/
//---->	
	    }
	}
	else if (!strcmp(command, "s"))
	{
		if(cube->game_status != 0) {
			printf("Game has not started or has already ended, enter 's', 'c', or 'exit'\n");
			continue;
		}
		
		int cw = check_winner(cube); //check winner each time to end game if there is a winner
		if(cw == 0) //if there is no winner start a wizard thread 
		{	
			//print_cube(cube);
			sem_post(&global); //post for wizard threads to be able to run              
			dostuff();
			sem_wait(&alternate);//wait for wizard thread to post to interface
                        dostuff();
		}
		else{ //if the game has ended kill wizard threads
                        int q;
			//kill wizard threads for team A
			for(q = 0; q < cube->teamA_size; q++)
                	{
                        	kill_wizards(cube->teamA_wizards[q]);
                	}
			//kill wizard threads for team B
                	for(q = 0; q < cube->teamB_size; q++)
                	{
                        	kill_wizards(cube->teamB_wizards[q]);
                	} 
			cube->game_status = 1;		
		}
	}
	else if (!strcmp(command, "c"))
        {
        	if(cube->game_status != 0) { 	
			printf("Game has not started  or has already ended, enter 's', 'c', or 'exit'\n");
                        continue;
                }
               
                while(!check_winner(cube)){
			sem_post(&global);
			dostuff();
			//print_cube(cube);
			while(sem_wait(&alternate));         
          	}
                
          	//kill wizards
          	int w;
          	for(w = 0; w < cube->teamA_size; w++)
                {       
                	kill_wizards(cube->teamA_wizards[w]);
                }
                for(w = 0; w < cube->teamB_size; w++)
                {
                	kill_wizards(cube->teamB_wizards[w]);
                }
          	cube->game_status = 1;
	}
        
        else if (!strcmp(command, "stop"))
	{
	  /* Stop the game */
	  return 1;
	}
        else
	{
	  fprintf(stderr, "unknown command %s\n", command);
	}
      free(line);
    }

  return 0;
}


//DONE
int main(int argc, char** argv)
{
  int cube_size = DEFAULT_CUBE_SIZE;
  int teamA_size = DEFAULT_TEAM_SIZE;
  int teamB_size = DEFAULT_TEAM_SIZE;
  unsigned seed = DEFAULT_SEED;
  struct cube *cube;
  struct room *room;
  struct room **room_col;
  int res;
  struct wizard *wizard_descr;
  int i, j;
  

  /* Parse command line and fill:
     teamA_size, timeBsize, cube_size, and seed */

    i = 1;
    while(i < argc) 
    {
        if (!strcmp(argv[i], "-size")) 
        {
	    i++;
	    if (argv[i] == NULL) 
            {
              fprintf(stderr, "Missing cube size\n");
              command_line_usage();
              exit(-1);
            }
            cube_size = atoi(argv[i]);
	    if (cube_size == 0)
	    {
              fprintf(stderr, "Illegal cube size\n");
              exit(-1);
	    }
        }
        else if (!strcmp(argv[i], "-teamA")) 
        {
            i++;
            if (argv[i] == NULL) 
            {
              fprintf(stderr, "Missing team size\n");
              command_line_usage();
              exit(-1);
            }
            teamA_size = atoi(argv[i]);
	    if (teamA_size == 0)
	    {
              fprintf(stderr, "Illegal team size\n");
              exit(-1);
	    }
        }
        else if (!strcmp(argv[i], "-teamB")) 
        {
            i++;
            if (argv[i] == NULL) 
            {
              fprintf(stderr, "Missing team size\n");
              command_line_usage();
              exit(-1);
            }
            teamB_size = atoi(argv[i]);
	    if (teamB_size == 0)
	    {
              fprintf(stderr, "Illegal team size\n");
              exit(-1);
	    }
        }
        else if (!strcmp(argv[i], "-seed")) 
        {
            i++;
            if (argv[i] == NULL) 
            {
              fprintf(stderr, "Missing seed value\n");
              command_line_usage();
              exit(-1);
            }
            seed = atoi(argv[i]);
	    if (seed == 0)
	    {
              fprintf(stderr, "Illegal seed value\n");
              exit(-1);
	    }
        }
        else 
        {
          fprintf(stderr, "Unknown command line parameter %s\n", argv[i]);
          command_line_usage();
          exit(-1);
        }
        i++;
    }

  /* Sets the random seed */
  srand(seed);

  /* Checks that the number of wizards does not violate
     the "max occupancy" constraint */
    if ((teamA_size + teamB_size) > ((cube_size * cube_size) * 2))
    {
      fprintf(stderr, "Sorry but there are too many wizards!\n");
      exit(1);
    }

  /* Creates the cube */
  cube = (struct cube *)malloc(sizeof(struct cube));
  assert(cube);
  cube->size = cube_size;
  cube->game_status = -1;

  /* Creates the rooms */
  cube->rooms = malloc(sizeof(struct room **) * cube_size);
  assert(cube->rooms);

    for (i = 0; i < cube_size; i++)
    {
      /* Creates a room column */
      room_col = malloc(sizeof(struct room *) * cube_size);
      assert(room_col);

        for (j = 0; j < cube_size; j++)
	{
	  /* Creates a room */
	  room = (struct room *)malloc(sizeof(struct room));
	  assert(room);
	  room->x = i;
	  room->y = j;
	  room->wizards[0] = NULL;
	  room->wizards[1] = NULL;
	  room_col[j] = room;

	  /* Fill in */
//---->
	  //ROOM SEMAPHORE initialized to 2		
          sem_init(&room->room_lock, 0, 2);
	  //INTERFACE/WIZARD SEMAPHORE initialized to 0
	  sem_init(&alternate, 0, 0);
//---->
	}
      
      cube->rooms[i] = room_col;
    }

  /* Creates the wizards and positions them in the cube */
  cube->teamA_size = teamA_size;
  cube->teamA_wizards = (struct wizard **)malloc(sizeof(struct wizard *) * teamA_size);
  assert(cube->teamA_wizards);

  cube->teamB_size = teamB_size;
  cube->teamB_wizards = (struct wizard **)malloc(sizeof(struct wizard *) * teamB_size);

  assert(cube->teamB_wizards);

  /* Team A */
    for (i = 0; i < teamA_size; i++)
    {
        if ((wizard_descr = init_wizard(cube, 'A', i)) == NULL)
	{
	  fprintf(stderr, "Wizard initialization failed (Team A number %d)\n", i);
	  exit(1);
	}
        cube->teamA_wizards[i] = wizard_descr;
    }

  /* Team B */

    for (i = 0; i < teamB_size; i++)
    {
        if ((wizard_descr = init_wizard(cube, 'B', i)) == NULL)
	{
	  fprintf(stderr, "Wizard initialization failed (Team B number %d)\n", i);
	  exit(1);
	}
        cube->teamB_wizards[i] = wizard_descr;
    }

  /* Fill in */
  //global semaphore
  sem_init(&global,0,0);
  
  /* Goes in the interface loop */
  res = interface(cube);

  exit(res);
}

//DONE
void dostuff()
{
  int i;
  int wait;
  
  wait = rand() % HOWBUSY;

  for (i = 0; i < wait; i++) {}

  return;
}

//DONE
struct room * choose_room(struct wizard* w)
{
  int newx = 0; 
  int newy = 0;

  /* The two values cannot be both 0 - no move - or 1 - diagonal move */ 
  while (newx == newy)
  {
      newx = rand() % 2;
      newy = rand() % 2;
  }
  if ((rand() % 2) == 1) 
  {
    newx = 0 - newx;
    newy = 0 - newy;
  }
  return w->cube->rooms[(w->x + w->cube->size + newx) % w->cube->size][(w->y + w->cube->size + newy) % w->cube->size];
}


//DONE
int try_room(struct wizard *w, struct room *oldroom, struct room* newroom)
{
	/*FILL IN*/
	//check if rooms are empty
	if(newroom->wizards[0] != NULL && newroom->wizards[1] != NULL ){
		//rooms are full
		printf("Request denied, room locked!\n");
		return 1; //one if room cant be filled
	}
   	return 0; //should return 0 if room can be filled
}







//DONE
struct wizard *find_opponent(struct wizard* self, struct room *room)
{
  struct wizard *other = NULL;
  /* Updates room wizards and determines opponent */
  if ((room->wizards[0] == self)){
      other = room->wizards[1];
  }
  else if (room->wizards[1] == self){
      other = room->wizards[0];
  } 
  return other;
}









//DONE 
void switch_rooms(struct wizard *w, struct room *oldroom, struct room* newroom)
{
    struct wizard *other;

    /* Removes self from old room */
    if (oldroom->wizards[0] == w){
        oldroom->wizards[0] = NULL;
    }
    else if (oldroom->wizards[1] == w){
        oldroom->wizards[1] = NULL;
    }
    else /* This should never happen */{
        printf("Wizard %c%d in room (%d,%d) can't find self!\n", w->team, w->id, oldroom->x, oldroom->y);
        print_cube(w->cube);
        exit(1);
    }

  /* Fill in */
//---->
   sem_post(&oldroom->room_lock);//after wizard is removed from old room, wizard gives up room semaphore
   dostuff();
//---->  

    /* Updates room wizards and determines opponent */
    if (newroom->wizards[0] == NULL)
    {
      newroom->wizards[0] = w;
      other = newroom->wizards[1];
    }
    else if (newroom->wizards[1] == NULL)
    {
      newroom->wizards[1] = w;
      other = newroom->wizards[0];
    }
    else /* This should never happen */
    {
      printf("Wizard %c%d in room (%d,%d) gets in a room already filled with people!\n",
	     w->team, w->id, newroom->x, newroom->y);
      print_cube(w->cube);
      exit(1);
    }
  
    /* Sets self's location to current room */
    w->x = newroom->x;
    w->y = newroom->y;
}







//
int fight_wizard(struct wizard *self, struct wizard *other, struct room *room)
{
    int res;
    /* Computes the result of the fight */
    res = rand() % 2;
		  
    /* The opponent becomes frozen */
    if (res == 0)
    {
      	printf("Wizard %c%d in room (%d,%d) freezes enemy %c%d\n",self->team, self->id, room->x, room->y, other->team, other->id);

      	/* Fill in */
	other->status = 1; //put that wizard thread to sleep
        //int u;
       // sem_getvalue(&other->w_semi,&u);
       // printf("Enemy semiphore semaphore = %d\n",u);
       // sem_post(&alternate);
    }

    /* Self freezes and release the lock */
    else 
    {
      	printf("Wizard %c%d in room (%d,%d) gets frozen by enemy %c%d\n", self->team, self->id, room->x, room->y, other->team, other->id);
      	/* Fill in */

	self->status = 1;
      	return 1;

    }
  return 0;
}



//
int free_wizard(struct wizard *self, struct wizard *other, struct room* room){
    int res;
    /* Computes the results of the unfreeze spell */
    res = rand() % 2;
    /* The friend is unfrozen */
    if (res == 0)
    {
      	printf("Wizard %c%d in room (%d,%d) unfreezes friend %c%d\n", self->team, self->id, room->x, room->y, other->team, other->id);
     
      	/* Fill in */
	other->status = 0;  //change status to alive 
        return 1;
    }

    /* The spell failed */
    printf("Wizard %c%d in room (%d,%d) fails to unfreeze friend %c%d\n", self->team, self->id, room->x, room->y, other->team, other->id);
    return 0;

}



