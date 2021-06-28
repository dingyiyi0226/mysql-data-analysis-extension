# MySQL Data Analysis Extension
An data analysis extension for MySQL server using [MySQL loadable function](https://dev.mysql.com/doc/extending-mysql/8.0/en/adding-loadable-function.html)

## Prerequisite
-   [MySQL community server](https://dev.mysql.com/downloads/mysql/) >= 8.0.0
-   [GSL](https://www.gnu.org/software/gsl/) >= 2.5

Make sure you have the `mysql_config` command. You can install it via `yum install mysql-devel`(on CentOS)

Otherwise, you have to modify the `INC` and `PLUGIN_DIR` variable in [Makefile](Makefile)

```
INC = -I/usr/local/mysql/include
PLUGIN_DIR = /usr/local/mysql/lib/plugin
```

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

1.  Install/Uninstall functions from MySQL
    
    ```
    mysql -u root -p < install.sql
    mysql -u root -p < uninstall.sql
    ```

2.  Launch MySQL server with `--binary-as-hex=0` flag. ([reference](https://bugs.mysql.com/bug.php?id=99480))

    ```
    mysql -u root -p --binary-as-hex=0
    ```
