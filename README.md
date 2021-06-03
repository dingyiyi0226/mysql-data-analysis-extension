# MySQL UDF for Data Analysis

## Prerequisite
-   [GSL](https://www.gnu.org/software/gsl/) >= 2.5

## Usage

### Compile

1.  Compile the source files via `make`

    ```
    make
    ```

2.  Install the library to MySQL

    ```
    sudo make install
    ```

### MySQL

1.  Launch MySQL server with `--binary-as-hex=0` flag. ([reference](https://bugs.mysql.com/bug.php?id=99480))

    ```
    mysql -u root -p --binary-as-hex=0
    ```

2.  Install/Remove the function from MySQL

    ```
    CREATE [AGGREGATE] FUNCTION <func> RETURNS <STRING|INT|REAL> SONAME 'udf_gsl.so';
    DROP FUNCTION <udf>;
    ```
