#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql.h>
#include <gsl/gsl_sf_bessel.h>

extern "C" {
    bool gsl_test_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void gsl_test_deinit(UDF_INIT *initid);
    double gsl_test(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

bool gsl_test_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: gsl_test() requires one argument");
        return 1;
    }
    args->arg_type[0] = REAL_RESULT;
    initid->decimals = 6;
    return 0;
}

void gsl_test_deinit(UDF_INIT *initid){
}

double gsl_test(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error){
    double res = (*(double *)args->args[0]);
    return gsl_sf_bessel_J0(res);
}

