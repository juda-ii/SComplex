#ifndef CUB_SCOMPLEX_ITERATORS_HPP
#define CUB_SCOMPLEX_ITERATORS_HPP

#include "CubSComplex.hpp"
#include <boost/mpl/if.hpp>



template<bool isConst>
class CubSComplex::IteratorsImpl {
  typedef typename boost::mpl::if_c<isConst, const CubSComplex&, CubSComplex&>::type SComplexRef;
public:
  typedef CubSComplex::IteratorProvider<CellNumerator, isConst> AllCells;
  typedef CubSComplex::IteratorProvider<CellDimNumerator, isConst> DimCells;
  typedef CubSComplex::IteratorProvider<CbdNumerator, isConst> BdCells;  
  typedef CubSComplex::IteratorProvider<CbdNumerator, isConst> CbdCells;


  IteratorsImpl(SComplexRef _scomplex): scomplex(_scomplex) {}
  
  AllCells allCells() const;
  DimCells dimCells(const Dim& dim) const;
  CbdCells bdCells(const Cell& cell) const;
  CbdCells cbdCells(const Cell& cell) const;
  
private:
  SComplexRef scomplex;
};

template<bool isConst>
inline typename CubSComplex::IteratorsImpl<isConst>::AllCells CubSComplex::IteratorsImpl<isConst>::allCells() const {
  return AllCells(CellNumerator(scomplex));
}

template<bool isConst>
inline typename CubSComplex::IteratorsImpl<isConst>::DimCells CubSComplex::IteratorsImpl<isConst>::dimCells(const Dim& dim) const {
  return DimCells(CellDimNumerator(scomplex, dim));
}

template<bool isConst>
inline typename CubSComplex::IteratorsImpl<isConst>::CbdCells CubSComplex::IteratorsImpl<isConst>::cbdCells(const Cell& cell) const {
  return CbdCells(CbdNumerator(cell));
}


#endif
