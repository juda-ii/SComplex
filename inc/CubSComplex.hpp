#ifndef CUB_SCOMPLEX_HPP
#define CUB_SCOMPLEX_HPP


#include <sstream>

#include "capd/repSet/ElementaryCell.h"
#include "capd/auxil/CRef.h"
#include "capd/bitSet/CubCellSetT.hpp"
#include "capd/bitSet/CubSetT.hpp"
#include "capd/homologicalAlgebra/readCubCellSet.hpp"

#include <boost/assert.hpp>
#include <boost/optional.hpp>


class CubSComplex {
public:

  class Cell;
  typedef int Dim;
  typedef int Color;

private:

  template<typename NumeratorT, bool isConst>
  class IteratorProvider;

  class CellNumerator;
  class CellDimNumerator;
  class CbdNumerator;

  typedef unsigned long int cluster;
  typedef BitSetT<BitmapT<cluster> > BitSet;
  typedef EuclBitSetT<BitSet,embeddingDim> EuclBitSet;
  typedef CubSetT<EuclBitSet> BCubSet;
  typedef CubCellSetT<EuclBitSet> BCubCellSet;


  template<bool isConst>
  class IteratorsImpl;

  template<bool isConst>
  class ColoredIteratorsImpl {

  public:
	 typedef IteratorsImpl<isConst> Iterators;

	 template<CubSComplex::Color color>
	 class Color {
	 public:
		typedef IteratorsImpl<isConst> Iterators;
	 };
  };

public:
  
  typedef IteratorsImpl<false> Iterators;
  typedef IteratorsImpl<true> ConstIterators;


  typedef ColoredIteratorsImpl<false> ColoredIterators;
  typedef ColoredIteratorsImpl<true> ColoredConstIterators;
  
  CubSComplex();
  CubSComplex(const int* A_w, bool clear=false);
  explicit CubSComplex(CRef<BCubCellSet> _bCubCellSet);

  size_t cardinality();

   ConstIterators iterators() const;
   Iterators iterators();

  ColoredConstIterators::Iterators iterators(const Color& color) const;
  ColoredIterators::Iterators iterators(const Color& color);

  template<Color color>
  typename ColoredIterators::Color<color>::Iterators iterators();

  template<Color color>
  typename ColoredConstIterators::Color<color>::Iterators iterators() const;

  bool getFaceCompanion(Cell& cell, Cell& companion);

  boost::optional<Cell> getUniqueFace(const Cell& cell) const;
  
  boost::optional<Cell> getUniqueCoFace(const Cell& cell) const;
  
  Dim getBaseDimension() const;

protected:
  CRef<BCubCellSet> bCubCellSetCR;
  Dim baseDimension;
  
  template<bool isConst>
  friend class IteratorsImpl;
  
};

inline CubSComplex::Dim CubSComplex::getBaseDimension() const {
  return baseDimension;
}

inline size_t CubSComplex::cardinality() {
  return bCubCellSetCR().cardinality();
}

#include "CubSComplex_Cell.hpp"
#include "CubSComplex_IteratorProvider.hpp"
#include "CubSComplex_Iterators.hpp"
#include "CubSComplex_ColoredIterators.hpp"
#include "CubSComplex_Numerators.hpp"

inline CubSComplex::CubSComplex():
  bCubCellSetCR(new BCubCellSet()), baseDimension(0)
{}

inline CubSComplex::CubSComplex(const int* A_w, bool clear):
  bCubCellSetCR(new BCubCelSet(A_w,clear)),baseDimension(0)
{}

inline CubSComplex::CubSComplex(CRef<BCubCellSet> _bCubCellSet):baseDimension(0){
  bCubCellSetCR = _bCubCellSet;
  bCubCellSetCR().addEmptyCollar();
}

inline CubSComplex::Iterators CubSComplex::iterators() {
  throw std::logic_error("Not implemented yet."); // How cn I iterate over removed elements ?

  return Iterators(*this);
}

inline CubSComplex::ConstIterators CubSComplex::iterators() const {
  throw std::logic_error("Not implemented yet."); // How cn I iterate over removed elements ?

  return ConstIterators(*this);
}

inline CubSComplex::ColoredConstIterators::Iterators CubSComplex::iterators(const Color& color) const {
  BOOST_ASSERT(color == 1);
  return ColoredConstIterators::Iterators(*this);
}

inline CubSComplex::ColoredIterators::Iterators CubSComplex::iterators(const Color& color) {
  BOOST_ASSERT(color == 1);
  return ColoredIterators::Iterators(*this);
}

template<>
inline CubSComplex::ColoredIterators::Color<1>::Iterators CubSComplex::iterators<1>() {
  return ColoredIterators::Color<1>::Iterators(*this);
}

template<>
inline CubSComplex::ColoredConstIterators::Color<1>::Iterators CubSComplex::iterators<1>() const {
  return ColoredConstIterators::Color<1>::Iterators(*this);
}


// 	 bool getCofaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
// 		  return reinterpret_cast<const BCubCelSet*>(this->itSet)->
// 	 }

// 	 bool getFaceCompanion(Cell& companion) {   // should be const, requires changes in isFreeCoFace to be const
// 		  return reinterpret_cast<const BCubCelSet*>(this->itSet)->isFreeFace(*this,companion);
// 	 }

// bool CubSComplex::getFaceCompanion(Cell& cell, Cell& companion) {
//  	 return reinterpret_cast<const BCubCelSet*>(cell.itSet)->isFreeFace(cell,companion);
//  }

inline bool CubSComplex::getFaceCompanion(Cell& cell, Cell& companion) {
  ColoredIterators::Iterators::CbdCells::iterator it = this->iterators(1).cbdCells(cell).begin(),
	 end = this->iterators(1).cbdCells(cell).end();

  int cnt=0;
  while(it != end){
	 if(it->getColor() == 1){
		if(++cnt>1) return false;
		companion=*it;;
	 }
	 ++it;
  }
  return (cnt ? true : false);
}

inline boost::optional<CubSComplex::Cell> CubSComplex::getUniqueFace(const Cell& cell) const {
  Cell face(*this);
  if (bCubCellSetCR().isFreeCoFace(const_cast<Cell&>(cell), face)) {
	 return face;
  } else {
	 return boost::optional<Cell>(); 
  }
}

inline boost::optional<CubSComplex::Cell> CubSComplex::getUniqueCoFace(const Cell& cell) const {
  Cell coface(*this);
  if (bCubCellSetCR().isFreeFace(const_cast<Cell&>(cell), coface)) {
	 return coface;
  } else {
	 return boost::optional<Cell>(); 
  }
}

#endif

