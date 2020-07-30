#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include "fifo_policy.h"


//global variable for print debug 
int STATE; 

//initilize new page table
struct PageEntryFifo* newPage(unsigned pgNum, int dirt, char _rw) 
{ 
    struct PageEntryFifo *temp = (struct PageEntryFifo*)malloc(sizeof(struct PageEntryFifo));  //allocate memory 
    temp->pageNum = pgNum;  //Virtual page number
    temp->dirty = dirt;     //dirty bit
    temp->rw = _rw;         //Performed R or W
    temp->next = NULL;      //point to next entry in table
    return temp;  
} 

//initialize empty page table
struct PageTableFifo *createTable(nf) 
{ 
    struct PageTableFifo *tbl = (struct PageTableFifo*)malloc(sizeof(struct PageTableFifo)); //allocate memory for page table
    tbl->MAX_SIZE_NFRAMES = nf;
    tbl->head = NULL;           //table list head
    tbl->tail = NULL;           //table list tail
    tbl->size = 0;              //no elements in table when created
    tbl->disk_write = 0;
    tbl->PAGE_FAULT_COUNT = 0;
    tbl->table_hits = 0;
    tbl->disk_read = 0;
    return tbl;                
} 

// add page to Table 
struct PageEntryFifo *addPage(struct PageTableFifo *tbl, unsigned pgNum, int dirt, char rw) 
{ 
    struct PageEntryFifo *temp = newPage(pgNum, dirt, rw);      // temporary table entry
    struct PageEntryFifo *removedPg = NULL;                     // temporary table entry for returning removed page if needed
    
    // If page table is empty, then new page added
    if (tbl->size  == 0) 
    { 
        tbl->head = temp;               
        tbl->tail = temp; 
        tbl->PAGE_FAULT_COUNT++;                              //if page table is empty first page is a page fault
        tbl->size ++;                                          //increment table size 
        return NULL; 
    }
    
    //if table is not empty
    else {
        if(entryInTbl(tbl, temp) == 0)                          //check if page is in table, if not in table continue
        {  

            if(tbl->size == (tbl->MAX_SIZE_NFRAMES)){           //if the table is full must remove a page 
                removedPg = removeFirstPage(tbl);               //removes first page in in FIFO order and stores pointer to page in removed
                if(removedPg->dirty == 1){                      //if a dirty page is removed increase write count
                    tbl->disk_write++;
                }
            }
            if(tbl->tail != NULL)                               //if table is not empty them add to page table an maintain FIFO order
            {
                tbl->tail->next = temp; 
                tbl->tail = temp;    
                //if(STATE == 1) printf("Added Page: %x\n", temp->pageNum);
                tbl->size ++;
                //if(STATE == 1) printf("Page Table size: %x\n", tbl->size);
            }
            else{                                       
                tbl->head = temp; 
                tbl->tail = temp; 
                tbl->size ++;
                //if(STATE == 1) printf("Page Table size: %x\n", tbl->size);
            }
        }
    }
    return removedPg;                                           //page added with no removal of any pages
} 

//checks to see if page is in table
int entryInTbl(struct PageTableFifo *tbl, struct PageEntryFifo *lookup)
{
    struct PageEntryFifo *temp;                 //temporary entry for table
    struct PageTableFifo *list = tbl;           //temporary page table
    
    if(tbl->size != 0){                             //if table is not empty 
        temp = list->head;   
        int i;                       //start at last page and look through table for page number
        for(i = 0; i < tbl->size; i++){         //while not the fist page in placed in the table
            if(lookup->pageNum == temp->pageNum){   //if the page number is in table 
               //if(STATE == 1)  printf("FOUND PAGE\n");
                if(lookup->rw == 'W'){              //check to see if its a write
                    if(temp->rw == 'R')             //If writing to a read page chage dirty bits
                    { 
                        temp->dirty = 1;            //if the memory is a write change bit to dirty
                        if(STATE == 1)  printf("Updated Dirty Bit: %d\n", temp->dirty);
                    }
                }
                tbl->table_hits++;
                return 1;                           //if found return 1
            }
            temp = temp->next;                      //move to next position
        }
        tbl->disk_read++;                           //increase the disk reads becuase there was no hit on the table
        tbl->PAGE_FAULT_COUNT++;                    //increase the page fault count becuase not in page
    }
    
    return 0;                                       //returns 0 if page is not in table
}
 
//write to disk or remove from page 
struct PageEntryFifo *removeFirstPage(struct PageTableFifo *tbl) 
{ 
    if (tbl->size == 0) return NULL;                //If table is empty retrun NULL pointer to indicate nothing found
  
    // Store previous first in and move front one page ahead 
    struct PageEntryFifo *temp = tbl->head;         //temp will be return as pointer to the page entry removed
    tbl->head = tbl->head->next;                    //move the head to maintail FIFO order 
    
    if (tbl->head == NULL)  tbl->tail = NULL;       //If table is empty after removing node make the pointers point to NULL
    
    tbl->size --;                                   //decrement table size
    //if(STATE == 1)  printf("Removed page number %x\n", temp->pageNum);
    return temp;
} 


int lookup(struct PageTableFifo *tbl, struct PageEntryFifo *lookup)
{
    //variables
    struct PageEntryFifo *curr;                 
    struct PageEntryFifo *prev;
    
    //empty table dont look for page
    if(tbl->size == 0) {
        //printf("Not in loopup\n");
        return -1; //empty table
    }

    //pointers to traverse through table
    curr = tbl->head;
    prev = NULL; 
    
    //look through table until page is found
    while(curr->pageNum != lookup->pageNum){
        //if(STATE) == 1) printf("Current page num: , %x\n", curr->pageNum);
        prev = curr;
        curr = curr->next;
        if(curr == NULL){
            //printf("Not in lookup\n");
            return -1;                              //if page is not in table return -1
        }
    }
    return 1; // return 1 if entry is in table
}
int vms_search_remove(struct PageTableFifo *tbl,unsigned pgNum){
    
    //variables to traverse table
    struct PageEntryFifo *curr;                    
    struct PageEntryFifo *prev;

    //printf("In search table size = %d\n", tbl->size);
    if(tbl->size == 0) {
        //printf("Not in C/D\n");
        return -1;                          //empty table
    }
    
    curr = tbl->head;
    prev = NULL; 
    
    while(curr->pageNum != pgNum){
        prev = curr;
        curr = curr->next;
        if(curr == NULL){
            //printf("Not in C/D\n");
            return -1;                     //returns if entry not found 
        }
    }
    
    //conditions to remove specific node from page table
    if(tbl->size == 1){
        tbl->head = NULL;
        tbl->tail = NULL;
        free(curr);
    }
    else if(prev == NULL){
       tbl->head = curr ->next;

       free(curr); 
           
    }
    else{

        if(curr == tbl->tail){
            tbl->tail = prev;
        }
        prev->next = curr->next;
        free(curr);
    }
    
    tbl->size--;            //decrements size of table becuase enrty was removed
    //printf("Removed from C/D\nC/D table size now: %d\n",tbl->size);
    return 1;               //returns 1 if removed page successfully 
}

struct PageEntryFifo*  getTail(struct PageTableFifo *t){
    return t->tail;         //funtion to get tail
}
/*````FIFO FUNCTION``````````````````````````````````````````````````````````````````````````````````*/
void fifo(char* file, int nframes, char* dq)
{
    FILE *fp;               //file pointer
    unsigned addr;          //holds address
    char rw;                //checks if read or write is performed
    int dirtyBit;           //if page must write
    unsigned pageNum;       //VPN
    
    if(strcmp(dq,"debug") == 0){    //changes state to print function if needs to debug
        STATE = 1;                  //based on command line inputs
    }

    int eventsInTrace = 0;          //counts the number of address read in total
    struct PageTableFifo *pageTable = createTable(nframes); //creates table of nframes size value passed in command

    fp = fopen(file, "r");          //open and check if file can be opened
    if(fp == NULL){
        printf("Error opening file\n");
    }
    if(fp){                         
        while(!feof(fp) && !ferror(fp)){  
            if(fscanf(fp, "%x %c", &addr, &rw) == EOF) break;           //read in address by line
            if (STATE == 1) {                                           //print functions to debug
                printf("\nTrace address: %x\t<R/W>: %c\n", addr, rw);
                printf("Virtual Page Number: %x\n", addr>>12);
            }

            pageNum = addr >> 12;       //shift address to get virtial page numbers  
            if(rw == 'W') {             //store if page is a write 
                dirtyBit = 1;           //change dirty to 1
            } 
            else dirtyBit = 0;           //if read dirty bit is 0
            
            struct PageEntryFifo *tofree = addPage(pageTable, pageNum, dirtyBit, rw); //adds page to page table and frees page returned
            if(tofree != NULL) free(tofree);
            eventsInTrace++;           
            if((!feof(fp) && !ferror(fp)) == 0) break; //end of file end loop
         
        }
    }
    if(strcmp(dq,"quiet") == 0){    //prints quiet statement
        printQuietMode(nframes, eventsInTrace, pageTable->PAGE_FAULT_COUNT, pageTable->disk_write);
    }
    fclose(fp); 
}
/*````FIFO FUNCTION``````````````````````````````````````````````````````````````````````````````````*/

/*````VMS FUNCTION``````````````````````````````````````````````````````````````````````````````````*/
void vms(char* file, int nframes, char* dq)
{
    //varibles for extracting information from trace files 
    FILE *fp;
    unsigned addr;
    char rw;
    int dirtyBit;
    int process = 0;

    //varables for keeping count to print
    int diskw = 0;
    int diskr = 0;
    int pagehit = 0;
    int events = 0;

    //check debug/quiet option 
    if(strcmp(dq,"debug") == 0){
        STATE = 1;
    }
    
    //page table varibles
    struct PageTableFifo *pTableP1 = createTable((nframes/2));      //process 1 page table
    struct PageTableFifo *pTableP2 = createTable((nframes/2));      //process 2 page table
    struct PageTableFifo *clean = createTable((nframes/2)+1);       //clean buffer
    struct PageTableFifo *dirty = createTable((nframes/2)+1);       //dirty buffer
    struct PageTableFifo *memory = createTable(nframes);            //memory buffer

    //opening file check for error
    fp = fopen(file, "r");          //open file
    if(fp == NULL){                 //if file not opened gives error
        printf("Error opening file\n");
    }

    
    while(!feof(fp) && !ferror(fp)){    
            if(fscanf(fp, "%x %c", &addr, &rw) == EOF) break;               //scan by line address  
            
            if (STATE == 1) {
                printf("\n\nTrace address: %x\t<R/W>: %c\n", addr, rw);
                printf("Virtual Page Number: %x\n", addr>>12);
            }
            
            events++; //per line increase events 

            //If page is a write make it dirty
            if(rw == 'W') dirtyBit = 1;        
            else dirtyBit = 0; 

            //variables to store page entries
            struct PageEntryFifo *newpg = newPage(addr>>12,dirtyBit,rw);    //new page made after line scan
            struct PageEntryFifo *oldpg;                                    //temp variable to store old page

            if(addr>>28 == 3){  process = 1;}                               //checks to see if page starts with a 3 to be stored in process 1
            else{ process= 2;}                                              //if process not then 2
            
            if(process == 1){                                               //if process 1 check if in FIFO
                if (entryInTbl(pTableP1, newpg) == 1) {
                    pagehit++;
                    continue;
                }
                else{                                                       //Not In fifo for p1 add page to p1 fifo
                    oldpg = addPage(pTableP1,addr>>12, dirtyBit, rw);
                    if(oldpg == NULL){diskr++;}
                    if(oldpg != NULL)          //FIFO full and page kicked is out not NULL and add to clean or dirty if R or W
                    {
                        if(oldpg->dirty == 1){
                            struct PageEntryFifo *f = addPage(dirty,oldpg->pageNum,oldpg->dirty,oldpg->rw); 
                            if(f != NULL){free(f);}                            
                        }
                        else{
                            struct PageEntryFifo *f = addPage(clean,oldpg->pageNum,oldpg->dirty,oldpg->rw);
                            if(f != NULL){free(f);} 
                        }
                    }

                    if(lookup(memory, newpg) == 1){  //New page in memory and not in FIFO removing from clean or dirty buffers
                        if(vms_search_remove(clean, newpg->pageNum) == -1){ //search clean
                            vms_search_remove(dirty, newpg->pageNum);       //not in clean search dirty
                        }
                    }
                    else {                           //New Page not in memory remove from clean or dirty buffers and memory and add new to memory
                        diskr++;
                        if(memory->size < nframes){  //Memory not full Add page to memory
                            addPage(memory, newpg->pageNum, newpg->dirty, newpg->rw);
                        }
                        else{                       //Memory full
                            if(clean->size !=0 ){   //if clean not empty remove first from clean 
                                struct PageEntryFifo *f = removeFirstPage(clean);       
                                vms_search_remove(memory, f->pageNum); //remove page from clean also
                                free(f);
                                struct PageEntryFifo *t = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw); //adds new mage to memory
                                free(t);
                            }
                            else if(dirty->size != 0 ){ //if dirty not empty remove from clean 
                                diskw++;
                                struct PageEntryFifo *f = removeFirstPage(dirty);
                                vms_search_remove(memory, f->pageNum);
                                free(f);
                                struct PageEntryFifo *t = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw);
                                free(t);
                            }
                            else{ //if not in clean or dirty and memory is full remove from memory
                                struct PageEntryFifo *d = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw);
                                if(d->dirty == 1) diskw++;
                                free(d);
                            }
                        }
                    }
                }
            }

            if(process == 2){                           //process 2 not starting with address 3
                if (entryInTbl(pTableP2, newpg) == 1) {     //check if entry is in FIFO P2 if so continue reading file
                    pagehit++;
                    continue;
                }
                else{                                       //Not In fifo add page to p2 fifo
                    oldpg = addPage(pTableP2,addr>>12, dirtyBit, rw);
                    if(oldpg == NULL){diskr++;}
                    if(oldpg != NULL)                       //FIFO full and page kicked out not NULL
                    {
                        if(oldpg->dirty == 1){              //Old page dirty
                            struct PageEntryFifo *f = addPage(dirty,oldpg->pageNum,oldpg->dirty,oldpg->rw);
                            if(f != NULL){free(f);} 
                        }
                        else{                               //Old page was clean
                            struct PageEntryFifo *f = addPage(clean,oldpg->pageNum,oldpg->dirty,oldpg->rw); 
                            if(f != NULL){free(f);}
                        }
                    }
                    if(lookup(memory, newpg) == 1){                             //New page in memory and not in FIFO removing from clean or dirty
                        if(vms_search_remove(clean, newpg->pageNum) == -1){     //search clean
                            vms_search_remove(dirty, newpg->pageNum);           //if not in clean search dirty
                        }
                    }
                    else {
                        diskr++;
                        if(memory->size < nframes){
                            //Memory not full Add page to memory
                            addPage(memory, newpg->pageNum, newpg->dirty, newpg->rw);
                        }
                        else{   //Memory full
                            if(clean->size != 0 ){      //if clean not empty remove from clean
                                struct PageEntryFifo *f = removeFirstPage(clean);   //removes first page
                                vms_search_remove(memory, f->pageNum);              //removes page removed from clean from memory
                                free(f);                                            //frees removed page
                                struct PageEntryFifo *t = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw); //add new page to memory 
                                free(t);
                            }
                            else if(dirty->size != 0 ){
                                diskw++;
                                struct PageEntryFifo *f = removeFirstPage(dirty);   //removes page from dirty 
                                vms_search_remove(memory, f->pageNum);              //removes page removed from dirty from memory
                                free(f);            
                                struct PageEntryFifo *t = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw);
                                free(t);
                            }
                            else{   
                                struct PageEntryFifo *t = addPage(memory,newpg->pageNum, newpg->dirty, newpg->rw);
                                if(t->dirty == 1) diskw++;
                                free(t);
                            }
                        }
                    }

                }
            }
    }
    
    printQuietMode(nframes, events,diskr,diskw); //prints
}
/*````VMS FUNCTION``````````````````````````````````````````````````````````````````````````````````*/

/*``PRINT FUNCTION``````````````````````````````````````````````````````````````````````````````````*/

void printQuietMode(int nframes, int eventsInTrace, int dR, int dW)
{
    printf("Total memory frames: %d\n", nframes);
    printf("Events in Trace: %d\n", eventsInTrace);
    printf("total disk reads: %d\n", dR);
    printf("total disk writes: %d\n", dW);
}
/*``PRINT FUNCTION``````````````````````````````````````````````````````````````````````````````````*/

