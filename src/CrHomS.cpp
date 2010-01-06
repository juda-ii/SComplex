/****  CRHomS.cpp            ****/
/**** (c) Marian Mrozek 2009 ****/

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

#include <CubSComplex.hpp>
#include <SComplexAlgs.hpp>

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;


template<typename SComplex>
void CrHomS(int argc,char* argv[]){
  Stopwatch swTot;
  string fileName=argc>1 ? string(argv[1]) : "sphere1d.txt";
  cout << " --- Reading cubical cellular set from  " << fileName  << endl;

  CRef<SComplex> SComplexCR(new SComplex(readCubCellSet<BCubSet,BCubCelSet>(fileName.c_str())));
  cout << "Successfully read  " << fileName <<
          " of "  << SComplexCR().cardinality() << " cells " << endl;

//  SComplexAlgs<CubSComplex>::test(SComplexCR());


  Stopwatch swComp,swRed;
  (ShaveAlgorithm<CubSComplex>(SComplexCR()))();  
  cout << " --- Shave reduced the size to " << SComplexCR().cardinality() << " in " << swRed <<  endl;
  
  Stopwatch swCoRed;
  (CoreductionAlgorithm<CubSComplex>(SComplexCR(), false, SComplexCR().getBaseDimension() == 0))();
  cout << " --- Coreduction reduced the size to " << SComplexCR().cardinality() << " in " << swCoRed <<  endl;

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
		(ReducibleFreeChainComplexOverZFromSComplexAlgorithm<CubSComplex, ReducibleFreeChainComplexType,ElementaryCellType>(SComplexCR()))();
  cout << " --- RFCC constructed  " << endl;

  CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
  cout << " --- Computation completed in " << swComp  << std::endl;
  cout << " --- Computed homology is: \n\n" << homSignCR()  << std::endl;

  cout << " --- Total computation time is: " << swTot  << std::endl;

}
ofstreamcout fcout;

int main(int argc,char* argv[]){
  std::string outFname="out.txt";
  fcout.turnOn();
  fcout.open(outFname.c_str());

  try{
    CrHomS<CubSComplex>(argc,argv);
  }catch(std::exception& e){
    std::cout << "Caught exception: " << e.what() << endl;
  }catch(std::string& s){
    std::cout << "Caught exception: " << s.c_str() << endl;
  }catch(const char* c){
    std::cout << "Caught exception: " << c << endl;
  }catch(...){
    std::cout << "Caught an unknown exception: " << endl;
  }
}

