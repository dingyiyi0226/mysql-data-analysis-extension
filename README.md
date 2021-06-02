# MySQL UDF for Data Analysis

## Usage

Compile the source files

```
gcc -shared <udf.so> -o <udf.cc> -fPIC `mysql_config --include` `gsl-config --libs`
cp <udf.so> `mysql_config --plugindir`  # need sudo
```

Install/remove the function from MySQL

```
CREATE [AGGREGATE] FUNCTION <udf> RETURNS <STRING|INT|REAL> SONAME '<udf.so>';
DROP FUNCTION <udf>;
```

