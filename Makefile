APP_NAME=SComplexMM

LOCAL_INC_PATHS= -I./inc/  
INC_PATHS = $(LOCAL_INC_PATHS) -I$(HOME)/local/apps/boost-1-41-0/include/ -I$(HOME)/workspace/capd/include
LIB_PATHS = -L$(HOME)/local/apps/boost-1-41-0/lib/ -L./ -L$(HOME)/workspace/capd/lib
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(HOME)/local/apps/boost-1-41-0/lib/


APP_SRCS = src/CrHomS.cpp
APP_OBJS = $(APP_SRCS:.cpp=.o)
APP_LIBS = -lcapd


TEST_APP_NAME=SComplexMMTest

TEST_SRCS = test/SComplexMMReductionTest.cpp \
	test/SComplexMMIteratorsTest.cpp \
	test/SComplexTestMain.cpp

TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TEST_LIBS = -lcapd -lboost_unit_test_framework
BOOST_FLAGS= -DBOOST_TEST_DYN_LINK


CC=g++


DEPEND_FLAGS=$(CFLAGS) $(LOCAL_INC_PATHS) $(BOOST_FLAGS)
WARNINGS= -Wall -Winline -Wdisabled-optimization
OPT_FLAGS=-O2 
#-finline-functions -finline-limit=100000 --param large-function-growth=100000 --param inline-unit-growth=100000
PROJECT_FLAGS= $(WARNINGS) -pedantic $(OPT_FLAGS)
COMP_FLAGS=$(CFLAGS) $(PROJECT_FLAGS) $(INC_PATHS) $(BOOST_FLAGS)

.PHONY: depend clean

all: $(APP_NAME)

%.o: %.cpp Makefile
	$(CC) $(COMP_FLAGS) -c $< -o $@

$(APP_NAME): $(APP_OBJS)
	$(CC) -o $(APP_NAME) $(APP_OBJS) $(LIB_PATHS) $(APP_LIBS)

$(TEST_APP_NAME): $(TEST_OBJS)
	$(CC) -o $(TEST_APP_NAME) $(LIB_PATHS) $(TEST_OBJS) $(TEST_LIBS) 

test_exec: $(TEST_APP_NAME)
	time (LD_LIBRARY_PATH=$(HOME)/local/apps/boost-1-41-0/lib/ ./$(TEST_APP_NAME))


clean:
	$(RM) $(TEST_OBJS) $(TEST_APP_NAME) $(APP_NAME)

depend: $(APP_SRCS) $(LIB_SRCS)  $(TEST_SRCS)
	makedepend  $(DEPEND_FLAGS) $^

# DO NOT DELETE

src/CrHomS.o: ./inc/CubSComplex.hpp ./inc/SComplexAlgs.hpp
test/SComplexMMReductionTest.o: ./inc/CubSComplex.hpp ./inc/SComplexAlgs.hpp
test/SComplexMMIteratorsTest.o: ./inc/CubSComplex.hpp
