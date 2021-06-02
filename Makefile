CC = gcc
CFLAGS = -fPIC $(INC)
LDFLAGS = -shared

INC = $(shell mysql_config --include) $(shell gsl-config --cflags)
LIBS = $(shell gsl-config --libs)

TARGET_LIB = udf_gsl.so

SRCS = $(shell echo *.cc)
OBJS = $(SRCS:.cc=.o)

all : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

%.o : %.cc
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -o $@ -c $<

install :
	@cp $(TARGET_LIB) $(shell mysql_config --plugindir)

clean :
	@rm -rf $(TARGET_LIB) $(OBJS)
