#ifndef BOT_H
#define BOT_H
#include <bits/stdc++.h>

#include "Move.h"
#include "PlaySide.h"

#define N 8

extern enum PlaySide getEngineSide();
extern enum PlaySide getSideToMove();

enum Column {Z, A, B, C, D, E ,F, G, H};

std::optional<std::string> coordToStr(int8_t x, int8_t y);

struct PieceData {
  enum Piece type = EMPTY;
  bool color = BLACK;
  bool promoted = false;
  bool moved = false;
  bool enPassantEligible = false;  // for En Passant

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

typedef std::vector <std::vector <PieceData> > SimpleTable;
typedef std::vector <std::pair <int8_t, int8_t>> KingPos;

struct Table {
  SimpleTable table;
  int8_t captured[2][6];
  KingPos kingPos;
};


class Bot {
 private:
  static const std::string BOT_NAME;

 public:
  /* Declare custom fields below */
  std::queue<Move*> Q;
  int queueCount = 0;

  Table currentTable;
  // int8_t captured[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
  // std::vector <std::pair <int8_t, int8_t> > kingPos;

  /* Declare custom fields above */
  Bot();

  /**
   * Record move (either by enemy in normal mode, or by either side
   * in force mode) in custom structures
   * @param move received move
   * @param sideToMode side to move
   */

  void recordMove(Move* move, PlaySide sideToMove);
  void recordMove(Move* move, PlaySide sideToMove,
                  Table *table, bool updateAux = true);

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
  Move* checkDropIn(Table *tableStruc, int8_t col, int8_t row, enum Piece p);
  Move* checkPosition(Table *tableStruc, int8_t col, int8_t row, int8_t i, int8_t j, bool king);
  void checkPawnMoves(Table *tableStruc, int8_t col, int8_t row);
  void checkBishopMoves(Table *tableStruc, int8_t col, int8_t row);
  void checkKnightMoves(Table *tableStruc, int8_t col, int8_t row);
  void checkRookMoves(Table *tableStruc, int8_t col, int8_t row);
  bool checkKingMoves(Table *tableStruc, int8_t col, int8_t row);
  bool isCheck(Table crtTable);
  bool isCheck(Table crtTable, int8_t king_x, int8_t king_y);

  static std::string getBotName();
};

#endif