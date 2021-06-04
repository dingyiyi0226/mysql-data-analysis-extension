/* stats_tss function
 *
 * Description:
 *   Calculate the total sum of squares of `X`
 *
 * Usage:
 *   STATS_TSS(`X`)
 *
 * Return:
 *   tss: double (REAL)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <mysql.h>
#include <gsl/gsl_statistics.h>

extern "C" {
    bool stats_tss_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void stats_tss_deinit(UDF_INIT *initid);
    void stats_tss_clear(UDF_INIT *initid, char *is_null, char *error);
    void stats_tss_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double stats_tss(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

#define DECIMALS 6

struct stats_tss_data {
    std::vector<double> data;
};


bool stats_tss_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: STATS_TSS() requires one argument");
        return 1;
    }

    args->arg_type[0] = REAL_RESULT;
    initid->decimals = DECIMALS;

    stats_tss_data *data = new stats_tss_data;
    data->data.clear();

    initid->ptr = (char *)data;

    return 0;
}

void stats_tss_deinit(UDF_INIT *initid){
    stats_tss_data *data = (stats_tss_data *)initid->ptr;
    delete data;
}

void stats_tss_clear(UDF_INIT *initid, char *is_null, char *error){
    stats_tss_data *data = (stats_tss_data *)initid->ptr;

    data->data.clear();
}

void stats_tss_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_tss_data *data = (stats_tss_data *)initid->ptr;

    data->data.push_back(*(double *)args->args[0]);
}

double stats_tss(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    stats_tss_data *data = (stats_tss_data *)initid->ptr;

    double *values = data->data.data();  // convert vector to array

    double tss = gsl_stats_tss(values, 1, data->data.size());

    return tss;
}
