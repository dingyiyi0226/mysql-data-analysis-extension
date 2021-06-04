/* stats_variance function
 *
 * Description:
 *   Calculate the mean of `X`
 *
 * Usage:
 *   STATS_VARIANCE(`X`)
 *
 * Return:
 *   mean value: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_variance_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_variance_deinit(UDF_INIT *initid);
    void stats_variance_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_variance_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_variance(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_variance_data {
    std::vector<double> data;
};


bool stats_variance_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_VARIANCE() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_variance_data *data = new stats_variance_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_variance_deinit(UDF_INIT *initid){
    stats_variance_data *data = (stats_variance_data *)initid->ptr;
    delete data;
}

void stats_variance_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_variance_data *data = (stats_variance_data *)initid->ptr;

    data->data.clear();
}

void stats_variance_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_variance_data *data = (stats_variance_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_variance(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_variance_data *data = (stats_variance_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double mean = gsl_stats_variance(values, 1, data->data.size());

    return mean;
}
