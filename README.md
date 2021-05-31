

```
gcc -shared -o <udf.so> <udf.cc> -fPIC `mysql_config --include`
cp <udf.so> `mysql_config --plugindir`
```
