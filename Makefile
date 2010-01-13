APP_NAME=CrHomS
LIB_NAME=libSComplex.a


BOOST_HOME=/home/juda/local/apps/boost-1-41-0
CAPD_HOME=/home/juda/workspace/capd

LOCAL_INC_PATHS= -I./inc/  
INC_PATHS = $(LOCAL_INC_PATHS) -I$(CAPD_HOME)/include -I$(BOOST_HOME)/include
LIB_PATHS = -L$(BOOST_HOME)/lib -L./ -L$(CAPD_HOME)/lib


LIB_SRCS = src/CubSComplex_Cell.cpp
APP_SRCS = src/CrHomS.cpp

LIB_OBJS = $(LIB_SRCS:.cpp=.o)
APP_OBJS = $(APP_SRCS:.cpp=.o)

APP_LIBS = -lcapd -lSComplex


TEST_APP_NAME=CubSComplexTest
TEST_SRCS = test/CubSComplexReductionTest.cpp \
	test/CubSComplexTestMain.cpp
#	test/CubSComplexIteratorsTest.cpp \

TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TEST_LIBS = $(APP_LIBS) -lboost_unit_test_framework




CC=g++

DEBUG_ON= -g
DEBUG_OFF=-DBOOST_DISABLE_ASSERTS
DEBUG= $(DEBUG_ON) 
BOOST_FLAGS= -DBOOST_TEST_DYN_LINK
DEPEND_FLAGS=$(CFLAGS) $(LOCAL_INC_PATHS) $(BOOST_FLAGS)
WARNINGS= -Wall
# -Winline -Wdisabled-optimization
OPT_FLAGS=-O2 -finline-functions -finline-limit=100000 --param large-function-growth=100000 --param inline-unit-growth=100000
#OPT_FLAGS=-O2
PROJECT_FLAGS= $(WARNINGS) -pedantic $(OPT_FLAGS)
COMP_FLAGS=$(CFLAGS) $(DEBUG) $(PROJECT_FLAGS) $(INC_PATHS) $(BOOST_FLAGS) 



.PHONY: depend clean

all: $(APP_NAME)

%.o: %.cpp Makefile
	$(CC) $(COMP_FLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJS)
	$(AR) rcs $(LIB_NAME) $(LIB_OBJS) 

$(APP_NAME): $(APP_OBJS) $(LIB_NAME) 
	$(CC) $(COMP_FLAGS) -o $(APP_NAME) $(APP_OBJS) $(LIB_PATHS) $(APP_LIBS)


$(TEST_APP_NAME): $(TEST_OBJS)
	$(CC) -o $(TEST_APP_NAME) $(LIB_PATHS) $(TEST_OBJS) $(TEST_LIBS) 

test_exec: $(TEST_APP_NAME)
	time (LD_LIBRARY_PATH=$(BOOST_HOME)/lib ./$(TEST_APP_NAME))


clean:
	$(RM) $(TEST_OBJS) $(TEST_APP_NAME) $(APP_NAME) $(APP_OBJS) $(LIB_NAME)

depend: $(APP_SRCS) $(LIB_SRCS)  $(TEST_SRCS)
	makedepend  $(DEPEND_FLAGS) $^

#Bellow are dependencies generated by 'dpend' target (make depend)
# DO NOT DELETE

src/CrHomS.o: ./inc/CubSComplex.hpp ./inc/CubSComplex_Cell.hpp
src/CrHomS.o: ./inc/CubSComplex_IteratorProvider.hpp
src/CrHomS.o: ./inc/CubSComplex_Iterators.hpp
src/CrHomS.o: ./inc/CubSComplex_ColoredIterators.hpp
src/CrHomS.o: ./inc/CubSComplex_Numerators.hpp ./inc/SComplexAlgs.hpp
src/CrHomS.o: ./inc/SComplexAlgs_Coreduction.hpp
src/CrHomS.o: ./inc/SComplexAlgs_DefaultReduceStrategy.hpp
src/CrHomS.o: ./inc/SComplexAlgs_Shave.hpp
src/CubSComplex_Cell.o: ./inc/CubSComplex.hpp ./inc/CubSComplex_Cell.hpp
src/CubSComplex_Cell.o: ./inc/CubSComplex_IteratorProvider.hpp
src/CubSComplex_Cell.o: ./inc/CubSComplex_Iterators.hpp
src/CubSComplex_Cell.o: ./inc/CubSComplex_ColoredIterators.hpp
src/CubSComplex_Cell.o: ./inc/CubSComplex_Numerators.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex_Cell.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex_IteratorProvider.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex_Iterators.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex_ColoredIterators.hpp
test/CubSComplexReductionTest.o: ./inc/CubSComplex_Numerators.hpp
test/CubSComplexReductionTest.o: ./inc/SComplexAlgs.hpp
test/CubSComplexReductionTest.o: ./inc/SComplexAlgs_Coreduction.hpp
test/CubSComplexReductionTest.o: ./inc/SComplexAlgs_DefaultReduceStrategy.hpp
test/CubSComplexReductionTest.o: ./inc/SComplexAlgs_Shave.hpp
