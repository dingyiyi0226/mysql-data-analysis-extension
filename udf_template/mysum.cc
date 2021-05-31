#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql.h>

extern "C" {
    bool mysum_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void mysum_deinit(UDF_INIT *initid);
    void mysum_clear(UDF_INIT *initid, char *is_null, char *error);
    void mysum_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
    double mysum(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

struct mysum_data {
    double sum;
};

bool mysum_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: MYSUM() requires one argument");
        return 1;
    }
    if (args->arg_type[0] != REAL_RESULT){
        strcpy(message, "wrong argument type: MYSUM() requires a  REAL");
        return 1;
    }

    mysum_data *data = new mysum_data;
    data->sum = 0.;

    initid->ptr = (char *)data;

    return 0;
}

void mysum_deinit(UDF_INIT *initid){
    mysum_data *data = (mysum_data *)initid->ptr;
    delete data;
}


void mysum_clear(UDF_INIT *initid, char *is_null, char *error){
    mysum_data *data = (mysum_data *)initid->ptr;
    data->sum = 0.;
}


void mysum_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
    mysum_data *data = (mysum_data *)initid->ptr;
    double value = *(double *)args->args[0];
    data->sum += value;
}

double mysum(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error){
    mysum_data *data = (mysum_data *)initid->ptr;
    return data->sum;
}
