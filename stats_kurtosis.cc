/* stats_kurtosis function
 *
 * Description:
 *   Calculate the kurtosis of `X`
 *
 * Usage:
 *   STATS_KURTOSIS(`X`)
 *
 * Return:
 *   kurtosis: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_kurtosis_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_kurtosis_deinit(UDF_INIT *initid);
    void stats_kurtosis_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_kurtosis_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_kurtosis(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_kurtosis_data {
    std::vector<double> data;
};


bool stats_kurtosis_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_KURTOSIS() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_kurtosis_data *data = new stats_kurtosis_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_kurtosis_deinit(UDF_INIT *initid){
    stats_kurtosis_data *data = (stats_kurtosis_data *)initid->ptr;
    delete data;
}

void stats_kurtosis_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_kurtosis_data *data = (stats_kurtosis_data *)initid->ptr;

    data->data.clear();
}

void stats_kurtosis_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_kurtosis_data *data = (stats_kurtosis_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_kurtosis(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_kurtosis_data *data = (stats_kurtosis_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double kurtosis = gsl_stats_kurtosis(values, 1, data->data.size());

    return kurtosis;
}
