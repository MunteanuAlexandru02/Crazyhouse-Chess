#ifndef BOT_H
#define BOT_H
#include <bits/stdc++.h>

#include "Move.h"
#include "PlaySide.h"

#define N 8

extern enum PlaySide getEngineSide();
extern enum PlaySide getSideToMove();
extern std::queue<Move*> Q;

enum Column { Z, A, B, C, D, E ,F, G, H };

std::optional<std::string> coordToStr(int8_t x, int8_t y);

struct PieceData {
  enum Piece type = EMPTY;
  bool color = BLACK;
  bool promoted = false;
  bool moved = false;
  bool enPassantEligible = false;  // for En Passant, eventually

  PieceData(enum Piece argtype, bool argcolor, bool argpromoted):
    type(argtype), color(argcolor), promoted(argpromoted)
  {}

  PieceData(enum Piece argtype, bool argcolor):
    type(argtype), color(argcolor)
  {}

  PieceData():
    type(EMPTY)
  {}
};

typedef std :: vector <std :: vector <PieceData> > Table;

class Bot {
 private:
  static const std::string BOT_NAME;

 public:
  /* Declare custom fields below */

  Table currentTable;
  int8_t captured[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
  std :: vector <std :: pair <int8_t, int8_t> > kingPos;

  /* Declare custom fields above */
  Bot();

  /**
   * Record move (either by enemy in normal mode, or by either side
   * in force mode) in custom structures
   * @param move received move
   * @param sideToMode side to move
   */

  void recordMove(Move* move, PlaySide sideToMove);
  void recordMove(Move* move, PlaySide sideToMove, Table *table);

  Table *createModifiedTable(Move* move, Table table);

  /**
   * Calculates next move, in response to enemyMove
   * @param enemyMove the enemy's last move
   *                  null if this is the opening move, or previous
   *                  move has been recorded in force mode
   * @return your move
   */
  Move* calculateNextMove();
  void printTable();
  void printTable(Table *table);

  void add(Move *m);
  Move* checkPosition(int8_t col, int8_t row, int8_t i, int8_t j, bool type);
  void checkPawnMoves(int8_t col, int8_t row);
  void checkBishopMoves(int8_t col, int8_t row);
  void checkKnightMoves(int8_t col, int8_t row);
  void checkRookMoves(int8_t col, int8_t row);
  bool isCheck(Table crtTable);
  bool isCheck(Table crtTable, int8_t king_x, int8_t king_y);

  static std::string getBotName();
};

#endif