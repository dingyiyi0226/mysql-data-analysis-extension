CC = gcc
CFLAGS = -fPIC $(INC)
LDFLAGS = -shared

INC = $(shell mysql_config --include) $(shell gsl-config --cflags)
LIBS = $(shell gsl-config --libs)
PLUGIN_DIR = $(shell mysql_config --plugindir)

TARGET_LIB = udf_gsl.so

SRCS = $(shell echo stats/*.cc) $(shell echo fit/*.cc)
OBJS = $(SRCS:.cc=.o)

all : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

%.o : %.cc
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -o $@ -c $<

install :
	@cp $(TARGET_LIB) $(PLUGIN_DIR)

clean :
	@rm -f $(TARGET_LIB) $(OBJS)
