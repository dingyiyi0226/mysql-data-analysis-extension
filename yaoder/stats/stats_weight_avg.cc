/* stats_weight_avg function
 *
 * Description:
 *    Calculate the weighted average of 'X' with respective weights
 * Usage:
 *    STATS_MEDIAN('X', weight)
 * 
 *    args[0]: column data
 *    args[1]: corresponding weight (real)
 *
 * Return:
 *    weighted average: double (REAL)
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include "cmath"

using namespace std;

#define DECIMALS 6

extern "C" {
  bool stats_weight_avg_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void stats_weight_avg_deinit( UDF_INIT* initid );
  void stats_weight_avg_clear( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_weight_avg_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_weight_avg_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double stats_weight_avg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct weight_avg_data {
  unsigned long count;
  double data_sum;
  double weight_sum;
};

bool stats_weight_avg_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 2) {
    strcpy(message, "Wrong number of arguments: UDF_WEIGHT_AVG() requires two arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: UDF_WEIGHT_AVG() requires a real number as parameter 1");
    return 1;
  }

  if(args->arg_type[1] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: UDF_WEIGHT_AVG() requires a real number as parameter 2");
    return 1;
  }

  weight_avg_data* buffer = new weight_avg_data;
  buffer->count = 0;
  buffer->data_sum = 0.0;
  buffer->weight_sum = 0.0;

  initid->decimals = DECIMALS;
  initid->maybe_null	= 1;
  initid->max_length	= 32;
  initid->ptr = (char*) buffer;

  return 0;
}

void stats_weight_avg_deinit( UDF_INIT* initid )
{
  delete initid->ptr;
}

void stats_weight_avg_clear( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

  buffer->count = 0;
  buffer->data_sum = 0;
  buffer->weight_sum = 0;
}

void stats_weight_avg_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_weight_avg_clear( initid, args, is_null, error );
  stats_weight_avg_add( initid, args, is_null, error );
}

void stats_weight_avg_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{ 
  if(args->args[0] != NULL && args->args[1] != NULL) {
    weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

    buffer->data_sum += ((double) *(double*) args->args[0]) * ((double) *(double*) args->args[1]);
    buffer->weight_sum += (double) *(double*) args->args[1];
    buffer->count ++;
  }
}

double stats_weight_avg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  weight_avg_data* buffer = (weight_avg_data*) initid->ptr;

  if (buffer->count == 0 || buffer->weight_sum == 0 ) {

    return 0.0;
  }

  return (double) buffer->data_sum / buffer->weight_sum;
}
