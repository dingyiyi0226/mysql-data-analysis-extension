/* stats_sd function
 *
 * Description:
 *   Calculate the standard deviation of `X`
 *
 * Usage:
 *   STATS_SD(`X`)
 *
 * Return:
 *   sd: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_sd_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_sd_deinit(UDF_INIT *initid);
    void stats_sd_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_sd_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_sd(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_sd_data {
    std::vector<double> data;
};


bool stats_sd_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_SD() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_sd_data *data = new stats_sd_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_sd_deinit(UDF_INIT *initid){
    stats_sd_data *data = (stats_sd_data *)initid->ptr;
    delete data;
}

void stats_sd_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_sd_data *data = (stats_sd_data *)initid->ptr;

    data->data.clear();
}

void stats_sd_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_sd_data *data = (stats_sd_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_sd(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_sd_data *data = (stats_sd_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double sd = gsl_stats_sd(values, 1, data->data.size());

    return sd;
}
