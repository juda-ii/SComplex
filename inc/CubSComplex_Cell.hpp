#ifndef CUB_SCOMPLEX_CELL_HPP
#define CUB_SCOMPLEX_CELL_HPP

#include "CubSComplex.hpp"

class CubSComplex::Cell: private BCubCellSet::BitCoordIterator {
	 friend class CellNumerator;
	 friend class CbdNumerator;
	 friend class CellDimNumerator;
	 typedef BCubCellSet::BitCoordIterator::WordIterator WordIterator;

public:
	 // Default constructor - should construct
	 // something which bool converts to false
	 Cell(): BitCoordIterator(EuclBitSet()){}
	 Cell(const CubSComplex& s): BitCoordIterator(s.bCubCellSetCR()){}

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

	 bool getCofaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
		  return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeCoFace(*this, companion);
	 }

	 bool getFaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
		  return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeFace(*this,companion);
	 }

	 Color getColor() const{
		  return this->getBit() ? 0 : 1;
	 }

	 template<Color color>
	 void setColor();

	 void setColor(const Color& color) {
		  BOOST_ASSERT(color == 0);
		  clearBit();
	 }

	 
	 Dim getDim() const {
		  return ownDim();
	 }
	 
	 ElementaryCell getElementaryCell() const { //conversion operator doesn't work here, because of copy constructor in ElementaryCell
		  int dim=this->embDim();
		  return ElementaryCell(this->coords(),dim);
	 }

	 
private:
	 
	 void toEnd() {
		  this->wIt=const_cast<WordIterator>(this->getBitmap().getBitmapEnd());
	 }

};

template<>
inline void CubSComplex::Cell::setColor<1>() {
	 clearBit();
}

#endif
