#include <boost/bind.hpp>

template<typename SComplex>
class SComplexAlgs{
  public:
  typedef typename SComplex::Cell Cell;
  
  void shave(SComplex& s){
    for(int d=embeddingDim-1;d>=0;--d){
		typedef typename SComplex::Iterators::DimCells::iterator DimIt;

      Cell face;
		for (DimIt it = s.iterators().dimCells(d).begin(),
				 end = s.iterators().dimCells(d).end();
			  it != end; ++it) {
		  if(it->getFaceCompanion(face)){
          face.remove();
          it->remove();
        }
      }
    }
  }

  int coreduce(SComplex& s){
    Cell coface,face,sourceFace;
    // A queue for coreduction candidates
    std::deque<Cell> cellsToProcess;
    // a counter
    int cnt=0;
    // main loop
    for(;;){
      sourceFace=false;
      coface=false;
      face=false;
      bool crpFound=false;

      // Originally there are no candidates in the queue
      // They may also disappear when a connected component
      // is exhausted
      if(cellsToProcess.empty()){
        // If we know that a coreduction may be there,
        // For instance when treating a non-compact set
        if(s.mayReduce()){
			 typename SComplex::Iterators::AllCells::iterator crpIt = std::find_if(s.iterators().allCells().begin(),
																										  s.iterators().allCells().end(),
																										  boost::bind(&Cell::getCofaceCompanion, _1, face));
			 if (crpIt != s.iterators().allCells().end()) {
				crpFound = *crpIt;
			 }
          // Go through all cells and search for a coreduction pair
            // if coface is a free coface,
            // then the search is successful.
            // We preserve the companion face
            // and quit the search
        }
		  
        // If the search failed or when we even did not try to search
        // and we know that a cell of lowest dimension is always
        // a homology generator like in the case of a vertex in
        // a compact set, we just pick up such a cell and
        // remove it from the complex
        if(!coface.isValid() && s.isExtractable()){
          sourceFace=s.getBaseCell();
          if(sourceFace.isValid()){
            sourceFace.remove();
            ++cnt;
            s.storeGenerator(sourceFace);
          }else{
            break; // no base face left: quit any further processing
          }
        }else{ // no coreduction, no extraction: quit any further processing
          break; // for loop
        }
      }else{ // the queue is non-empty
        // get a cell from the queue
        coface=cellsToProcess.front();
        cellsToProcess.pop_front();
        // the cell may have been already removed
        if(!coface.present()) continue; // for loop
        // check if a free coface and store the companion
        crpFound=coface.getCofaceCompanion(face);
      }
      // process the possible reduction unless
      // we are just after an augmentation
      // which is the only case when sourceFace is already true;
      if(!sourceFace.isValid()){
        // The source for populating the stack will be
        // the coface
        sourceFace = coface;
        // unless the coface is free, indicated by face==true
        // and then we set sourceFace to face, remove and store the
        // coreduction pair
        if(crpFound){
          sourceFace = face;
          face.remove();
          coface.remove();
          ++cnt;++cnt;
          s.storeReductionPair(coface,face);
        }
      }
      // Finally, put all present cofaces of the source face
      // into the queue
		for (typename SComplex::Iterators::CbdCells::iterator cbdn = s.iterators().cbdCells(sourceFace).begin(),
				 end = s.iterators().cbdCells(sourceFace).end(); cbdn != end; ++cbdn) {
        if(cbdn->present()) cellsToProcess.push_back(*cbdn);
      }
    }
    return cnt; // the number of cells removed
  }


// -------------------------------------------------------------------------------------- //

  template<typename FCComplex_P, typename SourceGenerator_P>
  CRef<FCComplex_P> ReducibleFreeChainComplexOverZFromSComplex(CRef<SComplex> A_SComplexCR){

    typedef typename FCComplex_P::GeneratorType GeneratorType;

    Stopwatch sw;
    std::set<SourceGenerator_P> cells;


	 for (typename SComplex::Iterators::AllCells::iterator it = A_SComplexCR().iterators().allCells().begin(),
			  end = A_SComplexCR().iterators().allCells().end();
			it != end; ++it) {
      cells.insert((SourceGenerator_P)*it);
	 }

    A_SComplexCR =CRef<SComplex>(new SComplex); // to free memory
    // -- MM std::cout << " rfccCR construction from " << cells.size() <<  " cells starting "  << std::endl;      // -- MM
    CRef<FCComplex_P> rfccCR( new FCComplex_P(cells));
    // -- MMstd::cout << rfccCR()  << std::endl;
    fcout << "Reducible chain complex (over Z) construction of CubCelSet completed in " << sw  << std::endl;
    return rfccCR;

  }

};
