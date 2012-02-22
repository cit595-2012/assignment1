#include "Players.h"

/* Puts the initial pieces on the board. */
void Game::setUp() {
  //Pawns
  for (int i = 0; i < DIM; i++) {
    Square *ws = board->squareAt(i, 1);
    Pawn *wp = new Pawn(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);

    Square *bs = board->squareAt(i, 6);
    Pawn *bp = new Pawn(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
  }

  //Rooks
  for (int i = 0; i < DIM; i += 7) {
    Square *ws = board->squareAt(i, 0);
    Rook *wp = new Rook(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);

    Square *bs = board->squareAt(i, 7);
    Rook *bp = new Rook(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
  }

  //Knights
  for (int i = 1; i < DIM; i += 5) {
    Square *ws = board->squareAt(i, 0);
    Knight *wp = new Knight(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);

    Square *bs = board->squareAt(i, 7);
    Knight *bp = new Knight(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
  }

  //Bishops
  for (int i = 2; i < DIM; i += 3) {
    Square *ws = board->squareAt(i, 0);
    Bishop *wp = new Bishop(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);

    Square *bs = board->squareAt(i, 7);
    Bishop *bp = new Bishop(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
  }

  //Queens
  { 
    Square *ws = board->squareAt(4,0);
    Queen *wp = new Queen(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);

    Square *bs = board->squareAt(3,7);
    Queen *bp = new Queen(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
  }
  //Kings
  { 
    Square *ws = board->squareAt(3,0);
    King *wp = new King(ws, true);
    ws->moveInto(wp);
    whiteTeam->insert(wp);
    setKing(wp);

    Square *bs = board->squareAt(4,7);
    King *bp = new King(bs, false);
    bs->moveInto(bp);
    blackTeam->insert(bp);
    setKing(bp);
  }
}

/* Sees if the current team is facing a limited definition of checkmate.
 */
bool Game::inCheckmate() {
  King *k = whiteTurn ? whiteKing : blackKing;
  set<Piece *> *team = whiteTurn ? blackTeam : whiteTeam;
  bool inCheck = teamCanMove(k->getHome()->getPos(), team);
  if (!inCheck) return false;
  bool hasEscape = false;
  set<IntTuple> *kmoves = k->legalMoves();
  for (set<IntTuple>::iterator kit = kmoves->begin();
         kit != kmoves->end(); kit++) {
    if (!teamCanMove(&(*kit), team)) hasEscape = true;
  }
  return !hasEscape;
}

/* Sees if there is any piece on a given team that can move to a given
 * coordinate.
 */
bool teamCanMove(const IntTuple *pos, set<Piece *> *team) {
  for (set<Piece *>::iterator pit = team->begin();
         pit != team->end(); pit++) {
    Piece *p = *pit;
    set<IntTuple> *moves = p->legalMoves();
    if (moves->count(*pos) > 0) return true;
  }
  return false;
}

/* Asks for a move from standard input and validates it before returning
 * a valid move.
 */
Move *HumanPlayer::getMove() {
  char entry[255];
  cout << "Please enter a move in the form [a-h][1-8] [a-h][1-8]." << endl;
  cin.getline(entry, 255);
  if (strcmp(entry, "quit") == 0) exit(0);
  if (!validateMoveSyntax(entry)) {
    cout << "Invalid move: " << entry << endl;
    return getMove();
  }
  int x = entry[0] - 'a';
  int y = entry[1] - ('0' + 1);
  Square *sq = game->getBoard()->squareAt(x, y);
  
  if (sq->isEmpty()) {
    cout << "There is no piece in that square." << endl;
    return getMove();
  }
  if (sq->resident()->isWhite() != game->whitesTurn()) {
    cout << "You do not own that piece and thus cannot move it." << endl;
    return getMove();
  }

  int i = entry[3] - 'a';
  int j = entry[4] - ('0' + 1);
  set<IntTuple> *moves = sq->resident()->legalMoves();
  IntTuple *dest = new IntTuple(i, j);
  if (moves->count(*dest) <= 0) {
    cout << "That move is illegal for that piece." << endl;
    delete dest;
    return getMove();
  }

  return new Move(sq->resident(), dest);
}

/* Helper function for move validation.
 */
bool validateMoveSyntax(char entry[]) {
  if (strlen(entry) != 5) return false;
  return checkInRange(entry[0], 'a', 'h')
      && checkInRange(entry[1], '1', '8') && (' ' == entry[2])
      && checkInRange(entry[3], 'a', 'h')
      && checkInRange(entry[4], '1', '8');
}

/* Checks if the given integer falls in the given range.
 */
bool checkInRange(int val, int min, int max) {
  return (val >= min) && (val <= max);
}

/* Chooses a random move from a random piece, trying again if that piece
 * has no legal moves.
 */
Move *RandomPlayer::getMove() {
  set<Piece *> *team = game->getTeam(game->whitesTurn()); 
  int which = -1;
  set<IntTuple> *moves = 0;
  Piece *toMove;
  do {
    int which = rand() % team->size();
    set<Piece *>::iterator it = team->begin();
    advance(it, which);
    toMove = *it;
    moves = (*it)->legalMoves();

  } while (moves->size() == 0);
  which = rand() % moves->size();
  set<IntTuple>::iterator it = moves->begin();
  advance(it, which);
  IntTuple *dest = new IntTuple((*it).fst(), (*it).snd());
  Move *m = new Move(toMove, dest);
  return m;
}

/* Moves thie given piece to the given square,
 * capturing any occupant, and triggering game end if the move was 
 * a capture of a King. Returns true if the game should continue.
 */
bool Game::makeMove(Move *m) {
  Piece *p = m->who();
  bool kingIsCaptured = false;
  Square *fromSq = p->getHome();
  Square *toSq = board->squareAt(m->to()->fst(), m->to()->snd());
  if (!(toSq->isEmpty())) {
    Piece *captured = toSq->resident();
    set<Piece *> *team = getTeam(captured->isWhite());
    toSq->moveOutOf();
    team->erase(captured);
    kingIsCaptured = (captured->toString() == "K" || 
                      captured->toString() == "k");
    delete captured;
  }
  fromSq->moveOutOf();
  toSq->moveInto(p);
  int y = toSq->getPos()->snd();
  if ((p->toString() == "P" || p->toString() == "p")
       && (y == 0 || y == 7)) {
    Queen *q = new Queen(toSq, p->isWhite());
    set<Piece *> *myteam = getTeam(p->isWhite());
    myteam->erase(p);
    toSq->moveOutOf();
    delete p;
    myteam->insert(q);
    toSq->moveInto(q);
  }
  
  delete m;
  if (kingIsCaptured) {
      cout << "The King has been captured!" << endl;
      return false;
  }
  return true;
}

/* Plays the game. Until checkmate is reached,
 * takes moves and flips the turn. Returns true if 
 * White is the winner.
 */
bool Game::play() {
  bool mate = false;
  while (!inCheckmate()) {
    cout << *(getBoard());
    //In case the King is captured 
    if (!makeMove(whosTurn()->getMove())) return whiteTurn;
    whiteTurn = !whiteTurn;
  } 
  return !whiteTurn; 
}

