#include "cdf.h"

typedef struct llist llist;

struct llist {
    llist * tail;
    llist * next; 
    long data_type;
    char * data;
};

typedef struct fdata {
    float value;
} fdata;

typedef struct ddata {
    float value;
} ddata;

int check_file(char *);
void handle_err(int);
double parse_time(char *);
llist * list_queue(llist *, long, char *);
llist * list_dequeue(llist *);
llist * list_init();
void list_free(llist *);
void print_list(llist *);
int dataTypeSize(long) ;
void printDataType(char *, long);
