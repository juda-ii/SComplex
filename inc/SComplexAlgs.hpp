
template<typename SComplex>
class SComplexAlgs{
  public:
  typedef typename SComplex::Cell Cell;
  typedef typename SComplex::CellNumerator CellNumerator;
  typedef typename SComplex::CellDimNumerator CellDimNumerator;
  typedef typename SComplex::CbdNumerator CbdNumerator;

  static void test(SComplex& s){
    CellNumerator cn(s);
    while(cn.MoveNext()){
      cout << cn.Current() << endl;
      CbdNumerator cbdn(cn.Current());
      while(cbdn.MoveNext()){
        if(cbdn.Current().present()) cout << "   " << cbdn.Current() << endl;
      }
    }
  }

  static void shave(SComplex& s){
    for(int d=embeddingDim-1;d>=0;--d){
      Cell face;
      CellDimNumerator cn(s,d);
      while(cn.MoveNext()){
        if(cn.Current().getFaceCompanion(face)){
          face.remove();
          cn.Current().remove();
        }
      }
    }
  }

  static int coreduce(SComplex& s){
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
          // Go through all cells and search for a coreduction pair
          CellNumerator cn(s);
          while(cn.MoveNext()){
            // if coface is a free coface,
            // then the search is successful.
            // We preserve the companion face
            // and quit the search
            if(crpFound=cn.Current().getCofaceCompanion(face)){
              break; // while loop
            }
          }
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
      CbdNumerator cbdn(sourceFace);
      while(cbdn.MoveNext()){
        if(cbdn.Current().present()) cellsToProcess.push_back(cbdn.Current());
      }
    }
    return cnt; // the number of cells removed
  }


// -------------------------------------------------------------------------------------- //

  template<typename FCComplex_P, typename SourceGenerator_P>
  static CRef<FCComplex_P> ReducibleFreeChainComplexOverZFromSComplex(CRef<SComplex> A_SComplexCR){

    typedef typename FCComplex_P::GeneratorType GeneratorType;

    Stopwatch sw;
    std::set<SourceGenerator_P> cells;

    CellNumerator cn(A_SComplexCR());

    while(cn.MoveNext()){
      cells.insert((SourceGenerator_P)cn.Current());
    }

    A_SComplexCR =CRef<SComplex>(new SComplex); // to free memory
    // -- MM std::cout << " rfccCR construction from " << cells.size() <<  " cells starting "  << std::endl;      // -- MM
    CRef<FCComplex_P> rfccCR( new FCComplex_P(cells));
    // -- MMstd::cout << rfccCR()  << std::endl;
    fcout << "Reducible chain complex (over Z) construction of CubCelSet completed in " << sw  << std::endl;
    return rfccCR;

  }

};
