#include "cdf.h"

typedef struct zlist {
    char * prev;
    char * next;

    long num;
    char name[CDF_VAR_NAME_LEN256 + 1];
    char dep_var[CDF_VAR_NAME_LEN256 + 1];
    long datatype;
    long num_elements;
    long dims;
    long dim_sizes[CDF_MAX_DIMS];
    long dim_indices[CDF_MAX_DIMS];
    long dim_varys[CDF_MAX_DIMS];
    long rec_vary;
} zlist;

struct fgm_gse_rec {
    long var_num[2]; /* CDF Variable Number */
    double time; /* Dependent Var */
    float b_field[3]; /* Three Coordinate Magnetic Field */
};

struct event {
    long rec_num;
};

void fetch_fgm_gse_rec(CDFid, long, struct fgm_gse_rec *);
zlist * zlist_init(CDFid, char *); 
int check_file(char *);
void handle_err(int);
double parse_time(char *);
