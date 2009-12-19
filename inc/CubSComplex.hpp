#include <sstream>

#include "capd/repSet/ElementaryCell.h"
#include "capd/auxil/CRef.h"
#include "capd/bitSet/CubCellSetT.hpp"
#include "capd/bitSet/CubSetT.hpp"
#include "capd/homologicalAlgebra/readCubCellSet.hpp"

typedef unsigned long int cluster;
typedef BitSetT<BitmapT<cluster> > BitSet;
typedef EuclBitSetT<BitSet,embeddingDim> EuclBitSet;
typedef CubSetT<EuclBitSet> BCubSet;
typedef CubCellSetT<EuclBitSet> BCubCelSet;
typedef BCubCelSet::PointIterator PointIterator;
typedef BCubCelSet::BitCoordIterator BitCoordIterator;
typedef BCubCelSet::BitIterator BitIterator;

class CubSComplex : public BCubCelSet{
  public:
    CubSComplex():
      BCubCelSet(),baseDimension(0)
    {}
    CubSComplex(const int* A_w, bool clear=false):
      BCubCelSet(A_w,clear),baseDimension(0)
    {}
    CubSComplex(const char* fileName):baseDimension(0){
      CRef<BCubCelSet> cubCellSetCR=readCubCellSet<BCubSet,BCubCelSet>(fileName);
      cubCellSetCR().addEmptyCollar();
      swap(*static_cast<BCubCelSet*>(this),cubCellSetCR());

    }
    using BCubCelSet::cardinality;

    class CellNumerator;
    class CellDimNumerator;
    class CbdNumerator;

    class Cell : public BitCoordIterator{
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

        Cell& operator=(bool b){
          if(!b){
            this->wIt=const_cast<WordIterator>(this->getBitmap().getBitmapEnd());
          }else{
            this->wIt= (WordIterator) this->getBitmap().getBitmap();
            this->bit=0;
          }
          return *this;
        }

        operator string() const{
          ostringstream o;
          o << *this;
          return o.str();
        }

        bool getCofaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
          return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeCoFace(*this,companion);
        }

/*      This general code seems to be slower
        bool getFaceCompanion(Cell& companion) {
          CbdNumerator cbdn(*this);
          int cnt=0;
          while(cbdn.MoveNext()){
            if(cbdn.Current().present()){
              if(++cnt>1) return false;
              companion=cbdn.Current();
            }
          }
          return (cnt ? true : false);
        }
*/

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

    class CellNumerator{
      public:
        CellNumerator(CubSComplex& s):cCell(s){
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
      protected:
        Cell cCell;
    };

    class CellDimNumerator{
      public:
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
        Cell& Current(){
          return cCell;
        }
      protected:
        Cell cCell;
        int dim;
    };

    class CbdNumerator{
      public:
        CbdNumerator(const Cell& c):cCell(c),center(c),i(0),downDir(true){
          cCell=false;
        }
        bool MoveNext(){
          while(i < cCell.embDim()){
            // process only directions in which cell is degenerate
            if(!downDir || !center.odd(i)){
              // First check the bottom face
              if(downDir){
                cCell=center;
                cCell.decInDir(i);
                downDir=false;
                return true;
              // and now go to the top face
              }else{
                cCell=center;
                cCell.incInDir(i);;
                downDir=true;
                ++i;
                return true;
              }
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


    // So far this code reduces to finding a vertex
    Cell getBaseCell(){
      CellNumerator cn(*this);
      while(cn.MoveNext()){
        if(!cn.Current().isValid() || cn.Current().ownDim()==baseDimension ) break;
      }
      return cn.Current();
    }

    // This is always false for standard cubical homology
    bool mayReduce(){
      return false;
    }

    // This is always true for standard cubical homology
    // as long as there remains a nonreduced vertex
    // A special code will have to be written for higher dimensions
    bool isExtractable(){
      return baseDimension==0;
    }

    void storeGenerator(const Cell& c){
      collectedHomGenerators.push_back(c);
    }

    // This will be needed only for homology maps
    void storeReductionPair(const Cell& coface, const Cell& face){
    }

/*
    operator string(){
      ostringstream o;
      CellNumerator cn(*this);
      Cell cell;
      while((cell=cn.get()).isValid()){
        o << "---" << cell << endl;
      }
      return o.str();
    }
*/

  protected:
    int baseDimension;
    std::vector<Cell> collectedHomGenerators;
};
