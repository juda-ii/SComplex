LIB_NAME=libSComplexMM.a

LOCAL_INC_PATHS= -I./inc/
INC_PATHS = $(LOCAL_INC_PATHS) -I$(HOME)/local/apps/boost-1-41-0/include/ -I$(HOME)/workspace/capd/include
LIB_PATHS = -L$(HOME)/local/apps/boost-1-41-0/lib/ -L./ -I$(HOME)/workspace/capd/lib
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(HOME)/local/apps/boost-1-41-0/lib/


LIB_SRCS = src/SComplex.cpp

LIB_OBJS = $(LIB_SRCS:.cpp=.o)



TEST_APP_NAME=SComplexTest

TEST_SRCS = test/SComplexInitTest.cpp \
	test/SComplexIteratorsTest.cpp \
	test/SComplexRemoveTest.cpp \
	test/SComplexTestMain.cpp

TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TEST_LIBS = -lSComplex -lboost_unit_test_framework
BOOST_FLAGS= -DBOOST_TEST_DYN_LINK


CC=g++


DEPEND_FLAGS=$(CFLAGS) $(LOCAL_INC_PATHS) $(BOOST_FLAGS)
WARNINGS= -Wall -Winline -Wdisabled-optimization
OPT_FLAGS=-O2
# -finline-functions -finline-limit=100000 --param large-function-growth=100000 --param inline-unit-growth=100000
PROJECT_FLAGS= $(WARNINGS) -pedantic $(OPT_FLAGS)
COMP_FLAGS=$(CFLAGS) $(PROJECT_FLAGS) $(INC_PATHS) $(BOOST_FLAGS) -g

.PHONY: depend clean

all: $(LIB_NAME)

%.o: %.cpp Makefile
	$(CC) $(COMP_FLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJS)
	rm -f $@
	ar cq $@ $(LIB_OBJS)

$(TEST_APP_NAME): $(TEST_OBJS) $(LIB_NAME)
	$(CC) -o $(TEST_APP_NAME) $(LIB_PATHS) $(TEST_OBJS) $(TEST_LIBS) 

test_exec: $(TEST_APP_NAME)
	time (LD_LIBRARY_PATH=$(HOME)/local/apps/boost-1-41-0/lib/ ./$(TEST_APP_NAME))


clean:
	$(RM) $(TEST_OBJS) $(TEST_APP_NAME) $(LIB_NAME)

depend: $(LIB_SRCS)  $(TEST_SRCS)
	makedepend  $(DEPEND_FLAGS) $^

# DO NOT DELETE

src/SComplex.o: ./inc/SComplex.h ./inc/def/SComplexDef.hpp
src/SComplex.o: ./inc/impl/SComplexImpl.hpp ./inc/def/SComplexDef.hpp
test/SComplexInitTest.o: ./inc/SComplex.h ./inc/def/SComplexDef.hpp
test/SComplexInitTest.o: ./inc/impl/SComplexImpl.hpp
test/SComplexInitTest.o: ./inc/def/SComplexDef.hpp
test/SComplexIteratorsTest.o: ./inc/SComplex.h ./inc/def/SComplexDef.hpp
test/SComplexIteratorsTest.o: ./inc/impl/SComplexImpl.hpp
test/SComplexIteratorsTest.o: ./inc/def/SComplexDef.hpp
