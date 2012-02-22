#include <iostream>
#include <string>
#include <string.h>
#include <iterator>
#include <set>
#include <time.h>
#include <stdlib.h>

#ifndef PIECES_INCL
#define PIECES_INCL
#include "Pieces.h"
#endif

class Player;
class HumanPlayer;
class RandomPlayer;
class Move;
class Game;
bool teamCanMove(const IntTuple *pos, set<Piece *> *team); 
bool validateMoveSyntax(char entry[]);
bool checkInRange(int val, int min, int max);

/* Contains a piece and a coordinate to which it will move.
 */
class Move{
  private:
    Piece *p;
    IntTuple *tp;

  public:
    Move(Piece *_fr = 0, IntTuple *_tp = 0) {
      p = _fr;
      tp   = _tp;  
    }
    ~Move() {
      delete tp;
    }

    Piece *who() {return p;}
    IntTuple *to()   {return tp;}
};

/* Abstract class. A player gives moves.
 */
class Player{
  protected:
    Game *game;

  public:
    Player(Game *_g = 0) {game = _g;}
    ~Player() {}
    set<Piece *> *getTeam();
    virtual Move *getMove() = 0;
    bool setGame(Game *_g) {game = _g;}
};

/* Gets moves by asking standard input for them.
 */
class HumanPlayer: public Player{
  public:
    HumanPlayer(Game *_g): Player(_g) {}
    Move *getMove();
};

/* Gets moves by randomly choosing a piece and randomly
 * choosing one of those piece's legal moves.
 */
class RandomPlayer: public Player{
  public:
    RandomPlayer(Game *_g): Player(_g) {}
    Move *getMove();
};

/* Controls everything. Connects players with sets of pieces on a board.
 * Tells them to play.
 */
class Game{
  private:
   Player *whiteP;
   Player *blackP;
   bool whiteTurn;
   set<Piece *> *whiteTeam;
   set<Piece *> *blackTeam;
   King *whiteKing;
   King *blackKing;
   Board *board;

  public:
    Game(Player *_wp = 0, Player *_bp = 0) { 
       whiteP = _wp;
       blackP = _bp;
       whiteTurn = true;
       whiteTeam = new set<Piece *>();
       blackTeam = new set<Piece *>();
       board = new Board();
    }
    ~Game() {
     delete whiteP;
     delete blackP;
     delete board;
    }
    Board *getBoard() {return board;}
    void setUp();
    bool inCheckmate();
    bool play();
    bool makeMove(Move *m); 
    /* Takes a King piece and slots it as the head of its
     * respective team.
     */
    void setKing(King *k) {
      if (k->isWhite()) whiteKing = k;
      else blackKing = k;
    }
    /* Tells whose turn it is. */
    bool whitesTurn() {return whiteTurn;}
    /* Fetches the pieces for a given team. */
    set<Piece *> *getTeam(bool white) {
      return white ? whiteTeam : blackTeam;
    }
    /* Fetches a team's king. */
    King *getKing(bool white) {
      return white ? whiteKing : blackKing;
    }
    /* Fetches the player whose turn it is. */
    Player *whosTurn() {
      return whiteTurn ? whiteP : blackP;
    }

};
