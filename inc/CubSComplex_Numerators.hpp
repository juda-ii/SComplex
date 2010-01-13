#ifndef CUB_SCOMPLEX_NUMERATORS_HPP
#define CUB_SCOMPLEX_NUMERATORS_HPP

#include "CubSComplex.hpp"

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

	 bool operator!=(const CellNumerator& o) const {
		  return this->cCell.wIt != o.cCell.wIt;
	 }

protected:
	 Cell cCell;
};

class CubSComplex::CellDimNumerator{
public:
	 typedef Cell value_type;
		
	 CellDimNumerator(const CubSComplex& s,int d):cCell(s),dim(d){
		  --cCell;
	 }


	 bool operator==(const CellDimNumerator& o) const {
		  return this->cCell.wIt == o.cCell.wIt;
	 }

	 bool operator!=(const CellDimNumerator& o) const {
		  return this->cCell.wIt != o.cCell.wIt;
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
		
  CbdNumerator(const Cell& c):center(c),i(0),downDir(true), dim(c.embDim()), cCell(c){
		//		  cCell=false;
	 }

  void toEnd() {
	 cCell.toEnd();
  }
		
	 bool operator==(const CbdNumerator& o) const {
		  return this->cCell.wIt == o.cCell.wIt;
	 }

	 bool operator!=(const CbdNumerator& o) const {
		  return this->cCell.wIt != o.cCell.wIt;
	 }

	 bool MoveNext(){
		  while(i < dim){
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
		  //cCell=false;
		  cCell.toEnd();
		  return false;
	 }
	 Cell& Current(){
		  return cCell;
	 }
protected:
  Cell cCell;
  const Cell& center;
  int i;
  const int dim;
  bool downDir;

};


#endif // CUB_SCOMPLEX_NUMERATORS_HPP
