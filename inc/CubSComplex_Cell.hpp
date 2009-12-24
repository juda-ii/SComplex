#ifndef CUB_SCOMPLEX_CELL_HPP
#define CUB_SCOMPLEX_CELL_HPP

#include "CubSComplex.hpp"

class CubSComplex::Cell : public BCubCellSet::BitCoordIterator{
	 friend class CellNumerator;
	 friend class CellDimNumerator;
	 typedef BitCoordIterator::WordIterator WordIterator;
public:
	 // Default constructor - should construct
	 // something which bool converts to false
	 Cell():BitCoordIterator(EuclBitSet()){}
	 Cell(const CubSComplex& s):BitCoordIterator(s.bCubCellSetCR()){}

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

#endif
