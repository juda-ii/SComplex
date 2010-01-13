#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <utility>

template<typename SComplexT>
class DefaultCoreductionStrategy {

public:
  typedef SComplexT SComplex;
  typedef typename SComplex::Cell Cell;
  typedef std::pair<boost::reference_wrapper<Cell>, boost::reference_wrapper<Cell> > CoreductionPair;
  typedef std::pair<boost::reference_wrapper<Cell>, boost::reference_wrapper<Cell> > ReductionPair;  


  DefaultCoreductionStrategy(SComplex& _complex): complex(_complex), dummyCell1(_complex), dummyCell2(_complex),  dummyCell3(_complex) {}
  
  SComplex& getComplex() const {
	 return complex;
  }

  Cell& getFace(const CoreductionPair& coRedPair) {
	 return boost::unwrap_ref(coRedPair.first);
  }
  
  bool reduced(const Cell& cell) const {
	 return cell.getColor() == 2;
  }

  void coreduce(const CoreductionPair& coRedPair) const {
	 boost::unwrap_ref(coRedPair.first).template setColor<2>();
	 boost::unwrap_ref(coRedPair.second).template setColor<2>();
  }

  void reduce(const ReductionPair& redPair) const {
	 boost::unwrap_ref(redPair.first).template setColor<2>();
	 boost::unwrap_ref(redPair.second).template setColor<2>();
  }

  void reduce(Cell& cell) {
	 cell.template setColor<2>();
  }
  
  boost::optional<Cell&> extract() {
	 typename SComplex::ColoredIterators::Iterators::DimCells::iterator end = complex.iterators(1).dimCells(0).end(),
		it = complex.iterators(1).dimCells(0).begin();

	 if (it != end) { 
		dummyCell1 = *it;
		return dummyCell1;
	 }
	 return boost::optional<Cell&>();	 
  }
  
  boost::optional<CoreductionPair> forceCoreductionPair() {
	 return boost::optional<CoreductionPair>();
  }

  boost::optional<CoreductionPair> getCoreductionPair(Cell& cell) {
	 if (complex.getUniqueFace(cell, dummyCell3)) {
	  	return std::make_pair(boost::ref(dummyCell3), boost::ref(cell));
	 } else {
		return boost::optional<ReductionPair>();
	 }
  }
  
  boost::optional<ReductionPair> getReductionPair(Cell& cell) {
	 if (complex.getUniqueCoFace(cell, dummyCell2)) {
	  	return std::make_pair(boost::ref(cell), boost::ref(dummyCell2));
	 } else {
		return boost::optional<ReductionPair>();
	 }
  }

private:
  SComplex& complex;
  Cell dummyCell1, dummyCell2, dummyCell3;
};

template<typename StrategyT>
class CoreductionAlgorithm {

public:
  typedef StrategyT Strategy;

  typedef typename Strategy::SComplex SComplex;
  typedef typename Strategy::Cell Cell;
  typedef typename Strategy::CoreductionPair CoreductionPair;

  
  CoreductionAlgorithm(Strategy* _strategy): strategy(_strategy), dummyCell1(_strategy->getComplex()), dummyCell2(_strategy->getComplex()) {}

  ~CoreductionAlgorithm() {
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
  Cell dummyCell1, dummyCell2;
};

class CoreductionAlgorithmFactory {

public:
  template<typename SComplex>
  static CoreductionAlgorithm<DefaultCoreductionStrategy<SComplex> > createDefault(SComplex& s) {
	 return CoreductionAlgorithm<DefaultCoreductionStrategy<SComplex> >(new DefaultCoreductionStrategy<SComplex>(s));
  }
};


template<typename StrategyT>
class ShaveAlgorithm {

public:
  typedef StrategyT Strategy;  
  typedef typename Strategy::SComplex SComplex;
  typedef typename SComplex::Cell Cell;
  typedef typename Strategy::ReductionPair ReductionPair;
  
  ShaveAlgorithm(Strategy* _strategy): strategy(_strategy) {}

  ~ShaveAlgorithm() {
	 delete strategy;
  }

  void operator()();

private:
  Strategy* strategy;
};

class ShaveAlgorithmFactory {

public:
  template<typename SComplex>
  static ShaveAlgorithm<DefaultCoreductionStrategy<SComplex> > createDefault(SComplex& s) {
	 return ShaveAlgorithm<DefaultCoreductionStrategy<SComplex> >(new DefaultCoreductionStrategy<SComplex>(s));
  }
};

template<typename SComplex, typename FCComplex_P, typename SourceGenerator_P>
class ReducibleFreeChainComplexOverZFromSComplexAlgorithm {

public:
  
  typedef typename SComplex::Cell Cell;

  ReducibleFreeChainComplexOverZFromSComplexAlgorithm(SComplex& _s): s(_s) {}
  CRef<FCComplex_P> operator()();
private:
  SComplex& s;
};

template<typename StrategyT>
inline void CoreductionAlgorithm<StrategyT>::storeGenerator(const Cell& c){
  collectedHomGenerators.push_back(c);
}


template<typename StrategyT>
inline boost::optional<typename CoreductionAlgorithm<StrategyT>::CoreductionPair> CoreductionAlgorithm<StrategyT>::getNextPair() {
  
  while (! cellsToProcess.empty() ) {
	 Cell& coface=cellsToProcess.front();
	 
	 if (! strategy->reduced(coface)) {
		boost::optional<CoreductionPair> coreductionPair = strategy->getCoreductionPair(coface);
		if (coreductionPair) {		
		  cellsToProcess.pop_front();	 
		  return coreductionPair;
		} else {
		  addCellsToProcess(coface);
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

template<typename StrategyT>
inline void ShaveAlgorithm<StrategyT>::operator()(){

  for(int d=embeddingDim-1;d>=0;--d){
	 typedef typename SComplex::ColoredIterators::Iterators::DimCells::iterator DimIt;

	 for (DimIt it = strategy->getComplex().template iterators<1>().dimCells(d).begin(),
			  end = strategy->getComplex().template iterators<1>().dimCells(d).end();
			it != end; ++it) {

		Cell& cell = *it;
		boost::optional<ReductionPair> reductionPair = strategy->getReductionPair(cell);
		if (reductionPair) {
		  strategy->reduce(*reductionPair);
		}
	 }
  }
}

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
