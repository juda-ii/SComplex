#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <utility>

template<typename SComplex>
class CoreductionAlgorithm {

public:
  
  typedef typename SComplex::Cell Cell;
  typedef boost::function< boost::optional<Cell> () > Extractor;
  typedef boost::function< boost::optional<std::pair<Cell, Cell> > () > ReductionPairProvider;
  
  CoreductionAlgorithm(SComplex& _s, const ReductionPairProvider _reductionPairProvider,  const Extractor& _extractor):
	 s(_s), reductionPairProvider(_reductionPairProvider), extractor(_extractor) {}
  
  int operator()();

private:
  void storeGenerator(const Cell& c);
  void storeReductionPair(const Cell& c, const Cell& f) {}
  boost::optional<std::pair<Cell, Cell> > getNextPair();  
  void addCellsToProcess(const Cell& sourceFace);
  void getUniqueFace(const Cell& cell, boost::optional<Cell>& face);
  
  SComplex& s;
  const ReductionPairProvider reductionPairProvider;
  const Extractor extractor;
  
  std::vector<Cell> collectedHomGenerators;

  // A queue for coreduction candidates
  std::deque<Cell> cellsToProcess;

};

class CoreductionAlgorithmFactory {

  template<typename SComplex>
  static boost::optional<typename SComplex::Cell> baseDimExtractor(const SComplex& s) {

	 typename SComplex::ColoredConstIterators::Iterators::DimCells::iterator it = s.iterators(1).dimCells(0).begin(),
		end = s.iterators(1).dimCells(0).end();
	 
	 if (it != end) {
		return *it;
	 }
	 
	 return boost::optional<typename SComplex::Cell>();	 
  }

  template<typename SComplex>
  static boost::optional<std::pair<typename SComplex::Cell, typename SComplex::Cell> >
  reductionPairProvider(const SComplex& s) {
	 // Go through all cells and search for a coreduction pair
	 // if coface is a free coface,
	 // then the search is successful.
	 // We preserve the companion face
	 // and quit the search
	 return boost::optional<std::pair<typename SComplex::Cell, typename SComplex::Cell> >();	 
	 for (typename SComplex::ColoredConstIterators::template Color<1>::Iterators::AllCells::iterator it = s.template iterators<1>().allCells().begin(),
			  end = s.template iterators<1>().allCells().end(); it != end; ++it) {

		boost::optional<typename SComplex::Cell> face = s.getUniqueFace(*it);
		if (face) {
		  return std::make_pair(*face, *it);
		}
	 }
  }

  
public:
  template<typename SComplex>
  static CoreductionAlgorithm<SComplex> createDefault(SComplex& s) {
	 return CoreductionAlgorithm<SComplex>(s, boost::bind(reductionPairProvider<SComplex>, s),
														boost::bind(baseDimExtractor<SComplex>, s));
  }
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
inline void CoreductionAlgorithm<SComplex>::getUniqueFace(const Cell& cell, boost::optional<Cell>& face) {
  typename SComplex::ColoredIterators::Iterators::CbdCells::iterator cbd = s.template iterators<1>().cbdCells(cell).begin(),
	 end = s.template iterators<1>().cbdCells(cell).end();

  if (cbd != end) {
	 typename SComplex::ColoredIterators::Iterators::CbdCells::iterator it = cbd;
	 ++cbd;
	 if (cbd == end) {
		face = *it;
	 }
  }

}

template<typename SComplex>
inline boost::optional<std::pair<typename CoreductionAlgorithm<SComplex>::Cell,
											typename CoreductionAlgorithm<SComplex>::Cell> > CoreductionAlgorithm<SComplex>::getNextPair() {
  
  while (! cellsToProcess.empty() ) {
	 // get a cell from the queue
	 Cell coface=cellsToProcess.front();
	 cellsToProcess.pop_front();
	 // the cell may have been already removed
	 if(coface.getColor() == 1) {
		// check if a free coface and store the companion
		boost::optional<Cell> face = s.getUniqueFace(coface);
//  		boost::optional<Cell> face;
//  		s.getUniqueFace(coface, face);
		
		if (face) {
		  return std::make_pair(*face, coface);
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
  return reductionPairProvider();
}


template<typename SComplex>
inline int CoreductionAlgorithm<SComplex>::operator()(){
  int cnt=0;

  for(;;){
	 boost::optional<std::pair<Cell, Cell> > nextPair = getNextPair();
	 
	 if (nextPair) {
		nextPair->first.template setColor<2>();
		nextPair->second.template setColor<2>();
		++cnt;++cnt;
		storeReductionPair(nextPair->second, nextPair->first);		
		addCellsToProcess(nextPair->first);
	 } else {
		// If the search failed or when we even did not try to search
		// and we know that a cell of lowest dimension is always
		// a homology generator like in the case of a vertex in
		// a compact set, we just pick up such a cell and
		// remove it from the complex
		boost::optional<Cell> sourceFace = extractor();

		if(sourceFace){
		  addCellsToProcess(*sourceFace);
		  sourceFace->template setColor<2>();
		  ++cnt;
		  storeGenerator(*sourceFace);
		}else{
		  break; // no base face left: quit any further processing
		}
	 } 
  }
  
  return cnt; // the number of cells removed
}

template<typename SComplex>
inline void CoreductionAlgorithm<SComplex>::addCellsToProcess(const Cell& sourceFace) {
  // Finally, put all present cofaces of the source face
  // into the queue
  for (typename SComplex::ColoredIterators::Iterators::CbdCells::iterator cbdn = s.template iterators<1>().cbdCells(sourceFace).begin(),
			end = s.template iterators<1>().cbdCells(sourceFace).end(); cbdn != end; ++cbdn) {
	 if(cbdn->getColor() == 1) cellsToProcess.push_back(*cbdn);
  }
}

template<typename SComplex>
inline void ShaveAlgorithm<SComplex>::operator()(){

  Cell face(s);
  for(int d=embeddingDim-1;d>=0;--d){
	 typedef typename SComplex::ColoredIterators::Iterators::DimCells::iterator DimIt;

	 for (DimIt it = s.template iterators<1>().dimCells(d).begin(),
			  end = s.template iterators<1>().dimCells(d).end();
			it != end; ++it) {
		boost::optional<Cell> face = s.getUniqueCoFace(*it);
		if (face) {
		  face->template setColor<2>();
		  it->template setColor<2>();
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
