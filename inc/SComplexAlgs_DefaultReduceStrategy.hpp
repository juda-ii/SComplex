#ifndef SCOMPLEX_ALGS_DEFAULT_REDUCE_STRATEGY_HPP_
#define SCOMPLEX_ALGS_DEFAULT_REDUCE_STRATEGY_HPP_

template<typename SComplexT>
class DefaultReduceStrategy {

public:
  typedef SComplexT SComplex;
  typedef typename SComplex::Cell Cell;
  typedef std::pair<boost::reference_wrapper<Cell>, boost::reference_wrapper<Cell> > CoreductionPair;
  typedef std::pair<boost::reference_wrapper<Cell>, boost::reference_wrapper<Cell> > ReductionPair;  


  DefaultReduceStrategy(SComplex& _complex): complex(_complex), dummyCell1(_complex), dummyCell2(_complex),  dummyCell3(_complex) {}
  
  SComplex& getComplex() const {
	 return complex;
  }

  Cell& getFace(const CoreductionPair& coRedPair) {
	 return boost::unwrap_ref(coRedPair.first);
  }
  
  bool reduced(const Cell& cell) const {
	 return cell.getColor() == 2;
  }

  void coreduce(const CoreductionPair& coRedPair) const {
	 boost::unwrap_ref(coRedPair.first).template setColor<2>();
	 boost::unwrap_ref(coRedPair.second).template setColor<2>();
  }

  void reduce(const ReductionPair& redPair) const {
	 boost::unwrap_ref(redPair.first).template setColor<2>();
	 boost::unwrap_ref(redPair.second).template setColor<2>();
  }

  void reduce(Cell& cell) {
	 cell.template setColor<2>();
  }
  
  boost::optional<Cell&> extract() {
	 typename SComplex::ColoredIterators::Iterators::DimCells::iterator end = complex.iterators(1).dimCells(0).end(),
		it = complex.iterators(1).dimCells(0).begin();

	 if (it != end) { 
		dummyCell1 = *it;
		return dummyCell1;
	 }
	 return boost::optional<Cell&>();	 
  }
  
  boost::optional<CoreductionPair> forceCoreductionPair() {
	 return boost::optional<CoreductionPair>();
  }

  boost::optional<CoreductionPair> getCoreductionPair(Cell& cell) {
	 if (complex.getUniqueFace(cell, dummyCell3)) {
	  	return std::make_pair(boost::ref(dummyCell3), boost::ref(cell));
	 } else {
		return boost::optional<ReductionPair>();
	 }
  }
  
  boost::optional<ReductionPair> getReductionPair(Cell& cell) {
	 if (complex.getUniqueCoFace(cell, dummyCell2)) {
	  	return std::make_pair(boost::ref(cell), boost::ref(dummyCell2));
	 } else {
		return boost::optional<ReductionPair>();
	 }
  }

private:
  SComplex& complex;
  Cell dummyCell1, dummyCell2, dummyCell3;
};

#endif

