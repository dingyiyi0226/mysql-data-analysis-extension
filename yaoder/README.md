# MySQL UDF Implementations

## Usage

### Compile 

1. Compile the source file via `make`
```
make
```
2. Install the library to MySQL
```
sudo make install
```

### MySQL

1. Install/Uninstall functions from MySQL
```
mysql -u root -p < import.sql
mysql -u root -p < drop.sql
```

2. Launch MySQL server
```
mysql -u root -p
```
3. Dependencies

User may have different mysql library and plugins dependencies, check [Makefile](/yaoder/Makefile)

My dependencies
```
MY_INCL = -I/usr/local/mysql/include
MY_PLUGINS = /usr/local/mysql/lib/plugin
```
