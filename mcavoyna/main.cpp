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

#ifndef PLAYERS_INCL
#define PLAYERS_INCL
#include "Players.h"
#endif

Player *setPlayer(string name);

int main(int argc, char **argv) {
  srand(time(0));

  Player *white = setPlayer("UPPER CASE (White)");
  Player *black = setPlayer("lower case (Black)");
  HumanPlayer *p = new HumanPlayer(0);
  RandomPlayer *r = new RandomPlayer(0);

  Game *g = new Game(white, black);
  white->setGame(g);
  black->setGame(g);

  g->setUp();
  bool whiteWins = g->play();
  cout << (whiteWins? "Upper" : "Lower") << " case wins." << endl;
  Board *b = g->getBoard();
  cout << *b;
  delete g;
  return 0;
}

Player *setPlayer(string name) {
  char entry[255];
  cout << "Enter 'y' if the " << name << " team should be human." << endl;
  cin.getline(entry, 255);
  if (strlen(entry) == 1 && entry[0] == 'y') {
    cout << "Human it is!" << endl;
    return new HumanPlayer(0);
  }
  else {
    cout << "Very well then, a computer player." << endl;
    return new RandomPlayer(0);
  } 
}
