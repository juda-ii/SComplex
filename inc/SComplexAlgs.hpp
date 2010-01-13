#ifndef SCOMPLEX_ALGS_HPP_
#define SCOMPLEX_ALGS_HPP_


#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <utility>

#include "SComplexAlgs_Coreduction.hpp"
#include "SComplexAlgs_Shave.hpp"


template<typename SComplex, typename FCComplex_P, typename SourceGenerator_P>
class ReducibleFreeChainComplexOverZFromSComplexAlgorithm {

public:
  
  typedef typename SComplex::Cell Cell;

  ReducibleFreeChainComplexOverZFromSComplexAlgorithm(SComplex& _s): s(_s) {}
  CRef<FCComplex_P> operator()();
private:
  SComplex& s;
};


template<typename SComplex, typename FCComplex_P, typename SourceGenerator_P>
inline CRef<FCComplex_P> ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex, FCComplex_P, SourceGenerator_P>::operator()(){

  typedef typename FCComplex_P::GeneratorType GeneratorType;

  Stopwatch sw;
  std::set<SourceGenerator_P> cells;


  for (typename SComplex::ColoredIterators::Iterators::AllCells::iterator it = s.template iterators<1>().allCells().begin(),
			end = s.template iterators<1>().allCells().end();
		 it != end; ++it) {
	 cells.insert(it->getElementaryCell());
  }

  // -- MM std::cout << " rfccCR construction from " << cells.size() <<  " cells starting "  << std::endl;      // -- MM
  CRef<FCComplex_P> rfccCR( new FCComplex_P(cells));
  // -- MMstd::cout << rfccCR()  << std::endl;
  fcout << "Reducible chain complex (over Z) construction of CubCelSet completed in " << sw  << std::endl;
  return rfccCR;
}


#endif // SCOMPLEX_ALGS_HPP_
