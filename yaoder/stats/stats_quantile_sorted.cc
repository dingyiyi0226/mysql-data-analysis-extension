/* stats_quantile_sorted function
 *
 * Description:
 *    Calculate the specific quantile of (sorted) 'X' with given percentile f
 * Usage:
 *    STATS_QUANTILE_SORTED('X', percentile)
 * 
 *    args[0]: column data
 *    args[1]: percentile f (f in [0, 100])
 *
 * Return:
 *    quantile: double (REAL)
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>

#include <vector>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort.h>

using namespace std;

#define DECIMALS 6

extern "C" {
  bool stats_quantile_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void stats_quantile_sorted_deinit( UDF_INIT* initid );
  void stats_quantile_sorted_clear( UDF_INIT* initid, char* is_null, char* error );
  void stats_quantile_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_quantile_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double stats_quantile_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct udf_quantile_sorted_data {
  unsigned long percentile;
  vector<double> data;
};

bool stats_quantile_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count < 1 || args->arg_count > 2) {
    strcpy(message, "Wrong number of arguments: STATS_QUANTILE_SORTED() requires one or two arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  /* Default median percentage is 50% */
  unsigned long percentile = 50;
  if(args->arg_count==2 && (*((long*) args->args[1])>=0) && (*((long*) args->args[1])<=100)) {
    percentile = *((long*) args->args[1]);
  }

  initid->decimals = DECIMALS;

  udf_quantile_sorted_data* buffer = new udf_quantile_sorted_data;
  buffer->percentile = percentile;
  buffer->data.clear();

  initid->maybe_null = 1;
  initid->ptr = (char*) buffer;

  return 0;
}

void stats_quantile_sorted_deinit( UDF_INIT* initid )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  delete buffer;
}

void stats_quantile_sorted_clear( UDF_INIT* initid, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;
  
  buffer->data.clear();
}

void stats_quantile_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_quantile_sorted_clear(initid, is_null, error);
  stats_quantile_sorted_add(initid, args, is_null, error);
}

void stats_quantile_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  if(args->args[0] != NULL) {
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

double stats_quantile_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_quantile_sorted_data* buffer = (udf_quantile_sorted_data*) initid->ptr;

  double* values = buffer->data.data();
  gsl_sort(values, 1, buffer->data.size());

  double quantile = gsl_stats_quantile_from_sorted_data(values, 1, buffer->data.size(), (double)buffer->percentile / 100.00);
  
  return quantile;
}

