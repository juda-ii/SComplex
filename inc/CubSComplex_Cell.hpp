#ifndef CUB_SCOMPLEX_CELL_HPP
#define CUB_SCOMPLEX_CELL_HPP

#include "CubSComplex.hpp"

class CubSComplex::Cell: private BCubCellSet::BitCoordIterator {
  friend class CellNumerator;
  friend class CbdNumerator;
  friend class CellDimNumerator;
  friend class CubSComplex;
  typedef BCubCellSet::BitCoordIterator::WordIterator WordIterator;

public:
  Cell(const CubSComplex& s): BitCoordIterator(s.bCubCellSetCR()) {}

  Color getColor() const{
	 return this->getBit() ? 1 : 2;
  }

  template<Color color>
  void setColor();

  void setColor(const Color& color) {
	 BOOST_ASSERT(color == 2);
	 clearBit();
  }

	 
  Dim getDim() const {
	 return ownDim();
  }
	 
  ElementaryCell getElementaryCell() const { //conversion operator doesn't work here, because of copy constructor in ElementaryCell
	 int dim=this->embDim();
	 return ElementaryCell(this->coords(),dim);
  }

  bool getFaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
	 return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeFace(*this,companion);
  }

  inline boost::optional<Cell> getUniqueCoFace(const CubSComplex& complex) {
	 boost::optional<CubSComplex::Cell> result(complex);
	 if (complex.bCubCellSetCR().isFreeFace(*this, *result)) {
	 } else {
		result.reset();
	 }
	 return result;
  }

private:
	 
  void toEnd() {
	 this->wIt=const_cast<WordIterator>(this->getBitmap().getBitmapEnd());
  }
};

template<>
inline void CubSComplex::Cell::setColor<2>() {
  clearBit();
}

#endif
