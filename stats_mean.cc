/* stats_mean function
 *
 * Description:
 *   Calculate the mean of `X`
 *
 * Usage:
 *   STATS_MEAN(`X`)
 *
 * Return:
 *   mean: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_mean_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_mean_deinit(UDF_INIT *initid);
    void stats_mean_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_mean_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_mean(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_mean_data {
    std::vector<double> data;
};


bool stats_mean_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_MEAN() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_mean_data *data = new stats_mean_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_mean_deinit(UDF_INIT *initid){
    stats_mean_data *data = (stats_mean_data *)initid->ptr;
    delete data;
}

void stats_mean_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_mean_data *data = (stats_mean_data *)initid->ptr;

    data->data.clear();
}

void stats_mean_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_mean_data *data = (stats_mean_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_mean(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_mean_data *data = (stats_mean_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double mean = gsl_stats_mean(values, 1, data->data.size());

    return mean;
}
