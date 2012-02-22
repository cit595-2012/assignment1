#define DIM 8

using namespace std;

class Square;
class Piece;
class Board;
class IntTuple;
int bound(int b);
void addSidewaysMoves(Piece *p);
void addDiagonalMoves(Piece *p);
void moveRay(Piece *p, int xdir, int ydir);
string itoa(int i);

/* Used to represent a position on the board. 
 * */
class IntTuple {
  private:
    int first;
    int second;
  public:
    IntTuple(int _fst, int _snd);

    int fst() const {return first;}
    int snd() const {return second;}
    string toString(); 
    bool operator== (const IntTuple &other) const;
    bool operator!= (const IntTuple &other) const;
    bool operator< (const IntTuple& other) const;
};

/* A Square on the board.
 */
class Square {
  private:
    IntTuple *pos;
    bool empty;
    Piece *myPiece;
    Board *parent;
  public:
    Square(Board * _p, int _x, int _y);
    ~Square();
    void moveInto(Piece *p);
    void moveOutOf();
    Board *getBoard() const {return parent;}
    IntTuple  *getPos() const {return pos;}
    Piece *resident() const {return myPiece;}
    bool isEmpty() const {return empty;}

    bool operator== (const Square &other) const;
    bool operator< (const Square& other) const;
    friend ostream& operator<< (ostream &out, const Square& sq); 
};

/* Abstract class for a chess piece.
 */
class Piece {
  protected:
    bool white;
    Square *home;
    set<IntTuple> *moves;
  public:
    Piece(Square *_h, bool _white);
    ~Piece() {
      delete moves;
    }
    virtual set<IntTuple> *legalMoves() = 0;
    Square *getHome() {return home;}
    Board *getBoard() {return home->getBoard();}
    void setHome(Square *sq) {home = sq;}
    bool isWhite() const {return white;}
    bool lookInto(int x, int y);
    virtual string toString() = 0;
};

class King: public Piece {
  public:
    King(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

class Rook: public Piece {
  public:
    Rook(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

class Bishop: public Piece {
  public:
    Bishop(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

class Queen: public Piece {
  public:
    Queen(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

class Knight: public Piece {
  public:
    Knight(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

class Pawn: public Piece {
  public:
    Pawn(Square *_h, bool _white) :Piece(_h, _white) {}
    set<IntTuple> *legalMoves();
    string toString();
};

/* The game board. Just a 2d square array.
 */
class Board {
  private:
    Square *squares[DIM][DIM];

  public:
    Board();
    ~Board() {
      for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
          delete squares[i][j];
        }
      }
    }
    Square *squareAt(int x, int y) const;
    friend ostream& operator<< (ostream &out, const Board& sq); 
};
