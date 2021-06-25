/* fit_polyn function
 *
 * Description:
 *   Fit the data by Y = c0 + c1X + c2 X^2 + ... + cn X^n
 *
 * Usage:
 *   FIT_POLYN(`X`, `Y`, degree, data_length)
 *
 * Return:
 *   "c0 c1 c2 ...": char * (STRING)
 *
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#include <mysql.h>
#include <gsl/gsl_multifit.h>

using namespace std;

extern "C" {
    bool fit_polyn_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void fit_polyn_deinit(UDF_INIT *initid);
    void fit_polyn_clear(UDF_INIT *initid, char *is_null, char *error);
    void fit_polyn_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    char *fit_polyn(UDF_INIT *initid, UDF_ARGS*args, char *result, unsigned long *length, char *is_null, char *error);
}

#define DECIMALS 6

struct fit_prepare {
    size_t N_row;
    size_t degree;
    size_t y0;

    gsl_multifit_linear_workspace *ws;
    gsl_matrix *cov, *X;
    gsl_vector *y, *c;
};


bool fit_polyn_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 4){
        strcpy(message, "wrong number of arguments: FIT_POLYN() requires two arguments");
        return 1;
    }

    if(args->arg_type[2] != INT_RESULT || args->arg_type[3] != INT_RESULT){
        strcpy(message, "wrong type of the third or fourth argument: requires an INT");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;

    fit_prepare *prepare = new fit_prepare;
    prepare->degree = *((int *)args->args[2]);
    prepare->N_row = *((int *)args->args[3]);
    prepare->y0 = 0;
    
    prepare->X = gsl_matrix_alloc(prepare->N_row, prepare->degree+1);
    prepare->y = gsl_vector_alloc(prepare->N_row);
    prepare->c = gsl_vector_alloc(prepare->degree+1);
    prepare->cov = gsl_matrix_alloc(prepare->degree+1, prepare->degree+1);
    prepare->ws = gsl_multifit_linear_alloc(prepare->N_row, prepare->degree+1);

    initid->decimals = DECIMALS;
    initid->ptr = (char *)prepare;

    return 0;
}

void fit_polyn_deinit(UDF_INIT *initid){
    fit_prepare *prepare = (fit_prepare *)initid->ptr;
    gsl_multifit_linear_free(prepare->ws);
    gsl_matrix_free(prepare->X);
    gsl_matrix_free(prepare->cov);
    gsl_vector_free(prepare->y);
    gsl_vector_free(prepare->c);
    delete prepare;
}

void fit_polyn_clear(UDF_INIT *initid, char *is_null, char *error){
    fit_prepare *prepare = (fit_prepare *)initid->ptr;
}

void fit_polyn_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    fit_prepare *prepare = (fit_prepare *)initid->ptr;

    gsl_vector_set(prepare->y,prepare->y0 , *(double *)args->args[1]);

    for(int j=0; j<=prepare->degree; j++){
        gsl_matrix_set(prepare->X,prepare->y0,j,pow(*(double *)args->args[0], j));
    }

    prepare->y0 += 1;
}

char *fit_polyn(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error){
    fit_prepare *prepare = (fit_prepare *)initid->ptr;

    double chisq;

    gsl_multifit_linear(prepare->X, prepare->y, prepare->c, prepare->cov, &chisq, prepare->ws);

    // return string should less than 255 bytes
    string ret_str;

    for(int i=0; i <= prepare->degree; i++){
        ret_str += to_string(gsl_vector_get(prepare->c, i));
        ret_str += " ";
    }

    strcpy(result, ret_str.c_str());
    *length = ret_str.size();

    return result;
}