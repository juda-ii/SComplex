#define BOOST_TEST_MODULE SComplexTestSuite

#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/results_reporter.hpp>

struct LogRedirector
{
    std::ofstream out;

  LogRedirector(const char* filename)
  {
	 if (filename) {
		out.open(filename);
		assert( out.is_open() );
		boost::unit_test::unit_test_log.set_stream(out);
    }
  }
  
};

static LogRedirector logRedirector(getenv("TEST_LOG_OUTPUT"));
