/* fit_linear function
 *
 * Description:
 *   Fit the data by Y = c0 + c1X
 *
 * Usage:
 *   FIT_LINEAR(`field`, interval_x)
 *   interval_x: the interval between two data points
 *
 * Return:
 *   A string of "c0 c1"
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <mysql.h>
#include <gsl/gsl_fit.h>

extern "C" {
    bool fit_linear_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void fit_linear_deinit(UDF_INIT *initid);
    void fit_linear_clear(UDF_INIT *initid, char *is_null, char *error);
    void fit_linear_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    char *fit_linear(UDF_INIT *initid, UDF_ARGS*args, char *result, unsigned long *length, char *is_null, char *error);
}

#define DECIMALS 6

struct fit_linear_data {
    size_t length;
    std::vector<double> data_x;
    std::vector<double> data_y;
};


bool fit_linear_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 2){
        strcpy(message, "wrong number of arguments: FIT_LINEAR() requires two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    initid->decimals = DECIMALS;

    fit_linear_data *data = new fit_linear_data;

    data->length = 0;
    data->data_x.clear();
    data->data_y.clear();

    initid->ptr = (char *)data;

    return 0;
}

void fit_linear_deinit(UDF_INIT *initid){
    fit_linear_data *data = (fit_linear_data *)initid->ptr;
    delete data;
}

void fit_linear_clear(UDF_INIT *initid, char *is_null, char *error){
    fit_linear_data *data = (fit_linear_data *)initid->ptr;

    data->length = 0;
    data->data_x.clear();
    data->data_y.clear();
}

void fit_linear_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    fit_linear_data *data = (fit_linear_data *)initid->ptr;
    double value = *(double *)args->args[0];
    double interval_x = *(double *)args->args[1];

    data->data_x.push_back(data->length * interval_x);
    data->data_y.push_back(value);
    data->length += 1;
}

char *fit_linear(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error){
    fit_linear_data *data = (fit_linear_data *)initid->ptr;

    double *data_x = data->data_x.data();  // convert vector to array
    double *data_y = data->data_y.data();

    double c0, c1, cov00, cov01, cov11, sumsq;

    gsl_fit_linear(data_x, 1, data_y, 1, data->length, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

    // return string should less than 255 bytes
    std::string ret_str = std::to_string(c0) + " " +std::to_string(c1);
    strcpy(result, ret_str.c_str());
    *length = ret_str.size();

    return result;
}
