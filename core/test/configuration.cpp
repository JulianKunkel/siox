#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#include <iostream>

#include "Configuration.hpp"
#include "SioxLogger.hpp"

Logger *logger;



BOOST_AUTO_TEST_CASE(configuration)
{
	std::string yamlfile(boost::unit_test::framework::master_test_suite().argv[1]);
	Configuration conf(yamlfile);
	
	BOOST_CHECK_EQUAL(0,0);
}

