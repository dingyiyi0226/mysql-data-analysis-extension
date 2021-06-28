/* stats_median_sorted function
 *
 * Description:
 *    Calculate the median of (sorted) 'X'
 * Usage:
 *    STATS_MEDIAN_SORTED('X')
 * 
 *    args[0]: column data
 *
 * Return:
 *    median: double (REAL)
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
  bool stats_median_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void stats_median_sorted_deinit( UDF_INIT* initid );
  void stats_median_sorted_clear( UDF_INIT* initid, char* is_null, char* error );
  void stats_median_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_median_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double stats_median_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error  );
}

struct udf_median_sorted_data {
  vector<double> data;
};


bool stats_median_sorted_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 1) {
    strcpy(message, "Wrong number of arguments: STATS_MEDIAN_SORTED() requires one argument");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  initid->decimals = DECIMALS;
  
  udf_median_sorted_data* buffer = new udf_median_sorted_data;
  buffer->data.clear();

  initid->maybe_null = 1;
  initid->ptr = (char*) buffer;

  return 0;
}

void stats_median_sorted_deinit( UDF_INIT* initid )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  delete buffer;
}

void stats_median_sorted_clear( UDF_INIT* initid, char* is_null, char* error )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;
  
  buffer->data.clear();
}

void stats_median_sorted_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_median_sorted_clear(initid, is_null, error);
  stats_median_sorted_add( initid, args, is_null, error );
}

void stats_median_sorted_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  if(args->args[0] != NULL){
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

double stats_median_sorted( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  udf_median_sorted_data* buffer = (udf_median_sorted_data*) initid->ptr;

  double* values = buffer->data.data();
  gsl_sort(values, 1, buffer->data.size());

  double median = gsl_stats_median(values, 1, buffer->data.size());

  return median;
}