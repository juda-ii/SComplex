#ifndef _SCOMPLEX_ALGS_COREDUCTION_HPP
#define _SCOMPLEX_ALGS_COREDUCTION_HPP

#include "SComplexAlgs_DefaultReduceStrategy.hpp"

template<typename StrategyT>
class CoreductionAlgorithm {

public:
  typedef StrategyT Strategy;

  typedef typename Strategy::SComplex SComplex;
  typedef typename Strategy::Cell Cell;
  typedef typename Strategy::CoreductionPair CoreductionPair;

  
  CoreductionAlgorithm(Strategy* _strategy): strategy(_strategy), dummyCell1(_strategy->getComplex()) {}

  ~CoreductionAlgorithm() {
	 if (strategy)
		delete strategy;
  }
  
  int operator()();

private:
  void storeGenerator(const Cell& c);
  void storeCoreductionPair(const CoreductionPair& ) {}
  
  boost::optional<CoreductionPair> getNextPair();  
  void addCellsToProcess(const Cell& sourceFace);


  Strategy* strategy;
  
  std::vector<Cell> collectedHomGenerators;
  std::deque<Cell> cellsToProcess;
  Cell dummyCell1;
};

class CoreductionAlgorithmFactory {

public:
  template<typename SComplex>
  static CoreductionAlgorithm<DefaultReduceStrategy<SComplex> > createDefault(SComplex& s) {
	 return CoreductionAlgorithm<DefaultReduceStrategy<SComplex> >(new DefaultReduceStrategy<SComplex>(s));
  }
};



template<typename StrategyT>
inline void CoreductionAlgorithm<StrategyT>::storeGenerator(const Cell& c){
  collectedHomGenerators.push_back(c);
}


template<typename StrategyT>
inline boost::optional<typename CoreductionAlgorithm<StrategyT>::CoreductionPair> CoreductionAlgorithm<StrategyT>::getNextPair() {
  
  while (! cellsToProcess.empty() ) {
	 Cell& cell = cellsToProcess.front();
	 
	 if (! strategy->reduced(cell)) {
		dummyCell1 = cell;
		boost::optional<CoreductionPair> coreductionPair = strategy->getCoreductionPair(dummyCell1);
		if (coreductionPair) {		
		  cellsToProcess.pop_front();	 
		  return coreductionPair;
		} else {
		  addCellsToProcess(cell);
		}
	 }
	 cellsToProcess.pop_front();	 
  }

  // Originally there are no candidates in the queue
  // They may also disappear when a connected component
  // is exhausted
  // If we know that a coreduction may be there,
  // For instance when treating a non-compact set
  return strategy->forceCoreductionPair();
}


template<typename StrategyT>
inline int CoreductionAlgorithm<StrategyT>::operator()(){
  int cnt=0;

  for(;;){
	 boost::optional<CoreductionPair> nextPair = getNextPair();
	 
	 if (nextPair) {
		storeCoreductionPair(*nextPair);
		addCellsToProcess(strategy->getFace(*nextPair));
		
		strategy->coreduce(*nextPair);		
		++cnt;++cnt;
	 } else {
		// If the search failed or when we even did not try to search
		// and we know that a cell of lowest dimension is always
		// a homology generator like in the case of a vertex in
		// a compact set, we just pick up such a cell and
		// remove it from the complex
		boost::optional<Cell&> sourceFace = strategy->extract();

		if(sourceFace){
		  storeGenerator(*sourceFace);		  
		  addCellsToProcess(*sourceFace);
		  
		  strategy->reduce(*sourceFace);
		  ++cnt;
		}else{
		  break; // no base face left: quit any further processing
		}
	 } 
  }
  
  return cnt; // the number of cells removed
}

template<typename StrategyT>
inline void CoreductionAlgorithm<StrategyT>::addCellsToProcess(const Cell& sourceFace) {
  // Finally, put all present cofaces of the source face
  // into the queue
  for (typename SComplex::ColoredIterators::Iterators::CbdCells::iterator cbdn = strategy->getComplex().template iterators<1>().cbdCells(sourceFace).begin(),
			end = strategy->getComplex().template iterators<1>().cbdCells(sourceFace).end(); cbdn != end; ++cbdn) {
	 if (! strategy->reduced(*cbdn)) {
		cellsToProcess.push_back(*cbdn);
	 }
  }
}

#endif
