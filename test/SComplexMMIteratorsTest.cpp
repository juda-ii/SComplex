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
#include <algorithm>

#include <CubSComplex.hpp>


BOOST_AUTO_TEST_CASE(iterators_AllCells_init_test) {
	 CubSComplex s("test/input_1.bmd");

	 BOOST_CHECK(s.allCellsIterators().begin() != s.allCellsIterators().end());
	 BOOST_CHECK( ((const CubSComplex&)s).allCellsIterators().begin() != ((const CubSComplex&)s).allCellsIterators().end());

}

BOOST_AUTO_TEST_CASE(iterators_AllCells_forEach_test) {
	 CubSComplex s("test/input_1.bmd");

	 CubSComplex::AllCellsIterators allCells = s.allCellsIterators();
	 std::for_each(allCells.begin(), allCells.end(), boost::bind(&CubSComplex::Cell::present, _1));
}
