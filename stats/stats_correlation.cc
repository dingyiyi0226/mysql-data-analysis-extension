/* stats_correlation function
 *
 * Description:
 *   Calculate the correlation of `X` and `Y`
 *
 * Usage:
 *   STATS_CORRELATION(`X`, `Y`)
 *
 * Return:
 *   correlation: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_correlation_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_correlation_deinit(UDF_INIT *initid);
    void stats_correlation_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_correlation_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_correlation(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_correlation_data {
    size_t length;
    std::vector<double> data_x;
    std::vector<double> data_y;
};


bool stats_correlation_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 2){
        strcpy(message, "wrong number of arguments: STATS_CORRELATION() requires two arguments");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    args->arg_type[1] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_correlation_data *data = new stats_correlation_data;

    data->length = 0;
    data->data_x.clear();
    data->data_y.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_correlation_deinit(UDF_INIT *initid){
    stats_correlation_data *data = (stats_correlation_data *)initid->ptr;
    delete data;
}

void stats_correlation_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_correlation_data *data = (stats_correlation_data *)initid->ptr;

    data->length = 0;
    data->data_x.clear();
    data->data_y.clear();
}

void stats_correlation_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_correlation_data *data = (stats_correlation_data *)initid->ptr;

    data->data_x.push_back(*(double *)args->args[0]);
    data->data_y.push_back(*(double *)args->args[1]);
    data->length += 1;
}

double stats_correlation(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_correlation_data *data = (stats_correlation_data *)initid->ptr;

    double *data_x = data->data_x.data();  // convert vector to array
    double *data_y = data->data_y.data();

    double correlation = gsl_stats_correlation(data_x, 1, data_y, 1, data->length);

    return correlation;
}
