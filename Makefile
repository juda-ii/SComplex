BUILD_DIR=build


XXX=xxx

BINS_DIR=$(BUILD_DIR)/bin
LIBS_DIR=$(BUILD_DIR)/lib
OBJS_DIR=$(BUILD_DIR)/obj
RUN_DIR=$(BUILD_DIR)/run

INCS_DIR=inc
SRCS_DIR=src
TEST_DIR=test
UNIT_TEST_DIR=$(TEST_DIR)/unit
PERFORMANCE_TEST_DIR=$(TEST_DIR)/performance

APP_NAME=CrHomS
LIB_NAME=libSComplex.a

CCCC_DIR=$(RUN_DIR)/cccc
CPPCHECK_OUTPUT=$(RUN_DIR)/cppcheck.xml
UNIT_TEST_LCOV_OUTPUT=$(LCOV_DIR)/test.info

LCOV_DIR=$(RUN_DIR)/lcov

TEST_OUTPUT_FORMAT=HRF
TEST_LOG_LEVEL=message
TEST_REPORT_LEVEL=short
TEST_APP_ARGS=--output_format=$(TEST_OUTPUT_FORMAT) --log_level=$(TEST_LOG_LEVEL) --report_level=$(TEST_REPORT_LEVEL)

BOOST_HOME=/home/juda/local/apps/boost-1-41-0
CAPD_HOME=/home/juda/workspace/capd

LOCAL_INC_PATHS= -I$(INCS_DIR) -I.
INC_PATHS = $(LOCAL_INC_PATHS) -isystem$(CAPD_HOME)/include -isystem$(BOOST_HOME)/include
LIB_PATHS = -L$(LIBS_DIR) -L$(CAPD_HOME)/lib -L$(BOOST_HOME)/lib


LIB_SRCS = $(SRCS_DIR)/CubSComplex_Cell.cpp
APP_SRCS = $(SRCS_DIR)/CrHomS.cpp
H_SRCS= $(shell find $(INCS_DIR) -name '*.hpp' -o -name '*.h')

LIB_OBJS = $(LIB_SRCS:%.cpp=$(OBJS_DIR)/%.o)
APP_OBJS = $(APP_SRCS:%.cpp=$(OBJS_DIR)/%.o)

APP_LIBS = -lcapd -lSComplex


UNIT_TEST_APP_NAME=SComplexUnitTest
UNIT_TEST_SRCS = \
	$(UNIT_TEST_DIR)/SComplexUnitTestMain.cpp \
	$(UNIT_TEST_DIR)/CubSComplexIteratorsTest.cpp \
	$(UNIT_TEST_DIR)/CubSComplexReductionTest.cpp

UNIT_TEST_OBJS = $(UNIT_TEST_SRCS:%.cpp=$(OBJS_DIR)/%.o)
UNIT_TEST_LIBS = $(APP_LIBS)  -lboost_unit_test_framework


PERFORMANCE_TEST_APP_NAME=SComplexPerformanceTest
PERFORMANCE_TEST_SRCS = \
	$(PERFORMANCE_TEST_DIR)/SComplexPerformanceTestMain.cpp \
	$(PERFORMANCE_TEST_DIR)/CubSComplexCoreductionPerformance.cpp

PERFORMANCE_TEST_OBJS = $(PERFORMANCE_TEST_SRCS:%.cpp=$(OBJS_DIR)/%.o)
PERFORMANCE_TEST_LIBS = $(APP_LIBS)  -lboost_unit_test_framework

ALL_SRCS = $(LIB_SRCS) $(APP_SRCS) $(UNIT_TEST_SRCS) $(PERFORMANCE_TEST_SRCS) $(H_SRCS)
ALL_OBJS = $(LIB_OBJS) $(APP_OBJS) $(UNIT_TEST_OBJS) $(PERFORMANCE_TEST_OBJS)


MKDIR=mkdir -p
CC=g++

DEBUG=-g
#DEBUG=-DBOOST_DISABLE_ASSERTS
BOOST_FLAGS= -DBOOST_TEST_DYN_LINK
WARNINGS= -Wall
# -Winline -Wdisabled-optimization
#OPT_FLAGS=-O2 -finline-functions -finline-limit=100000 --param large-function-growth=100000 --param inline-unit-growth=100000
OPT_FLAGS=-O2
SOURCE_STANDARD=-pedantic
COMP_FLAGS= $(DEBUG) $(WARNINGS) $(SOURCE_STANDARD) $(OPT_FLAGS) $(INC_PATHS) $(BOOST_FLAGS)


coverage: OPT_FLAGS=
coverage: DEBUG:=$(DEBUG) -fprofile-arcs -ftest-coverage

.PHONY: init depend clean clean-obj libs apps all compile

$(BUILD_DIR)/init: 
	$(MKDIR) $(BINS_DIR)
	$(MKDIR) $(LIBS_DIR)
	$(MKDIR) $(OBJS_DIR)
	$(MKDIR) $(OBJS_DIR)/$(SRCS_DIR)
	$(MKDIR) $(OBJS_DIR)/$(UNIT_TEST_DIR)
	$(MKDIR) $(OBJS_DIR)/$(PERFORMANCE_TEST_DIR)
	$(MKDIR) $(RUN_DIR)
	$(MKDIR) $(CCCC_DIR)
	$(MKDIR) $(LCOV_DIR)
	touch $(BUILD_DIR)/init

init: $(BUILD_DIR)/init

all: init libs apps test

libs: init $(LIBS_DIR)/$(LIB_NAME)

apps: init $(BINS_DIR)/$(APP_NAME)

compile: $(BUILD_DIR)/init $(ALL_OBJS)

test: init $(BINS_DIR)/$(UNIT_TEST_APP_NAME)
	@time (LD_LIBRARY_PATH=$(BOOST_HOME)/lib $(PWD)/$(BINS_DIR)/$(UNIT_TEST_APP_NAME) $(TEST_APP_ARGS))

performance_test: init $(BINS_DIR)/$(PERFORMANCE_TEST_APP_NAME)
	@time (LD_LIBRARY_PATH=$(BOOST_HOME)/lib $(BINS_DIR)/$(PERFORMANCE_TEST_APP_NAME) $(TEST_APP_ARGS))

cccc: init $(ALL_SRCS)
	@cccc --outdir=$(CCCC_DIR) $(ALL_SRCS)

cppcheck: $(ALL_SRCS)
	@cppcheck -v --enable=all $(LOCAL_INC_PATHS) --xml $(ALL_SRCS) 2> $(CPPCHECK_OUTPUT)

coverage: init $(BINS_DIR)/$(UNIT_TEST_APP_NAME)
	@echo "Init info"
	@lcov -o $(UNIT_TEST_LCOV_OUTPUT) -b ./ -c -i -d $(OBJS_DIR) > /dev/null
	@echo "Run tests"
	@LD_LIBRARY_PATH=$(BOOST_HOME)/lib $(BINS_DIR)/$(UNIT_TEST_APP_NAME) > /dev/null
	@lcov -o $(UNIT_TEST_LCOV_OUTPUT) -d $(OBJS_DIR) -c -b ./ > /dev/null
	@lcov -o $(UNIT_TEST_LCOV_OUTPUT).tmp -d $(OBJS_DIR) -e $(UNIT_TEST_LCOV_OUTPUT) $(addprefix $(PWD)/, $(ALL_SRCS)) > /dev/null
	@echo "Generate report"
	@genhtml -o $(LCOV_DIR) $(UNIT_TEST_LCOV_OUTPUT).tmp > /dev/null


$(OBJS_DIR)/%.o: %.cpp $(BUILD_DIR)/init
	@echo $(CC) $<
	@$(CC) $(COMP_FLAGS) -c $< -o $@


$(LIBS_DIR)/$(LIB_NAME): $(LIB_OBJS)
	@echo $(AR) $(LIB_NAME)
	@$(AR) rcs $(LIBS_DIR)/$(LIB_NAME) $(LIB_OBJS) 

$(BINS_DIR)/$(APP_NAME): init libs $(APP_OBJS)
	@echo $(APP_NAME)
	@$(CC) $(COMP_FLAGS) -o $(APP_NAME) $(APP_OBJS) $(LIB_PATHS) $(APP_LIBS)

$(BINS_DIR)/$(UNIT_TEST_APP_NAME): init libs $(UNIT_TEST_OBJS)
	@echo $(UNIT_TEST_APP_NAME)
	@$(CC) $(COMP_FLAGS) -o $(BINS_DIR)/$(UNIT_TEST_APP_NAME) $(LIB_PATHS) $(UNIT_TEST_OBJS) $(UNIT_TEST_LIBS) 

$(BINS_DIR)/$(PERFORMANCE_TEST_APP_NAME): init libs $(PERFORMANCE_TEST_OBJS)
	@echo $(PERFORMANCE_TEST_APP_NAME)
	@$(CC) $(COMP_FLAGS) -o $(BINS_DIR)/$(PERFORMANCE_TEST_APP_NAME) $(LIB_PATHS) $(PERFORMANCE_TEST_OBJS) $(PERFORMANCE_TEST_LIBS)

clean_obj:
	$(RM) $(ALL_OBJS)

clean: clean_obj
	$(RM) -r $(BUILD_DIR)/*

depend: $(APP_SRCS) $(LIB_SRCS)  $(UNIT_TEST_SRCS) $(PERFORMANCE_TEST_SRCS)
	makedepend  -p$(OBJS_DIR)/ $(LOCAL_INC_PATHS) -f Makefile.dep $^

include Makefile.dep

