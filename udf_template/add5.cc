#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql.h>

extern "C" {
    bool add5_init(UDF_INIT *inited,UDF_ARGS *args, char *message);
    void add5_deinit(UDF_INIT *inited);
    long long add5(UDF_INIT *inited, UDF_ARGS*args, char *is_null, char *error);
}

bool add5_init(UDF_INIT *inited,UDF_ARGS *args, char *message){
    if(args->arg_count != 1){
        strcpy(message, "add_5() can only accept one arg");
        return 1;
    }
    if (args->arg_type[0] != INT_RESULT){
        strcpy(message, "add_5() can only operate with a int");
        return 1;
    }
    return 0;
}

void add5_deinit(UDF_INIT *inited){
}

long long add5(UDF_INIT *inited, UDF_ARGS*args, char *is_null, char *error){
    long long res = (*(long long *)args->args[0]);
    return res + 5;
}
