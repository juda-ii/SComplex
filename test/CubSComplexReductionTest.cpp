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

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/test/unit_test.hpp>

#include <CubSComplex.hpp>
#include <SComplexAlgs.hpp>

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;


template<typename SComplex>
boost::tuple<int, int, int, std::string>  CrHomS(const std::string &fileName) {
	 boost::tuple<int, int, int, std::string> result;
	 using boost::tuples::get;
	 
  Stopwatch swTot;
  cout << " --- Reading cubical cellular set from  " << fileName  << endl;

  CRef<SComplex> SComplexCR(new SComplex(readCubCellSet<BCubSet,BCubCelSet>(fileName.c_str())));
  cout << "Successfully read  " << fileName <<
          " of "  << SComplexCR().cardinality() << " cells " << endl;

  get<0>(result) = SComplexCR().cardinality();
//  SComplexAlgs<CubSComplex>::test(SComplexCR());


  Stopwatch swComp,swRed;

  (ShaveAlgorithmFactory::createDefault(SComplexCR()))();  
  cout << " --- Shave reduced the size to " << SComplexCR().cardinality() << " in " << swRed <<  endl;
  get<1>(result) = SComplexCR().cardinality();
  
  Stopwatch swCoRed;

  (CoreductionAlgorithmFactory::createDefault(SComplexCR()))();
  cout << " --- Coreduction reduced the size to " << SComplexCR().cardinality() << " in " << swCoRed <<  endl;
  get<2>(result) = SComplexCR().cardinality();
  
  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<CubSComplex, ReducibleFreeChainComplexType,ElementaryCellType>(SComplexCR()))();
  cout << " --- RFCC constructed  " << endl;

  CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
  cout << " --- Computation completed in " << swComp  << std::endl;
  cout << " --- Computed homology is: \n\n" << homSignCR()  << std::endl;
  get<3>(result) = homSignCR().bettiVector();
  
  cout << " --- Total computation time is: " << swTot  << std::endl;

  return result;
}


BOOST_AUTO_TEST_CASE(reduction_test) {
  BOOST_CHECK_EQUAL(CrHomS<CubSComplex>("test/input_1.bmd"), boost::make_tuple(71639264, 817510, 3815, "0,1057"));
}
