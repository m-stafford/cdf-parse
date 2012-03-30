/******************************************************************
 * Author: Matthew Stafford
 * Created: 2/15/2012
 * Modified: 2/15/2012
 * Version: 0.0.1
 * Name: cdf-parse
 * Desc: command line utility for parsing CDF data files
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#include "cdf.h"
#include "main.h"

#define MALLOC_ERROR -1 
#define ARG_ERROR -2

int main(int argc, char *argv[]) {
    int ret;
    char * filename;
    long i, j, k;

    long  num_recs, num_vars, num_dims;
    char * tmp_data;
    long data_type;
    long dim_sizes[CDF_MAX_DIMS];
    char * var_names[256];

    llist * data_list = NULL;
        
    llist * node;
    
    CDFid id;
    CDFstatus status;
    


    if (id <= 0) 
        handle_err(MALLOC_ERROR);

    if (argc < 2)
        handle_err(ARG_ERROR);

    /* Last arguement is the file */
    num_vars = argc - 2;
    filename = argv[1];

    ret = check_file(filename);
    if (ret < 0)
        handle_err(ret);

    status = CDFopenCDF(filename, &id);
    if (status != CDF_OK)
        handle_err(status);


    status = CDFlib(OPEN_, CDF_, filename, &id, SELECT_, CDF_, id, 
                SELECT_, zVAR_NAME_, argv[2], 
                GET_, zVAR_NUMRECS_, &num_recs, NULL_);
    handle_err(status);


    
    for (i = 0; i < num_recs; i++) {

        for (j = 0; j < num_vars; j++) {
            status = CDFlib(SELECT_, CDF_, id,
                        SELECT_, zVAR_NAME_, argv[2 + j],
                        SELECT_, zVAR_RECNUMBER_, i,
                        GET_, zVAR_NUMDIMS_, &num_dims, NULL_);
            handle_err(status);

            status = CDFlib(GET_, zVAR_DIMSIZES_, dim_sizes, 
                        GET_, zVAR_DATATYPE_, &data_type, NULL_);
            handle_err(status);

            if ((dim_sizes[0] > 0) && (num_dims != 0)) {
                for (k = 0; k < dim_sizes[0]; k++) {
                    tmp_data = (void *)calloc(1,dataTypeSize(data_type));
                     
                    status = CDFlib(SELECT_, zVAR_DIMINDICES_, &k, 
                                GET_, zVAR_DATA_, tmp_data, NULL_);

                    handle_err(status);
                    data_list = list_queue(data_list, data_type, tmp_data);
                }
            } else {
                tmp_data = (void *)calloc(1,dataTypeSize(data_type));
                status = CDFlib(GET_, zVAR_DATA_, tmp_data, NULL_);
                handle_err(status);
                data_list = list_queue(data_list, data_type, tmp_data);
            }
        }
        print_list(data_list);
        data_list = NULL;
    }

    return 0;
}

void handle_err(int status) {
    char message[CDF_STATUSTEXT_LEN+1];
    if (status < CDF_OK) {
        printf("An error has occurred, halting...\n");
        CDFlib(SELECT_, CDF_STATUS_, status,
           GET_, STATUS_TEXT_, &message[0],
           CLOSE_, CDF_, NULL_);
        printf("%s\n", message);
        
        exit(status);
    }
}

int check_file(char * filename) {
    return 0;
}

void print_list(llist * list) {
    llist * node = list;
    if (node != NULL)
    while (node != NULL) {
        printDataType(node->data, node->data_type);
        if (node->next != NULL) 
            printf(",");

        node = node->next;
    }
    printf("\n");
    
}

llist * list_queue(llist * list, long data_type, char * data) {
    llist * node;

    node = (llist *)malloc(sizeof(llist));    
    node->data_type = data_type;
    node->data = data;
    node->tail = node; //Only list head keeps track of tail
    node->next = NULL;

    if (list != NULL) {
        if (list->next == NULL)
            list->next = node;
        else
            list->tail->next = node;
        list->tail = node;
        return list;
    } else
        return node;
}

llist * list_dequeue(llist * list) {
    llist * node;
    if (list == NULL)
        return NULL;

    if (list->tail == list) 
        return list;
    else {
        node = list;
        list = list->next;
        list->tail = node->tail;
        list->next = node->next;
        return node;
    } 
}

llist * list_init() {
    llist * node;

    node = (llist *)malloc(sizeof(llist));
    node->tail = node;
    node->next = NULL;
    
    return node;
}

void list_free(llist * list) {
}

int dataTypeSize(long data_type) {
    int size;
    switch(data_type) {
        case CDF_FLOAT:
            size = sizeof(float);
            break;
        case CDF_DOUBLE:
            size = sizeof(double);
            break;
    } 
    return size;
}

void printDataType(char * data, long data_type) {
   switch(data_type) {
        case CDF_FLOAT:
            printf("%f", *(float *)data);    
            break;
        case CDF_DOUBLE:
            printf("%f", *(double *)data);    
            break;
    }
}
