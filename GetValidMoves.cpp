#include "Bot.h"
#include "PlaySide.h"

extern std::ofstream f;

inline bool betweenBorders (int8_t x, int8_t y) {
  return x > 0 && x <= 8 && y > 0 && y <= 8;
}

void Bot::add(Move *m) {
  if (m != NULL) {
    Q.push(m);
    queueCount++;
  }
}

Move* Bot::checkDropIn(Table *tableStruc, int8_t col, int8_t row, enum Piece p) {
  SimpleTable currentTable = tableStruc->table;
  if (!betweenBorders(col, row) || currentTable[col][row].type != EMPTY)
    return NULL;
  
  if (p == PAWN && (row == 1 || row == 8))
    return NULL;

  Move *m = Move::dropIn(coordToStr(col, row), p);
  Table *table = Bot::createModifiedTable(m, *tableStruc);

  bool check = isCheck(*table);
  delete table;

  if (!check)
    return m;

  delete m;
  return NULL;
}

/* check whether the final position of a given move leaves the king attacked */
Move* Bot::checkPosition(Table *tableStruc, int8_t startCol, int8_t startRow,
             int8_t endCol, int8_t endRow, bool king = false) {
  SimpleTable table = tableStruc->table;
  if (betweenBorders(endCol, endRow)) {
    if (table[endCol][endRow].type == EMPTY ||
      table[endCol][endRow].color != getSideToMove()) {
      Move* m = Move::moveTo(coordToStr(startCol, startRow),
                   coordToStr(endCol, endRow));

      Table *newTable = Bot::createModifiedTable(m, *tableStruc);

      if (king) { // the piece that's moved is the king itself
        if (isCheck(*newTable, endCol, endRow) == false) {
          delete newTable;
          return m;
        }
      } else {
        if (isCheck(*newTable) == false) {
          delete newTable;
          return m;
        }
      }
      delete newTable;
      delete m;
    }
  }
  return NULL;
}

/* checks king's moves and returns whether a castling was found */
bool Bot::checkKingMoves(Table *tableStruc,int8_t col, int8_t row) {
  int offset_x[8] = { 1,  1, -1, -1,  0,  0, -1,  1};
  int offset_y[8] = { 1, -1,  1, -1,  1, -1,  0,  0};
  int i;

  SimpleTable currentTable = tableStruc->table;

  if (currentTable[col][row].moved == false && !isCheck(*tableStruc)) {
    if (currentTable[A][row].type == ROOK &&
        currentTable[A][row].moved == false) { // long castling
      if (currentTable[B][row].type==EMPTY &&
          currentTable[C][row].type==EMPTY &&
          currentTable[D][row].type==EMPTY) {
        Move *m = checkPosition(tableStruc, col, row, col - 2, row, true);
        Move *m1 = checkPosition(tableStruc, col, row, col - 3, row, true);
        Move *m2 = checkPosition(tableStruc, col, row, col - 1, row, true);
        delete m1;
        delete m2;
        if (m1 && m2 && m) {
          add(m);
          return true; // force castling
        }
        delete m;
      }
    }

    if (currentTable[H][row].type == ROOK &&
        currentTable[H][row].moved == false) { // short castling
      if (currentTable[F][row].type==EMPTY &&
          currentTable[G][row].type==EMPTY) {
        Move *m = checkPosition(tableStruc, col, row, col + 2, row, true);
        Move *m1 = checkPosition(tableStruc, col, row, col + 1, row, true);
        delete m1;
        if (m1 && m) {
          add(m);
          return true; // force castling
        }
        delete m;
      }
    }
  }

  for (i = 0; i < 8; i++)
    add(checkPosition(tableStruc, col, row, col + offset_x[i], row + offset_y[i], true));

  return false; // no castling found, so it's not forced
}

/* check possible move for PAAAWN - NOT SIGMA PIECE */
void Bot::checkPawnMoves(Table *tableStruc, int8_t col, int8_t row) {
  int8_t nextRow, nextNextRow, firstRow;

  SimpleTable currentTable = tableStruc->table;

  switch ((int8_t)(currentTable[col][row].color)) {
    case BLACK:
      nextRow = row - 1;
      nextNextRow = row - 2;
      firstRow = 7;
      break;

    case WHITE:
      nextRow = row + 1;
      nextNextRow = row + 2;
      firstRow = 2;
      break;
    default:
      f << "Invalid piece color\n";
      exit(1);
  }

  Move *m;

  /* just one basic step ahead */
  if (betweenBorders(col, nextRow) &&
    currentTable[col][nextRow].type == EMPTY) {
    m = checkPosition(tableStruc, col, row, col, nextRow);
    if (m != NULL && (nextRow == 1 || nextRow == 8)) {
      add(Move::promote(m->getSource(), m->getDestination(), QUEEN));
      add(Move::promote(m->getSource(), m->getDestination(), KNIGHT));
      delete m;
    } else add(m);
    /* 2 steps ahead*/
    if (row == firstRow && betweenBorders(col, nextNextRow) &&
      currentTable[col][nextNextRow].type == EMPTY)
      add(checkPosition(tableStruc, col, row, col, nextNextRow));
  }

  for (int i = -1; i <= 1; i+=2) // left and right diagonal
    if (betweenBorders(col - i, nextRow)) {
      if ((currentTable[col - i][nextRow].type != EMPTY &&
          currentTable[col - i][nextRow].color != getSideToMove()) ||
          (currentTable[col - i][nextRow].type == EMPTY &&
          currentTable[col - i][row].enPassantEligible)) {
        m = checkPosition(tableStruc, col, row, col - i, nextRow);
        if (m != NULL && (nextRow == 1 || nextRow == 8)) {
          add(Move::promote(m->getSource(), m->getDestination(), QUEEN));
          add(Move::promote(m->getSource(), m->getDestination(), KNIGHT));
          delete m;
        } else add(m);
      }
    }
}

/* check possible moves for BISHOP */
void Bot::checkBishopMoves(Table *tableStruc, int8_t col, int8_t row) {
  SimpleTable currentTable = tableStruc->table;
   /* up right */
  for (int i = col + 1, j = row + 1; betweenBorders(i ,j); ++i, ++j) {
    add(checkPosition(tableStruc, col, row, i, j));
    if (currentTable[i][j].type != EMPTY)
      break;
  }

   /* down right */
  for (int i = col + 1, j = row - 1; betweenBorders(i ,j); ++i, --j) {
    add(checkPosition(tableStruc, col, row, i, j));
    if (currentTable[i][j].type != EMPTY)
      break;
  }

   /* up left */
  for (int i = col - 1, j = row + 1; betweenBorders(i ,j); --i, ++j) {
    add(checkPosition(tableStruc, col, row, i, j));
    if (currentTable[i][j].type != EMPTY)
      break;
  }

   /* down left */
  for (int i = col - 1, j = row - 1; betweenBorders(i ,j); --i, --j) {
    add(checkPosition(tableStruc, col, row, i, j));
    if (currentTable[i][j].type != EMPTY)
      break;
  }
}

/* check moves for KNIGHT */
void Bot::checkKnightMoves(Table *tableStruc, int8_t col, int8_t row) {
  add(checkPosition(tableStruc, col, row, col + 2, row + 1));
  add(checkPosition(tableStruc, col, row, col - 2, row + 1));
  add(checkPosition(tableStruc, col, row, col + 2, row - 1));
  add(checkPosition(tableStruc, col, row, col - 2, row - 1));
  add(checkPosition(tableStruc, col, row, col + 1, row + 2));
  add(checkPosition(tableStruc, col, row, col - 1, row + 2));
  add(checkPosition(tableStruc, col, row, col + 1, row - 2));
  add(checkPosition(tableStruc, col, row, col - 1, row - 2));
}

/* check moves for THE ROOOOOOOK */
void Bot::checkRookMoves(Table *tableStruc, int8_t col, int8_t row) {
  SimpleTable currentTable = tableStruc->table;
  /* check left - if black, else check right*/
  for (int i = col + 1; i <= H; i++) {
    add(checkPosition(tableStruc, col, row, i, row));
    if (currentTable[i][row].type != EMPTY)
      break;
  }

  /* check right - if black, else check left */
  for (int i = col - 1; i >= A; i--) {
    add(checkPosition(tableStruc, col, row, i, row));
    if (currentTable[i][row].type != EMPTY)
      break;
  }

  /* check down - if black, else check up */
  for (int i = row + 1; i <= 8; i++) {
    add(checkPosition(tableStruc, col, row, col, i));
    if (currentTable[col][i].type != EMPTY)
      break;
  }

  /* check up - if black, else check down */
  for (int i = row - 1; i >= 1; i--) {
    add(checkPosition(tableStruc, col, row, col, i));
    if (currentTable[col][i].type != EMPTY)
      break;
  }
}

bool Bot::isCheck(Table crtTable) {
  int8_t king_x = crtTable.kingPos[getSideToMove()].first;
  int8_t king_y = crtTable.kingPos[getSideToMove()].second;
  return isCheck(crtTable, king_x, king_y);
}

/*
 * Given a table and the position of the king of the side to move,
 * check whether said king is attacked.
 */
bool Bot::isCheck(Table tableStruc, int8_t king_x, int8_t king_y) {

  int8_t x, y;

  SimpleTable crtTable = tableStruc.table;

  /* check for knights */
  int offset_x[8] = {-2, -1,  1,  2,  2,  1, -1, -2};
  int offset_y[8] = { 1,  2,  2,  1, -1, -2, -2, -1};

  int i;
  for (i = 0; i < 8; i++) {
    x = king_x + offset_x[i];
    y = king_y + offset_y[i];

    if (betweenBorders(x, y)) {
      if (crtTable[x][y].color != getSideToMove() &&
        crtTable[x][y].type == KNIGHT) {
          return true;
        }
    }
  }

  /* check for pawns */
  int offset_x_pawn[2];
  int offset_y_pawn[2];
  
  if (getSideToMove() == BLACK) {
    offset_x_pawn[0] = 1;
    offset_x_pawn[1] = -1;
    offset_y_pawn[0] = -1;
    offset_y_pawn[1] = -1; 
  } else {
    offset_x_pawn[0] = 1;
    offset_x_pawn[1] = -1;
    offset_y_pawn[0] = 1;
    offset_y_pawn[1] = 1;
  }

  for (i = 0; i < 2; i++) {
    x = king_x + offset_x_pawn[i];
    y = king_y + offset_y_pawn[i];

    if (betweenBorders(x, y)) {
      if (crtTable[x][y].color != getSideToMove() &&
        crtTable[x][y].type == PAWN) {
        return true;
      }
    }
  }

  /* check for bishop / queen */
  int offset_x_bq[4] = {1, 1, -1, -1};
  int offset_y_bq[4] = {1, -1, -1, 1};

  for (i = 0; i < 4; i++) {
    x = king_x + offset_x_bq[i];
    y = king_y + offset_y_bq[i];

    while (betweenBorders(x, y)) {
      if (crtTable[x][y].color != getSideToMove()) {
        if (crtTable[x][y].type == BISHOP ||
          crtTable[x][y].type == QUEEN) {
            return true;
          }
      }

      /*
       * if we reach this statement, we're sure that the
       * current piece is not a bishop / queen of other colour
       */
      if (crtTable[x][y].type != EMPTY) {
        /* protected by other piece */
        break;
      }

      x = x + offset_x_bq[i];
      y = y + offset_y_bq[i];
    }
  }
  
  /* check for rook / queen */
  int offset_x_rq[4] = {0, -1, 0, 1};
  int offset_y_rq[4] = {-1, 0, 1, 0};

  for (i = 0; i < 4; i++) {
    x = king_x + offset_x_rq[i];
    y = king_y + offset_y_rq[i];

    while (betweenBorders(x, y)) {
      if (crtTable[x][y].color != getSideToMove()) {
        if (crtTable[x][y].type == ROOK ||
          crtTable[x][y].type == QUEEN) {
            return true;
          }
      }

      if (crtTable[x][y].type != EMPTY) {
        /* protected by other piece */
        break;
      }

      x = x + offset_x_rq[i];
      y = y + offset_y_rq[i];
    }
  }

  /* check for other king */
  if ((abs(king_x - tableStruc.kingPos[1 ^ getSideToMove()].first) <= 1) &&
      (abs(king_y - tableStruc.kingPos[1 ^ getSideToMove()].second) <= 1))
    return true;

  return false;
}
