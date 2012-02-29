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

#include "cdf.h"
#include "main.h"

#define MALLOC_ERROR -1 
#define ARG_ERROR -2

int main(int argc, char *argv[]) {
    int ret;
    char * filename;
    int i, j, k, print;

    char  copyright[CDF_COPYRIGHT_LEN+1];
    long  recNum, numRecs;
    long pos;
    long  d_size[CDF_MAX_DIMS];
    long  version, release, increment;
    double t_start, t_end;

    zlist * head; 
    zlist * tail;
    zlist * curr;

    CDFid id;
    CDFstatus status;

    id = (CDFid *)malloc(sizeof(CDFid));
    head = NULL;
    tail = NULL;
    curr = NULL;

    if (id <= 0) 
        handle_err(MALLOC_ERROR);

    if (argc < 2)
        handle_err(ARG_ERROR);

    /* Last arguement is the file */
    filename = argv[argc - 1];
    ret = check_file(filename);
    if (ret < 0)
        handle_err(ret);

    status = CDFopenCDF(filename, &id);
    if (status != CDF_OK)
        handle_err(status);

    status = CDFgetCopyright (id, copyright);
    if (status != CDF_OK) 
        handle_err(status);

    //printf ("CDF copyright:\n\t%s\n\n", copyright);

    /* Get the CDF version that was used to create this CDF file */
    status = CDFgetVersion (id, &version, &release, &increment);
    if (status != CDF_OK)
        handle_err(status);

    for (i = 0; i < (argc - 4); i++) {
        if (head == NULL) {
            head = zlist_init(id, argv[i + 3]);
            tail = head;
        } else {
            curr = zlist_init(id, argv[i + 3]);
            curr->prev = (char *)tail;
            tail->next = (char *)curr;
            tail = (zlist *)tail->next;
        }
    }
    
    recNum = 0;

    curr = head;
    
    while (curr != NULL) {
        status = CDFsetzVarSeqPos(id, curr->num, recNum, curr->dim_indices);
        if (status != CDF_OK) 
            handle_err (status);

        curr = (zlist *)curr->next;
    }

    curr = head;

    t_start = parse_time(argv[1]); 
    t_end = parse_time(argv[2]);

#if 1
    status = CDFgetzVarNumRecsWritten(id, head->num, &numRecs);
    for (j=0; j < numRecs; j++) {
        curr = head;
        print = 1;
        while (curr != NULL) {
            if (curr->dims > 0) {
                for (k = 0; k < curr->dim_sizes[0]; k++) {
                    status = CDFgetzVarSeqData(id, curr->num, curr->data);

                    status = CDFgetzVarSeqPos(id, curr->num, &pos, d_size);
                    if (status != CDF_OK) 
                        handle_err (status);
                    if (curr->datatype == CDF_FLOAT && print == 1)  {
                        printf("%f,", *(float *)curr->data);
                    }
                    else if (curr->datatype == CDF_DOUBLE) {
                        if (*(double *)curr->data > t_start &&
                                *(double *)curr->data < t_end) {
                            printf("%f,", *(double *)curr->data);
                        }
                        else {
                            print = 0;
                        }
                    }
                }
            } else {
                status = CDFgetzVarSeqData(id, curr->num, curr->data);
                status = CDFgetzVarSeqPos(id, curr->num, &pos, d_size);
                if (status != CDF_OK) 
                    handle_err (status);
                if (curr->datatype == CDF_FLOAT && print == 1) 
                    printf("%f,", *(float *)curr->data);
                else if (curr->datatype == CDF_DOUBLE) {
                    if (*(double *)curr->data > t_start &&
                            *(double *)curr->data < t_end) {
                        printf("%f,", *(double *)curr->data);
                    }
                    else {
                        print = 0;
                    }
                }
            }
            curr = curr->next;
        }
        if (print)
            printf("\n");
    }
#endif

    return 0;
}

void handle_err(int status) {
    char message[CDF_STATUSTEXT_LEN+1];

    if (status < CDF_WARN) {
        printf("An error has occurred, halting...\n");
        CDFgetStatusText(status, message);
        printf("%s\n", message);
        exit(status);
    }
    else
        if (status < CDF_OK) {
            printf("Warning, function may not have completed as expected...\n");
            CDFgetStatusText(status, message);
            printf("%s\n", message);
        }
        else
            if (status > CDF_OK) {
                printf("Function completed successfully, but be advised that...\n");
                CDFgetStatusText(status, message);
                printf("%s\n", message);
            }       
    return;
}

zlist * zlist_init(CDFid id, char * var_name) {
    int status;
    long num;
    long num_recs;
    long len;
    int i;

    zlist * z_attrs;

    num = CDFgetVarNum(id, var_name);
    if (num < CDF_OK)
        return NULL;

    status = CDFgetzVarAllocRecords(id, num, &num_recs);
    
    z_attrs = (zlist *)malloc(sizeof(zlist));
    z_attrs->prev = NULL;
    z_attrs->next = NULL;

    z_attrs->num = num;
    
    strncpy(z_attrs->name, var_name, 256);
    status = CDFinquirezVar(id, z_attrs->num, z_attrs->name,
            &z_attrs->datatype, &z_attrs->num_elements, 
            &z_attrs->dims, z_attrs->dim_sizes, &z_attrs->rec_vary,
            z_attrs->dim_varys);
 
    for (i = 0; i < z_attrs->dims; i++) 
        z_attrs->dim_indices[i] = 0;

    status = CDFgetDataTypeSize(z_attrs->datatype, &len);
    len = len * z_attrs->num_elements;

    if (z_attrs->datatype == CDF_CHAR || z_attrs->datatype == CDF_UCHAR) 
        ++len;
    
    z_attrs->data = (void *)malloc(len);

    return z_attrs;
}

/* Parse command line time arguments into seconds from epoch */
double parse_time(char * time_str) {
    struct tm * time;
    double secs;

    time = (struct tm *)calloc(1, sizeof(struct tm));
     
    sscanf(time_str, "%d/%d/%d %d:%d:%d", &time->tm_mon, &time->tm_mday, &time->tm_year,
            &time->tm_hour, &time->tm_min, &time->tm_sec);
    
    time->tm_mon -= 1;
    time->tm_year += 100;
    secs = gmtime(time);

    return secs; 
}

int check_file(char * filename) {
    return 0;
}
