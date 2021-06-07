/* stats_skew function
 *
 * Description:
 *   Calculate the skew of `X`
 *
 * Usage:
 *   STATS_SKEW(`X`)
 *
 * Return:
 *   skew: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_skew_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_skew_deinit(UDF_INIT *initid);
    void stats_skew_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_skew_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_skew(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_skew_data {
    std::vector<double> data;
};


bool stats_skew_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_SKEW() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_skew_data *data = new stats_skew_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_skew_deinit(UDF_INIT *initid){
    stats_skew_data *data = (stats_skew_data *)initid->ptr;
    delete data;
}

void stats_skew_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_skew_data *data = (stats_skew_data *)initid->ptr;

    data->data.clear();
}

void stats_skew_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_skew_data *data = (stats_skew_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_skew(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_skew_data *data = (stats_skew_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double skew = gsl_stats_skew(values, 1, data->data.size());

    return skew;
}
