/* stats_lag1_autocorrelation function
 *
 * Description:
 *   Calculate the lag1_autocorrelation of `X`
 *
 * Usage:
 *   STATS_LAG1_AUTOCORRELATION(`X`)
 *
 * Return:
 *   lag1_autocorrelation: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_lag1_autocorrelation_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_lag1_autocorrelation_deinit(UDF_INIT *initid);
    void stats_lag1_autocorrelation_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_lag1_autocorrelation_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_lag1_autocorrelation(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_lag1_autocorrelation_data {
    std::vector<double> data;
};


bool stats_lag1_autocorrelation_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_LAG1_AUTOCORRELATION() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_lag1_autocorrelation_data *data = new stats_lag1_autocorrelation_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_lag1_autocorrelation_deinit(UDF_INIT *initid){
    stats_lag1_autocorrelation_data *data = (stats_lag1_autocorrelation_data *)initid->ptr;
    delete data;
}

void stats_lag1_autocorrelation_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_lag1_autocorrelation_data *data = (stats_lag1_autocorrelation_data *)initid->ptr;

    data->data.clear();
}

void stats_lag1_autocorrelation_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_lag1_autocorrelation_data *data = (stats_lag1_autocorrelation_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_lag1_autocorrelation(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_lag1_autocorrelation_data *data = (stats_lag1_autocorrelation_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double lag1_autocorrelation = gsl_stats_lag1_autocorrelation(values, 1, data->data.size());

    return lag1_autocorrelation;
}
