#define BOOST_TEST_MODULE SComplexTestSuite

#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include "test/inc/LogRedirector.h"

static LogRedirector logRedirector(getenv("TEST_LOG_OUTPUT"));


