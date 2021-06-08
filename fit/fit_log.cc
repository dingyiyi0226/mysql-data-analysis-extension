/* fit_log function
 *
 * Description:
 *   Fit the data by Y = c0 * log(X) + c1
 *
 * Usage:
 *   FIT_LOG(`X`, `Y`, data_length)
 *
 * Return:
 *   "c0 c1": char * (STRING)
 *
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

extern "C" {
    bool fit_log_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void fit_log_deinit(UDF_INIT *initid);
    void fit_log_clear(UDF_INIT *initid, char *is_null, char *error);
    void fit_log_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    char *fit_log(UDF_INIT *initid, UDF_ARGS*args, char *result, unsigned long *length, char *is_null, char *error);
}

#define DECIMALS 6

struct fit_log_data {
    size_t nth_row;
    size_t length;
    size_t var_num;

    gsl_vector *x;
    gsl_vector *y;

    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params;

    gsl_vector *f;
    gsl_matrix *J;
    gsl_matrix *covar;
};

int log_f(const gsl_vector *x, void *data, gsl_vector *f){
    size_t n = ((fit_log_data *)data)->length;
    gsl_vector *data_x = ((fit_log_data *)data)->x;
    gsl_vector *data_y = ((fit_log_data *)data)->y;

    double c0 = gsl_vector_get(x, 0);
    double c1 = gsl_vector_get(x, 1);
    double Yi;

    for(int i=0; i<n; i++){
        Yi = c0 * log(gsl_vector_get(data_x, i)) + c1;
        gsl_vector_set(f, i, Yi - gsl_vector_get(data_y, i));
    }

    return GSL_SUCCESS;
}

bool fit_log_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 3){
        strcpy(message, "wrong number of arguments: FIT_LOG() requires three arguments");
        return 1;
    }

    if(args->arg_type[2] != INT_RESULT){
        strcpy(message, "wrong type of the third argument: requires an INT");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    initid->decimals = DECIMALS;

    fit_log_data *data = new fit_log_data;

    data->length = *(int *)args->args[2];
    data->var_num = 2;  // c0, c1
    data->nth_row = 0;


    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;

    data->fdf_params = gsl_multifit_nlinear_default_parameters();

    data->x = gsl_vector_alloc(data->length);
    data->y = gsl_vector_alloc(data->length);

    data->fdf.f = log_f;
    data->fdf.df = NULL;   /* set to NULL for finite-difference Jacobian */
    data->fdf.fvv = NULL;  /* not using geodesic acceleration */
    data->fdf.n = data->length;
    data->fdf.p = data->var_num;
    data->fdf.params = (void *)data;

    data->w = gsl_multifit_nlinear_alloc(T, &data->fdf_params, data->length, data->var_num);
    data->covar = gsl_matrix_alloc(data->var_num, data->var_num);

    initid->ptr = (char *)data;

    return 0;
}

void fit_log_deinit(UDF_INIT *initid){
    fit_log_data *data = (fit_log_data *)initid->ptr;

    gsl_vector_free(data->x);
    gsl_vector_free(data->y);
    gsl_multifit_nlinear_free (data->w);
    gsl_matrix_free (data->covar);

    delete data;
}

void fit_log_clear(UDF_INIT *initid, char *is_null, char *error){
    fit_log_data *data = (fit_log_data *)initid->ptr;

    data->nth_row = 0;
}

void fit_log_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    fit_log_data *data = (fit_log_data *)initid->ptr;

    gsl_vector_set(data->x, data->nth_row, *(double *)args->args[0]);
    gsl_vector_set(data->y, data->nth_row, *(double *)args->args[1]);

    data->nth_row += 1;
}

char *fit_log(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error){
    fit_log_data *data = (fit_log_data *)initid->ptr;

    double chisq, chisq0;
    int status, info;

    const double xtol = 1e-8;
    const double gtol = 1e-8;
    const double ftol = 0.0;

    double x_init[3] = { 1.0, 0.0 };  // starting values
    gsl_vector_view x = gsl_vector_view_array(x_init, data->var_num);
    gsl_multifit_nlinear_init (&x.vector, &data->fdf, data->w);

    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, NULL, NULL, &info, data->w);

    data->J = gsl_multifit_nlinear_jac(data->w);
    gsl_multifit_nlinear_covar(data->J, 0.0, data->covar);

    data->f = gsl_multifit_nlinear_residual(data->w);
    gsl_blas_ddot(data->f, data->f, &chisq);


    *length = sprintf(result, "%f %f",
        gsl_vector_get(data->w->x, 0), gsl_vector_get(data->w->x, 1));

    return result;
}
