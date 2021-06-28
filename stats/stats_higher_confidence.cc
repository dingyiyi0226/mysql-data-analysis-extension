/* stats_higher_conficence function
 *
 * Description:
 *    Return the higher confidence x-value given data mean, size, sd and confidence level
 *
 * Usage:
 *   STATS_HIGHER_CONFIDENCE(X, probability)
 * 
 *   args[0]: input data
 *   args[1]: confidence probability / level (real number in [0, 1])
 *
 * Return:
 *   higher confidence x-value : double (REAL)
 *  
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include <vector>
#include <gsl/gsl_statistics.h>
#include "cmath"

using namespace std;

#define DECIMALS 6
#define Z_MAX 6

extern "C" {
  bool stats_higher_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message );
  void stats_higher_confidence_deinit( UDF_INIT* initid );
  void stats_higher_confidence_clear( UDF_INIT* initid, char* is_null, char* error );
  void stats_higher_confidence_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  void stats_higher_confidence_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
  double stats_higher_confidence( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );
}

struct stats_confidence_data {
  vector<double> data;
};

bool stats_higher_confidence_init( UDF_INIT* initid, UDF_ARGS* args, char* message )
{
  if(args->arg_count != 2) {
    strcpy(message, "Wrong number of arguments: STATS_HIGHER_CONFIDENCE() requires four arguments");
    return 1;
  }

  if(args->arg_type[0] != REAL_RESULT) {
    args->arg_type[0] = REAL_RESULT;
  }

  if(args->arg_type[1] != REAL_RESULT) {
    args->arg_type[1] = REAL_RESULT;
  }

  if(args->arg_type[1] != REAL_RESULT) {
    strcpy(message, "Wrong type of arguments: STATS_HIGHER_CONFIDENCE() requires a real number as parameter 1");
    return 1;
  }

  initid->decimals = DECIMALS;
  initid->maybe_null    = 0; 
  initid->max_length    = 20;

  stats_confidence_data* buffer = new stats_confidence_data;
  buffer->data.clear();
  initid->ptr = (char*) buffer;
  
  return 0;
}

void stats_higher_confidence_deinit( UDF_INIT* initid )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  delete buffer;
}

void stats_higher_confidence_clear( UDF_INIT* initid, char* is_null, char* error )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  buffer->data.clear();
}

void stats_higher_confidence_reset( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_higher_confidence_clear( initid, is_null, error );
  stats_higher_confidence_add( initid, args, is_null, error );
}

void stats_higher_confidence_add( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  if (args->args[0] != NULL) {
    buffer->data.push_back(*(double*)args->args[0]);
  }
}

static double poz(double z) {
  double y = 0.0;
  double x = 0.0;
  double w = 0.0;

  if (z == 0.0) {
    x = 0.0;
  } else {
    y = 0.5 * abs(z);
    if (y > (Z_MAX * 0.5)) {
      x = 1.0;
    } else if (y < 1.0) {
      w = y * y;
      x = ((((((((0.000124818987 * w
                - 0.001075204047) * w + 0.005198775019) * w
                - 0.019198292004) * w + 0.059054035642) * w
                - 0.151968751364) * w + 0.319152932694) * w
                - 0.531923007300) * w + 0.797884560593) * y * 2.0;
    } else {
      y -= 2.0;
      x = (((((((((((((-0.000045255659 * y
                + 0.000152529290) * y - 0.000019538132) * y
                - 0.000676904986) * y + 0.001390604284) * y
                - 0.000794620820) * y - 0.002034254874) * y
                + 0.006549791214) * y - 0.010557625006) * y
                + 0.011630447319) * y - 0.009279453341) * y
                + 0.005353579108) * y - 0.002141268741) * y
                + 0.000535310849) * y + 0.999936657524;
    }
  }
  if (z > 0.0) {
    return ((x + 1.0) * 0.5);
  } else {
    return ((1.0 - x) * 0.5);
  }
}

static double criticZ(double p) {
  double Z_EPSILON = 0.000001;   /* Accuracy of z approximation */
  double minz = -Z_MAX;
  double maxz = Z_MAX;
  double zval = 0.0;
  double pval = 0.0;

  if (p < 0.0) { p = 0.0; } 
  if (p > 1.0) { p = 1.0; }

  while ((maxz - minz) > Z_EPSILON) {
    pval = poz(zval);
    if (pval > p) {
      maxz = zval;
    } else {
      minz = zval;
    }
    zval = (maxz + minz) * 0.5;
  }
  return zval;
} 

double stats_higher_confidence( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error )
{
  double conf_prob;
  double sample_size;
  double mean;
  double std_dev;

  stats_confidence_data* buffer = (stats_confidence_data*) initid->ptr;

  if(args->arg_type[1]==INT_RESULT) {
    conf_prob = (double) *((long long*) args->args[1]);
  } else if(args->arg_type[1]==REAL_RESULT) {
    conf_prob = (double) *((double*) args->args[1]);
  }

  double* values = buffer->data.data();
  sample_size = buffer->data.size();
  mean = gsl_stats_mean(values, 1, buffer->data.size());
  std_dev = gsl_stats_sd(values, 1, buffer->data.size());

  double z_score = criticZ(conf_prob);
  return 1.0 * mean + (std_dev * z_score / sqrt(sample_size));
}