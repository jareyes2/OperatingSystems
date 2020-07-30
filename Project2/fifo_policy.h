#ifndef FIFO_POLICY_H
#define FIFO_POLICY_H

struct PageEntryFifo 
{ 
    int pageNum, dirty;
    char rw;
    struct PageEntryFifo *next; 
}; 
  
  //page table 
struct PageTableFifo 
{ 
    struct PageEntryFifo *head, *tail; 
    int size;
    int MAX_SIZE_NFRAMES;
    int PAGE_FAULT_COUNT;
    int disk_write;
    int disk_read;    
    int table_hits;
}; 


struct PageEntryFifo* newPage(unsigned pgNum, int dirt, char _rw);
struct PageTableFifo *createTable();
struct PageEntryFifo *addPage(struct PageTableFifo *tbl, unsigned pgNum, int dirt, char _rw);
int entryInTbl(struct PageTableFifo *tbl, struct PageEntryFifo *lookup);
struct PageEntryFifo *removeFirstPage(struct PageTableFifo *tbl);

void fifo(char* file, int nframes, char* dq);
void printQuietMode(int nframes, int eventsInTrace, int dR, int dW);
void vms(char* file, int nframes, char* dq);
struct PageEntryFifo*  getTail(struct PageTableFifo *t);
int lookup(struct PageTableFifo *tbl, struct PageEntryFifo *lookup);

//cd functions 



#endif /* FIFO_POLICY */