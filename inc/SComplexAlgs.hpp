#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <utility>

template<typename SComplex>
class CoreductionAlgorithm {

public:
  
  typedef typename SComplex::Cell Cell;

  CoreductionAlgorithm(SComplex& _s, const bool _mayReduce,  const bool _isExtractable):
	 s(_s), mayReduce(_mayReduce), isExtractable(_isExtractable) {}
  
  int operator()();

private:
  void storeGenerator(const Cell& c);
  void storeReductionPair(const Cell& c, const Cell& f) {}

  boost::optional<std::pair<Cell, Cell> > getNextPair();
  
  void addCellsToProcess(const Cell& sourceFace);
  
  SComplex& s;
  const bool mayReduce;
  const bool isExtractable;
  std::vector<Cell> collectedHomGenerators;

  // A queue for coreduction candidates
  std::deque<Cell> cellsToProcess;

};

template<typename SComplex>
class ShaveAlgorithm {

public:
  
  typedef typename SComplex::Cell Cell;

  ShaveAlgorithm(SComplex& _s): s(_s) {}
  void operator()();

private:
  SComplex& s;
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

template<typename SComplex>
inline void CoreductionAlgorithm<SComplex>::storeGenerator(const Cell& c){
  collectedHomGenerators.push_back(c);
}

template<typename SComplex>
inline boost::optional<std::pair<typename CoreductionAlgorithm<SComplex>::Cell,
											typename CoreductionAlgorithm<SComplex>::Cell> > CoreductionAlgorithm<SComplex>::getNextPair() {

  using namespace boost::lambda;
  
  while (! cellsToProcess.empty() ) {
	 // get a cell from the queue
	 Cell coface=cellsToProcess.front();
	 cellsToProcess.pop_front();
	 // the cell may have been already removed
	 if(coface.getColor() == 0) {
		// check if a free coface and store the companion
		Cell face;
		if (coface.getCofaceCompanion(face)) {
		  return std::make_pair(face, coface);
		} else {
		  addCellsToProcess(coface);
		}
	 } 
  }
  // Originally there are no candidates in the queue
  // They may also disappear when a connected component
  // is exhausted

  // If we know that a coreduction may be there,
  // For instance when treating a non-compact set
  if(mayReduce){
	 // Go through all cells and search for a coreduction pair
	 // if coface is a free coface,
	 // then the search is successful.
	 // We preserve the companion face
	 // and quit the search
	 Cell face;
	 
	 typename SComplex::ColoredIterators::Iterators::AllCells::iterator crpIt = std::find_if(s.template iterators<0>().allCells().begin(),
																														  s.template iterators<0>().allCells().end(),
																														  bind(&Cell::getCofaceCompanion, _1, var(face)));
	 if (crpIt != s.template iterators<0>().allCells().end()) {
		return std::make_pair(face, *crpIt);
	 }
  }
  
  return boost::optional<std::pair<Cell, Cell> >();
}


template<typename SComplex>
inline int CoreductionAlgorithm<SComplex>::operator()(){
  int cnt=0;

  for(;;){
	 boost::optional<std::pair<Cell, Cell> > nextPair = getNextPair();
	 
	 if (nextPair) {
		nextPair->first.template setColor<1>();
		nextPair->second.template setColor<1>();
		++cnt;++cnt;
		storeReductionPair(nextPair->second, nextPair->first);		
		addCellsToProcess(nextPair->first);
	 } else if(isExtractable) {
		// If the search failed or when we even did not try to search
		// and we know that a cell of lowest dimension is always
		// a homology generator like in the case of a vertex in
		// a compact set, we just pick up such a cell and
		// remove it from the complex
		Cell sourceFace=s.getBaseCell();
		if(sourceFace.isValid()){
		  addCellsToProcess(sourceFace);
		  sourceFace.template setColor<1>();
		  ++cnt;
		  storeGenerator(sourceFace);
		}else{
		  break; // no base face left: quit any further processing
		}
	 }else{ // no coreduction, no extraction: quit any further processing
		break; // for loop
	 }	 
  }
  
  return cnt; // the number of cells removed
}

template<typename SComplex>
inline void CoreductionAlgorithm<SComplex>::addCellsToProcess(const Cell& sourceFace) {
  // Finally, put all present cofaces of the source face
  // into the queue
  for (typename SComplex::ColoredIterators::Iterators::CbdCells::iterator cbdn = s.template iterators<0>().cbdCells(sourceFace).begin(),
			end = s.template iterators<0>().cbdCells(sourceFace).end(); cbdn != end; ++cbdn) {
	 if(cbdn->getColor() == 0) cellsToProcess.push_back(*cbdn);
  }
}

template<typename SComplex>
inline void ShaveAlgorithm<SComplex>::operator()(){
  for(int d=embeddingDim-1;d>=0;--d){
	 typedef typename SComplex::ColoredIterators::Iterators::DimCells::iterator DimIt;

	 Cell face;
	 for (DimIt it = s.template iterators<0>().dimCells(d).begin(),
			  end = s.template iterators<0>().dimCells(d).end();
			it != end; ++it) {
		if(it->getFaceCompanion(face)){
		  face.template setColor<1>();
		  it->template setColor<1>();
		}
	 }
  }
}

template<typename SComplex, typename FCComplex_P, typename SourceGenerator_P>
inline CRef<FCComplex_P> ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex, FCComplex_P, SourceGenerator_P>::operator()(){

  typedef typename FCComplex_P::GeneratorType GeneratorType;

  Stopwatch sw;
  std::set<SourceGenerator_P> cells;


  for (typename SComplex::ColoredIterators::Iterators::AllCells::iterator it = s.template iterators<0>().allCells().begin(),
			end = s.template iterators<0>().allCells().end();
		 it != end; ++it) {
	 cells.insert(it->getElementaryCell());
  }

  // -- MM std::cout << " rfccCR construction from " << cells.size() <<  " cells starting "  << std::endl;      // -- MM
  CRef<FCComplex_P> rfccCR( new FCComplex_P(cells));
  // -- MMstd::cout << rfccCR()  << std::endl;
  fcout << "Reducible chain complex (over Z) construction of CubCelSet completed in " << sw  << std::endl;
  return rfccCR;
}
