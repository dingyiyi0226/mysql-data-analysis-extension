# MySQL UDF for Data Analysis

## Usage

Compile the source files

```
gcc -shared <udf.cc> -o <udf.so> -fPIC `mysql_config --include` `gsl-config --libs`
cp <udf.so> `mysql_config --plugindir`  # need sudo
```

Install/remove the function from MySQL

```
CREATE [AGGREGATE] FUNCTION <udf> RETURNS <STRING|INT|REAL> SONAME '<udf.so>';
DROP FUNCTION <udf>;
```

To show the STRING type UDF correctly, run the MySQL server with `--binary-as-hex=0` flag. ([reference](https://bugs.mysql.com/bug.php?id=99480))

```
mysql -u root -p --binary-as-hex=0
```
