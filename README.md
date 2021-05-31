# MySQL Data Analysis UDL

## Usage

Compile the source files

```
gcc -shared -o <udf.so> <udf.cc> -fPIC `mysql_config --include`
cp <udf.so> `mysql_config --plugindir`
```

Install/remove the function from MySQL

```
CREATE [AGGREGATE] FUNCTION <udf> RETURNS <TYPE> SONAME '<udf.so>';
DROP FUNCTION <udf>
```

