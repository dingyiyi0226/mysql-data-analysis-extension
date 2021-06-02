#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql.h>

extern "C" {
    bool add5_init(UDF_INIT *initid,UDF_ARGS *args, char *message);
    void add5_deinit(UDF_INIT *initid);
    long long add5(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error);
}

bool add5_init(UDF_INIT *initid,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "wrong number of arguments: add5() requires one argument");
        return 1;
    }
    if (args->arg_type[0] != INT_RESULT){
        strcpy(message, "wrong argument type: add5() requires a INT");
        return 1;
    }
    return 0;
}

void add5_deinit(UDF_INIT *initid){
}

long long add5(UDF_INIT *initid, UDF_ARGS*args, char *is_null, char *error){
    long long res = (*(long long *)args->args[0]);
    return res + 5;
}
