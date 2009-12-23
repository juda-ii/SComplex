#include <iostream>
using namespace std;

#include "capd/auxil/Stopwatch.h"
#include "capd/auxil/CRef.h"
#include "capd/homologicalAlgebra/embeddingDim.h"

#include "capd/vectalg/MatrixSlice.h"
#include "capd/matrixAlgorithms/intMatrixAlgorithms.hpp"

#include "capd/homologicalAlgebra/homologicalAlgebra.hpp"
#include "capd/homologicalAlgebra/homAlgFunctors.hpp"
#include "capd/homologicalAlgebra/cubSetFunctors.hpp"
#include "capd/homologicalAlgebra/ReducibleFreeChainComplex.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>

#include <CubSComplex.hpp>


BOOST_AUTO_TEST_CASE(iterators_AllCells_init_test) {
	 CubSComplex s("test/input_1.bmd");

	 BOOST_CHECK(s.allCellsIterators().begin() != s.allCellsIterators().end());
	 BOOST_CHECK(s.allCellsIterators().begin() == s.allCellsIterators().begin());
	 BOOST_CHECK(s.allCellsIterators().end() == s.allCellsIterators().end());
	 BOOST_CHECK( ((const CubSComplex&)s).allCellsIterators().begin() != ((const CubSComplex&)s).allCellsIterators().end());
	 BOOST_CHECK( ((const CubSComplex&)s).allCellsIterators().begin() == ((const CubSComplex&)s).allCellsIterators().begin());
	 BOOST_CHECK( ((const CubSComplex&)s).allCellsIterators().end() == ((const CubSComplex&)s).allCellsIterators().end());
}

BOOST_AUTO_TEST_CASE(iterators_DimCells_init_test) {
	 CubSComplex s("test/input_1.bmd");

	 BOOST_CHECK(s.dimCellsIterators(0).begin() != s.dimCellsIterators(0).end());
	 BOOST_CHECK(s.dimCellsIterators(0).begin() == s.dimCellsIterators(0).begin());
	 BOOST_CHECK(s.dimCellsIterators(0).end() == s.dimCellsIterators(0).end());
	 BOOST_CHECK( ((const CubSComplex&)s).dimCellsIterators(0).begin() != ((const CubSComplex&)s).dimCellsIterators(0).end());
	 BOOST_CHECK( ((const CubSComplex&)s).dimCellsIterators(0).begin() == ((const CubSComplex&)s).dimCellsIterators(0).begin());
	 BOOST_CHECK( ((const CubSComplex&)s).dimCellsIterators(0).end() == ((const CubSComplex&)s).dimCellsIterators(0).end());
}

BOOST_AUTO_TEST_CASE(iterators_AllCells_forEach_test) {
	 CubSComplex s("test/input_1.bmd");
	 const int initSize = s.cardinality();
	 
	 BOOST_CHECK_EQUAL(s.cardinality(), 71639264);

	 int cnt = 0;
	 for (CubSComplex::AllCellsIterators::iterator it = s.allCellsIterators().begin();
			it != s.allCellsIterators().end(); ++it) {
		if (it->present()) {
		  ++cnt;
		}
	 }
	 BOOST_CHECK_EQUAL(cnt, initSize);

	 
	 CubSComplex::AllCellsIterators allCells = s.allCellsIterators();

	 std::for_each(allCells.begin(), allCells.end(), boost::bind(&CubSComplex::Cell::remove, _1));
	 BOOST_CHECK_EQUAL(s.cardinality(), 0);

	 cnt = 0;
	 std::for_each(allCells.begin(), allCells.end(), boost::lambda::var(cnt)++);
	 BOOST_CHECK_EQUAL(cnt, initSize);	 
}
