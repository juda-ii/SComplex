#include <iostream>
using namespace std;

#include <capd/auxil/Stopwatch.h>
#include <capd/auxil/CRef.h>
#include <capd/homologicalAlgebra/embeddingDim.h>

#include <capd/vectalg/MatrixSlice.h>
#include <capd/matrixAlgorithms/intMatrixAlgorithms.hpp>

#include <capd/homologicalAlgebra/homologicalAlgebra.hpp>
#include <capd/homologicalAlgebra/homAlgFunctors.hpp>
#include <capd/homologicalAlgebra/cubSetFunctors.hpp>
#include <capd/homologicalAlgebra/ReducibleFreeChainComplex.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>

#include "CubSComplex.hpp"

BOOST_AUTO_TEST_SUITE(CubSComplex_iterators)

BOOST_AUTO_TEST_CASE(iterators_AllCells_init_test) {
	 CubSComplex s(readCubCellSet<BCubSet,BCubCelSet>("test/input_1.bmd"));

	 BOOST_CHECK(s.iterators(1).allCells().begin() != s.iterators(1).allCells().end());
	 BOOST_CHECK(s.iterators(1).allCells().begin() == s.iterators(1).allCells().begin());
	 BOOST_CHECK(s.iterators(1).allCells().end() == s.iterators(1).allCells().end());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).allCells().begin() != ((const CubSComplex&)s).iterators(1).allCells().end());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).allCells().begin() == ((const CubSComplex&)s).iterators(1).allCells().begin());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).allCells().end() == ((const CubSComplex&)s).iterators(1).allCells().end());
}

BOOST_AUTO_TEST_CASE(iterators_DimCells_init_test) {
	 CubSComplex s(readCubCellSet<BCubSet,BCubCelSet>("test/input_1.bmd"));

	 BOOST_CHECK(s.iterators(1).dimCells(0).begin() != s.iterators(1).dimCells(0).end());
	 BOOST_CHECK(s.iterators(1).dimCells(0).begin() == s.iterators(1).dimCells(0).begin());
	 BOOST_CHECK(s.iterators(1).dimCells(0).end() == s.iterators(1).dimCells(0).end());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).dimCells(0).begin() != ((const CubSComplex&)s).iterators(1).dimCells(0).end());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).dimCells(0).begin() == ((const CubSComplex&)s).iterators(1).dimCells(0).begin());
	 BOOST_CHECK( ((const CubSComplex&)s).iterators(1).dimCells(0).end() == ((const CubSComplex&)s).iterators(1).dimCells(0).end());
}

BOOST_AUTO_TEST_CASE(iterators_AllCells_forEach_test) {
	 CubSComplex s(readCubCellSet<BCubSet,BCubCelSet>("test/input_1.bmd"));
	 const int initSize = s.cardinality();
	 
	 BOOST_CHECK_EQUAL(s.cardinality(), 71639264);

	 int cnt = 0;
	 for (CubSComplex::Iterators::AllCells::iterator it = s.iterators(1).allCells().begin();
			it != s.iterators(1).allCells().end(); ++it) {
		if (it->getColor() == 1) {
		  ++cnt;
		}
	 }
	 BOOST_CHECK_EQUAL(cnt, initSize);

	 
	 
	 
	 std::for_each(s.iterators(1).allCells().begin(), s.iterators(1).allCells().end(), boost::bind(&CubSComplex::Cell::setColor, _1, 2));
	 BOOST_CHECK_EQUAL(s.cardinality(), 0);

	 cnt = 0;
	 std::for_each(s.iterators(1).allCells().begin(), s.iterators(1).allCells().end(), boost::lambda::var(cnt)++);
	 //	 BOOST_CHECK_EQUAL(cnt, initSize);	 
}

BOOST_AUTO_TEST_SUITE_END()
