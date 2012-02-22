#include <set>
#include <sstream>
#include "Pieces.h"

/*IntTuple implementation*/
IntTuple::IntTuple(int _fst = 0, int _snd = 0) {
  first = _fst;
  second = _snd;
}
/* Comparison and equality operators overloaded for purpose of 
 * adding IntTuples to sets and easily comparing them.
 */
bool IntTuple::operator== (const IntTuple &other) const {
  return (first == other.fst() && second == other.snd());
} 
bool IntTuple::operator!= (const IntTuple &other) const {
  return !(*this == other);
} 
bool IntTuple::operator< (const IntTuple& other) const{
  if (first != other.fst()) return first < other.fst(); 
  if (second != other.snd()) return second < other.snd();
  return false;
}
string IntTuple::toString() {
  return "(" + itoa(first) + "," + itoa(second) + ")";
}

/*Square implementation*/
Square::Square(Board * _p = 0, int _x = 0, int _y = 0) {
  parent = _p;
  pos = new IntTuple(_x, _y);
  empty = true;
  myPiece = 0;
}
Square::~Square() {
  delete pos;
  if (!empty) delete myPiece;
}
void Square::moveInto(Piece *p){
  if (!empty) throw "Moving piece into occupied square!\n";
  myPiece = p;
  p->setHome(this);
  empty = false;
}
void Square::moveOutOf(){
  myPiece = 0;
  empty = true;
}
bool Square::operator== (const Square &other) const {
  return (parent == other.getBoard() && *pos == *other.getPos());
}
bool Square::operator< (const Square& other) const{
  return *pos < *other.getPos();
}
ostream& operator<<(ostream& out, const Square& sq){
      if (sq.isEmpty()) return out << " |"; 
      else return out << sq.resident()->toString() << "|";
}

/*Piece implementation*/
Piece::Piece(Square *_h = 0, bool _white = true) {
  white = _white;
  home = _h;
  moves = new set<IntTuple>();
}
/* If the given space is free or enemy-occupied,
 * adds it to the piece's list of moves. 
 * Returns true if the square was unoccupied and certain
 * pieces can thus continue adding moves.
 */
bool Piece::lookInto(int x, int y){
  if (x != bound(x) || y != bound(y)) return false;
  IntTuple *nt = new IntTuple(x, y);
  Square *sq = getBoard()->squareAt(x, y);
  if (sq->isEmpty() ||
      sq->resident()->isWhite() != white) {
    moves->insert(*nt); 
  }
  else delete nt;
  return sq->isEmpty();
}

/*King implementation*/
set<IntTuple> *King::legalMoves() {
  moves->clear();
  IntTuple *pos = home->getPos();
  int x = pos->fst();
  int y = pos->snd();
  for (int i = bound(x - 1); i <= bound(x + 1); i++) {
    for (int j = bound(y - 1); j <= bound(y + 1); j++) {
      lookInto(i, j);
    }
  }
  return moves;
}
string King::toString() {return white ? "K" : "k";}

/*Rook implementation*/
set<IntTuple> *Rook::legalMoves() {
  moves->clear();
  addSidewaysMoves(this);
  return moves;
}
string Rook::toString() {return white ? "R" : "r";}

/*Bishop implementation*/
set<IntTuple> *Bishop::legalMoves() {
  moves->clear();
  addDiagonalMoves(this);
  return moves;
}
string Bishop::toString() {return white ? "B" : "b";}

/*Queen implementation*/
set<IntTuple> *Queen::legalMoves() {
  moves->clear();
  addSidewaysMoves(this);
  addDiagonalMoves(this);
  return moves;
}
string Queen::toString() {return white ? "Q" : "q";}

/*Knight implementation*/
set<IntTuple> *Knight::legalMoves() {
  moves->clear();
  int x = home->getPos()->fst();
  int y = home->getPos()->snd();

  lookInto(x+1,y+2);
  lookInto(x+1,y-2);
  lookInto(x-1,y+2);
  lookInto(x-1,y-2);
  lookInto(x+2,y+1);
  lookInto(x+2,y-1);
  lookInto(x-2,y+1);
  lookInto(x-2,y-1);
  return moves;
}
string Knight::toString() {return white ? "N" : "n";}

/*Pawn implementation*/
set<IntTuple> *Pawn::legalMoves() {
  moves->clear();
  int x = home->getPos()->fst();
  int y = home->getPos()->snd();
  int yt = y + (white ? 1 : -1);
  if (yt == bound(yt)) {
    if (getBoard()->squareAt(x, yt)->isEmpty()) {
      moves->insert(*new IntTuple(x, yt));
      //Special case of two-space move allowed.
      if (white) {
        if (y == 1 && getBoard()->squareAt(x, 3)->isEmpty())
          moves->insert(*new IntTuple(x, 3));
      }
      else {
        if (y == 6 && getBoard()->squareAt(x, 4)->isEmpty())
          moves->insert(*new IntTuple(x, 4));
      }
    }

    int left = x-1;
    Square * sqAt = getBoard()->squareAt(left, yt);
    if (left == bound(left) &&
        !sqAt->isEmpty() && 
        white != sqAt->resident()->isWhite()) 
      moves->insert(*new IntTuple(left, yt));
  
    int right = x+1;
    sqAt = getBoard()->squareAt(right, yt);
    if (right == bound(right) && 
        !sqAt->isEmpty() && 
        white != sqAt->resident()->isWhite()) 
      moves->insert(*new IntTuple(right, yt));
  } 
  return moves;
}
string Pawn::toString() {return white ? "P" : "p";}

/*Board implementation*/
Board::Board() {
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      squares[i][j] = new Square(this, i, j);
    }
  }
}
Square *Board::squareAt(int x, int y) const {
  return squares[x][y];
}
ostream& operator<<(ostream& out, const Board& b){
  out <<
    "   a b c d e f g h" << endl <<
    "   ----------------" << endl;
  for (int i = DIM - 1; i >= 0; i--) {
    out << i+1 << " |";
    for  (int j = 0; j < DIM; j++) {
      out << *b.squareAt(j, i);
    }
    out << " " << i+1 << endl;
  }
  out << "   ----------------" << endl <<  
    "   a b c d e f g h" << endl;
}

/* If the integer is off the edge of the board,
 * returns the nearest edge.
 */
int bound(int i) {
  if (i < 0) return 0;
  if (i >= DIM) return DIM - 1;
  return i;
}

/* Add moves for Rook and Queen.
 */
void addSidewaysMoves(Piece *p) {
  moveRay(p,  1,  0);
  moveRay(p,  0,  1);
  moveRay(p, -1,  0);
  moveRay(p,  0, -1);
}

/* Add moves for Bishop and Queen.
 */
void addDiagonalMoves(Piece *p) {
  moveRay(p,  1,  1);
  moveRay(p,  1, -1);
  moveRay(p, -1,  1);
  moveRay(p, -1, -1);
} 

/* Starting with the piece, moves in the given x
 * and y directions, adding moves until an occupied square is found.
 */
void moveRay(Piece *p, int dx, int dy) {
  IntTuple *pos = p->getHome()->getPos();
  int x = pos->fst();
  int y = pos->snd();
  bool go = true;
  for (int i = x, j = y; i == bound(i) && j == bound(j) && go; 
      i += dx, j += dy) {
    if (x != i || y != j) go = p->lookInto(i, j);
  }
}

/* Converts an integer to a string.
 */
string itoa(int i) {
  stringstream out;
  out << i;
  return out.str();
}
