#include <sstream>

#include "capd/repSet/ElementaryCell.h"
#include "capd/auxil/CRef.h"
#include "capd/bitSet/CubCellSetT.hpp"
#include "capd/bitSet/CubSetT.hpp"
#include "capd/homologicalAlgebra/readCubCellSet.hpp"

#include <boost/mpl/if.hpp>
#include <iterator>

typedef unsigned long int cluster;
typedef BitSetT<BitmapT<cluster> > BitSet;
typedef EuclBitSetT<BitSet,embeddingDim> EuclBitSet;
typedef CubSetT<EuclBitSet> BCubSet;
typedef CubCellSetT<EuclBitSet> BCubCelSet;
typedef BCubCelSet::PointIterator PointIterator;
typedef BCubCelSet::BitCoordIterator BitCoordIterator;
typedef BCubCelSet::BitIterator BitIterator;

class CubSComplex : public BCubCelSet{
private:

  template<typename NumeratorT, bool isConst>
  class IteratorProvider;
  class CellNumerator;
  
public:

  class CellDimNumerator;
  class CbdNumerator;

  class Cell;


  typedef IteratorProvider<CellNumerator, false> AllCellsIterators;
  typedef IteratorProvider<CellNumerator, true> AllCellsConstIterators;

  typedef IteratorProvider<CellDimNumerator, false> DimCellsIterators;
  typedef IteratorProvider<CellDimNumerator, true> DimCellsConstIterators;

  typedef int Dim;
  
  CubSComplex();
  CubSComplex(const int* A_w, bool clear=false);
  CubSComplex(const char* fileName);

  using BCubCelSet::cardinality;

  AllCellsIterators allCellsIterators();
  AllCellsConstIterators allCellsIterators() const;

  DimCellsIterators dimCellsIterators();
  DimCellsConstIterators dimCellsIterators() const;

  
  // So far this code reduces to finding a vertex
  Cell getBaseCell();
  
  // This is always false for standard cubical homology
  bool mayReduce();

  // This is always true for standard cubical homology
    // as long as there remains a nonreduced vertex
    // A special code will have to be written for higher dimensions
  bool isExtractable();

    void storeGenerator(const Cell& c);

    // This will be needed only for homology maps
  void storeReductionPair(const Cell& coface, const Cell& face);
  
  protected:
    int baseDimension;
    std::vector<Cell> collectedHomGenerators;
  
};

    class CubSComplex::Cell : public BitCoordIterator{
      friend class CellNumerator;
      friend class CellDimNumerator;
      typedef BitCoordIterator::WordIterator WordIterator;
      public:
        // Default constructor - should construct
        // something which bool converts to false
        Cell():BitCoordIterator(EuclBitSet()){}
        Cell(const PointIterator& ptIt):BitCoordIterator(ptIt){}
        Cell(const BitCoordIterator& ptIt):BitCoordIterator(ptIt){}

        bool isValid() const{
            return this->wIt < this->getBitmap().getBitmapEnd();
        }

		void toEnd() {
		  this->wIt=const_cast<WordIterator>(this->getBitmap().getBitmapEnd());
		}
		
        Cell& operator=(bool b){
          if(!b){
            this->wIt=const_cast<WordIterator>(this->getBitmap().getBitmapEnd());
          }else{
            this->wIt= (WordIterator) this->getBitmap().getBitmap();
            this->bit=0;
          }
          return *this;
        }

        bool getCofaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
          return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeCoFace(*this,companion);
        }

        bool getFaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
          return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeFace(*this,companion);
        }

        void remove(){
          this->clearBit();
        }
        bool present() const{
          return this->getBit()!=0;
        }
        operator ElementaryCell(){
          int dim=this->embDim();
          return ElementaryCell(this->coords(),dim);
        }
    };

  template<typename NumeratorT, bool isConst>
  class CubSComplex::IteratorProvider {
  public:
	 typedef NumeratorT Numerator;

  private:
	 template<typename ValueT>
	 class IteratorFromNumeratorAdapter;

	 typedef typename boost::mpl::if_c<isConst, const typename Numerator::value_type, typename Numerator::value_type>::type ValueT;
  public:
	 
	 typedef IteratorFromNumeratorAdapter<ValueT>  iterator;

	 IteratorProvider(const Numerator& numerator): first(numerator), last(findLast(numerator)) {}
	 
	 iterator begin() { return iterator(first); }
	 iterator end() { return iterator(last); }

	 private:

	 static Numerator findLast(Numerator numerator) {
		numerator.toEnd();
		return numerator;
	 }
	 
	 const Numerator first, last;
  };


template<typename NumeratorT, bool isConst>
template<typename ValueT>
class CubSComplex::IteratorProvider<NumeratorT, isConst>::IteratorFromNumeratorAdapter: public std::iterator<forward_iterator_tag, ValueT> {

  typedef std::iterator<forward_iterator_tag, ValueT> Base;
  typedef typename IteratorProvider<NumeratorT, isConst>::Numerator Numerator;
  Numerator currentNumerator;


  
public:

	 explicit IteratorFromNumeratorAdapter(const Numerator& numerator): currentNumerator(numerator) {}

	 IteratorFromNumeratorAdapter& operator++() {
		currentNumerator.MoveNext();
		return *this;
	 }

	 IteratorFromNumeratorAdapter operator++(int) {
		IteratorFromNumeratorAdapter tmp(*this);
		currentNumerator.MoveNext();
		return tmp;
	 }

  typename Base::reference operator*() {
		return currentNumerator.Current();
	 }

  typename Base::pointer operator->()  {
		return &currentNumerator.Current();
	 }

	 bool operator==(const IteratorFromNumeratorAdapter& o) const {
		return this->currentNumerator == o.currentNumerator;
	 }

	 bool operator!=(const IteratorFromNumeratorAdapter& o) const {
		return !(*this == o);
	 }

  };


class CubSComplex::CellNumerator{
  
public:
  typedef Cell value_type;
  
  CellNumerator(const CubSComplex& s):cCell(s){
	 --cCell;
  }
  
        bool MoveNext(){
          ++cCell;
          cCell.moveToFirstPixel();
          return cCell.wIt < cCell.getBitmap().getBitmapEnd();
        }

        Cell& Current(){
          return cCell;
        }

  void toEnd() {
	 cCell.toEnd();
  }
  
  bool operator==(const CellNumerator& o) const {
	 return this->cCell.wIt == o.cCell.wIt;
  }

protected:
        Cell cCell;
    };

    class CubSComplex::CellDimNumerator{
	 public:
		typedef Cell value_type;
		
        CellDimNumerator(CubSComplex& s,int d):cCell(s),dim(d){
          --cCell;
        }

        bool MoveNext(){
          for(;;){
            ++cCell;
            cCell.moveToFirstPixel();
            if(cCell.wIt == cCell.getBitmap().getBitmapEnd()) return false;
            if(cCell.ownDim()==dim) return true;
          }
        }
		void toEnd() {
		  cCell.toEnd();
		}
		
		Cell& Current(){
          return cCell;
        }
      protected:
        Cell cCell;
        int dim;
    };

    class CubSComplex::CbdNumerator{
	 public:
		typedef Cell value_type;
		
        CbdNumerator(const Cell& c):cCell(c),center(c),i(0),downDir(true){
          cCell=false;
        }
        bool MoveNext(){
          while(i < cCell.embDim()){
            // process only directions in which cell is degenerate
            if(!downDir || !center.odd(i)){
				  cCell=center;
              // First check the bottom face
              if(downDir){
                cCell.decInDir(i);
                downDir=false;
              // and now go to the top face
              }else{
                cCell.incInDir(i);;
                downDir=true;
                ++i;
              }
				  return true;
            }else{
              ++i;
            }
          }
          cCell=false;
          return false;
        }
        Cell& Current(){
          return cCell;
        }
      protected:
        Cell cCell,center;
        int i;
        bool downDir;
    };

  
inline CubSComplex::CubSComplex():
  BCubCelSet(),baseDimension(0)
{}

inline CubSComplex::CubSComplex(const int* A_w, bool clear):
  BCubCelSet(A_w,clear),baseDimension(0)
{}

inline CubSComplex::CubSComplex(const char* fileName):baseDimension(0){
  CRef<BCubCelSet> cubCellSetCR=readCubCellSet<BCubSet,BCubCelSet>(fileName);
  cubCellSetCR().addEmptyCollar();
  swap(*static_cast<BCubCelSet*>(this),cubCellSetCR());

}

  
// So far this code reduces to finding a vertex
inline CubSComplex::Cell CubSComplex::getBaseCell(){
  CellNumerator cn(*this);
  while(cn.MoveNext()){
	 if(!cn.Current().isValid() || cn.Current().ownDim()==baseDimension ) break;
  }
  return cn.Current();
}

// This is always false for standard cubical homology
inline bool CubSComplex::mayReduce(){
  return false;
}

// This is always true for standard cubical homology
// as long as there remains a nonreduced vertex
// A special code will have to be written for higher dimensions
inline bool CubSComplex::isExtractable(){
  return baseDimension==0;
}

inline void CubSComplex::storeGenerator(const CubSComplex::Cell& c){
  collectedHomGenerators.push_back(c);
}

// This will be needed only for homology maps
inline void CubSComplex::storeReductionPair(const CubSComplex::Cell& coface, const CubSComplex::Cell& face){
}

inline CubSComplex::AllCellsIterators CubSComplex::allCellsIterators() {
  return AllCellsIterators(CellNumerator(*this));
}

inline CubSComplex::AllCellsConstIterators CubSComplex::allCellsIterators() const {
  return AllCellsConstIterators(CellNumerator(*this));
}

inline CubSComplex::DimCellsIterators CubSComplex::dimCellsIterators() {
  return DimCellsIterators(CellDimNumerator(*this));
}

inline CubSComplex::DimCellsConstIterators CubSComplex::dimCellsIterators() const {
  return DimCellsConstIterators(CellDimNumerator(*this));
}
