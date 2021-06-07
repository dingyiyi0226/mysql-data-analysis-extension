/* fit_square function
 *
 * Description:
 *   Fit the data by Y = c0 + c1X + c2 X^2
 *
 * Usage:
 *   FIT_SQUARE(`X`, `Y`, data_length)
 *
 * Return:
 *   "c0 c1 c2": char * (STRING)
 *
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_multifit.h>


extern "C" {
    bool fit_square_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void fit_square_deinit(UDF_INIT *initid);
    void fit_square_clear(UDF_INIT *initid, char *is_null, char *error);
    void fit_square_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    char *fit_square(UDF_INIT *initid, UDF_ARGS*args, char *result, unsigned long *length, char *is_null, char *error);
}

#define DECIMALS 6

struct fit_square_data {
    size_t nth_row;
    size_t length;

    gsl_matrix *X;
    gsl_vector *y;

    gsl_vector *c;
    gsl_matrix *cov;
    gsl_multifit_linear_workspace *work;
};


bool fit_square_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 3){
        strcpy(message, "wrong number of arguments: FIT_SQUARE() requires three arguments");
        return 1;
    }

    if(args->arg_type[2] != INT_RESULT){
        strcpy(message, "wrong type of the third argument: requires an INT");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    initid->decimals = DECIMALS;

    fit_square_data *data = new fit_square_data;

    data->length = *(int *)args->args[2];
    data->nth_row = 0;

    data->X = gsl_matrix_alloc(data->length, 3);
    data->y = gsl_vector_alloc(data->length);
    data->c = gsl_vector_alloc(3);
    data->cov = gsl_matrix_alloc(3, 3);
    data->work = gsl_multifit_linear_alloc(data->length, 3);

    initid->ptr = (char *)data;

    return 0;
}

void fit_square_deinit(UDF_INIT *initid){
    fit_square_data *data = (fit_square_data *)initid->ptr;

    gsl_matrix_free(data->X);
    gsl_vector_free(data->y);
    gsl_vector_free(data->c);
    gsl_matrix_free(data->cov);
    gsl_multifit_linear_free(data->work);

    delete data;
}

void fit_square_clear(UDF_INIT *initid, char *is_null, char *error){
    fit_square_data *data = (fit_square_data *)initid->ptr;

    data->nth_row = 0;
}

void fit_square_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    fit_square_data *data = (fit_square_data *)initid->ptr;

    gsl_matrix_set(data->X, data->nth_row, 0, 1.0);
    gsl_matrix_set(data->X, data->nth_row, 1, *(double *)args->args[0]);
    gsl_matrix_set(data->X, data->nth_row, 2, pow(*(double *)args->args[0], 2));
    gsl_vector_set(data->y, data->nth_row, *(double *)args->args[1]);

    data->nth_row += 1;
}

char *fit_square(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error){
    fit_square_data *data = (fit_square_data *)initid->ptr;

    double chisq;
    gsl_multifit_linear(data->X, data->y, data->c, data->cov, &chisq, data->work);

    // return string should less than 255 bytes

    *length = sprintf(result, "%f %f %f",
        gsl_vector_get(data->c, 0), gsl_vector_get(data->c, 1), gsl_vector_get(data->c, 2));

    return result;
}
