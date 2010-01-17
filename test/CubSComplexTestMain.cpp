#define BOOST_TEST_MODULE SComplexTestSuite


#include <boost/test/unit_test.hpp>

#define BOOST_AUTO_TEST_MAIN

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/results_reporter.hpp>

struct ReportRedirector
{
    std::ofstream out;

  ReportRedirector(const char* filename) : out(filename ? filename : "test_result.xml")
    {
        assert( out.is_open() );
        boost::unit_test::results_reporter::set_stream(out);
    }
};

static ReportRedirector foo(getenv("TEST_REPORT_OUTPUT"));
