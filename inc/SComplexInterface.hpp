class SComplex{
  public:
    SComplex();
    SComplex(const char* fileName);

    class Cell{
      public:
        // Default constructor - should construct
        // something which bool converts to false
        Cell();
        bool isValid() const;
        Cell& operator=(bool b);
        operator string() const;
        bool getCofaceCompanion(Cell& companion);
        bool getFaceCompanion(Cell& companion);
        void remove();
        bool present() const;
        operator ElementaryCell();
    };

    class CellNumerator{
      public:
        CellNumerator(SComplex& s);
        bool MoveNext();
        Cell& Current();
    };

    class CellDimNumerator{
      public:
        CellDimNumerator(SComplex& s,int d);
        bool MoveNext();
        Cell& Current();
    };

    class CbdNumerator{
      public:
        CbdNumerator(const Cell& c);
        bool MoveNext();
        Cell& Current();
    };


    int cardinality();

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

};
