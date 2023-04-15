#ifndef BOT_H
#define BOT_H
#include <bits/stdc++.h>

#include "Move.h"
#include "PlaySide.h"

class Bot {
 private:
  static const std::string BOT_NAME;

 public:
  /* Declare custom fields below */

  /* Declare custom fields above */
  Bot();

  /**
   * Record move (either by enemy in normal mode, or by either side
   * in force mode) in custom structures
   * @param move received move
   * @param sideToMode side to move
   */
  void recordMove(Move* move, PlaySide sideToMove);

  /**
   * Calculates next move, in response to enemyMove
   * @param enemyMove the enemy's last move
   *                  null if this is the opening move, or previous
   *                  move has been recorded in force mode
   * @return your move
   */
  Move* calculateNextMove();

  static std::string getBotName();
};
#endif

struct PieceData {
  enum Piece type;
  union {
    bool moved = false;
    bool lastMoved;  // for En Passant, eventually
  };
  bool color;
  bool promoted = false;

  PieceData(enum Piece type, bool color, bool promoted) {
    this->type = type;
    this->color = color;
    this->promoted = promoted;
  }

  PieceData(enum Piece type, bool color) {
    PieceData(type, color, false);
  }

  PieceData() {
    PieceData(EMPTY, 0);
  }
};
