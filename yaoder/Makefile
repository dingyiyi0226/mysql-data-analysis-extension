CC = g++
CFLAGS = -fPIC $(INCL)
LDFLAGS = -shared

# my local include files & mysql plugins
MY_INCL = -I/usr/local/mysql/include
MY_PLUGINS = /usr/local/mysql/lib/plugin

INCL = $(shell mysql_config --include) $(shell gsl-config --cflags) $(MY_INCL)
LIBS = $(shell gsl-config --libs)

TARGET_LIB = udf_gsl.so

SRCS = $(shell echo stats/*.cc)
OBJS = $(SRCS:.cc=.o)

all : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

%.o : %.cc
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -o $@ -c $<

install :
	@cp $(TARGET_LIB) $(shell mysql_config --plugindir) $(MY_PLUGINS)

clean :
	@rm -rf $(TARGET_LIB) $(OBJS)