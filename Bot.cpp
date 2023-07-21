#include "Bot.h"
#include "PlaySide.h"
#include <bits/stdc++.h>

#define MAX_DEPTH 3
#define INF (INT32_MAX / 2)
#define CHECK_BONUS 5
// enum Piece { PAWN = 0, ROOK = 1, BISHOP = 2, KNIGHT = 3, QUEEN = 4, KING = 5, EMPTY = 6 };
int         values[] = { 1,  5,  3,  3,  9, 90,  0};
int capturedValues[] = { 1,  5,  5, 10, 10, 90,  0}; // myVersion

extern std::ofstream f;
extern std::string serializeMove(Move *m);

const std::string Bot::BOT_NAME = "The_Escapists";

std::vector <std::pair<int8_t, int8_t>> kingPos(2);

enum Column getCol(std::__cxx11::basic_string<char> s) {
  return (enum Column)(s.at(0) - 'a' + 1);
}

int8_t getRow(std::__cxx11::basic_string<char> s) {
  return (int8_t)(s.at(1) - '0');
}

std::optional<std::string> coordToStr(int8_t x, int8_t y) {
  char s[3];
  s[0] = x - 1 + 'a';
  s[1] = y + '0';
  s[2] = 0;

  return std::optional{s};
}

void Bot::printTable() {
  printTable(&currentTable);
}
void Bot::printTable(Table *tableStruc) {
  char pieceChar[] = "PRBNQK ";
  SimpleTable *table = &(tableStruc->table);
  for (int i = A; i <= H; ++i) {
    f << '|';
    for (int j = 1; j <= 8; ++j) {
      if ((*table)[i][j].type == EMPTY) {
          f << "  |";
          continue;
      };
      f << pieceChar[(int)((*table)[i][j].type)]
        << ((*table)[i][j].color?'w':'b') << '|';
    }
    f << '\n';
  }
  f << "\n\n";
}

Bot::Bot() {
  srand(time(0));
  f << "Before constructing table\n\n";
  currentTable.table = std::vector(N + 1, std::vector <PieceData>(N + 1, PieceData()));

  for (int i = 0; i < 6; ++i)
    currentTable.captured[0][i] = currentTable.captured[1][i] = 0;

  /* Add pawns */
  for (int i = A; i <= H; ++i) {
    currentTable.table[i][2] = PieceData(PAWN, WHITE);
    currentTable.table[i][7] = PieceData(PAWN, BLACK);
  }

  /* Add ROOOOOOOOOOOOOOOOOOOOOOOOOOOOK */
  currentTable.table[H][8] = currentTable.table[A][8] = PieceData(ROOK, BLACK);
  currentTable.table[H][1] = currentTable.table[A][1] = PieceData(ROOK, WHITE);

  /* Add Bishops */
  currentTable.table[C][8] = currentTable.table[F][8] = PieceData(BISHOP, BLACK);
  currentTable.table[C][1] = currentTable.table[F][1] = PieceData(BISHOP, WHITE);
  
  /* Add Knights */
  currentTable.table[B][1] = PieceData(KNIGHT, WHITE);
  currentTable.table[G][1] = PieceData(KNIGHT, WHITE);
  
  currentTable.table[B][8] = PieceData(KNIGHT, BLACK);
  currentTable.table[G][8] = PieceData(KNIGHT, BLACK);

  /* Add Queens */
  currentTable.table[D][8] = PieceData(QUEEN, BLACK);
  currentTable.table[D][1] = PieceData(QUEEN, WHITE);

  /* Add Kings */
  currentTable.table[E][8] = PieceData(KING, BLACK);
  currentTable.table[E][1] = PieceData(KING, WHITE);
  currentTable.kingPos.push_back(std::pair<int8_t, int8_t>(E, 8));
  currentTable.kingPos.push_back(std::pair<int8_t, int8_t>(E, 1));

  f << "Initial table:\n";
  printTable();
}

Table* Bot::createModifiedTable(Move* move, Table table) {
  Table *ret = new Table;
  *ret = table;
  Bot::recordMove(move, currentSide, ret, false);
  return ret;
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  recordMove(move, sideToMove, &currentTable);
}

void Bot::recordMove(Move* move, PlaySide sideToMove,
                     Table *tableStruc, bool updateAux) {
  SimpleTable *table = &(tableStruc->table);
  KingPos kingPos = tableStruc->kingPos;
  /* You might find it useful to also separately
    * record last move in another custom field */
  // f << "\n" << (sideToMove == BLACK ? "BLACK":"WHITE") << " moves\n";

  std::__cxx11::basic_string<char> src;
  std::__cxx11::basic_string<char> dest = *(move->getDestination());
  PieceData *pieceDest = &((*table)[getCol(dest)][getRow(dest)]);

  if (pieceDest->type != EMPTY && updateAux) {
    if (pieceDest->promoted == true) {
      (tableStruc->captured)[sideToMove][PAWN]++;
    } else {
      (tableStruc->captured)[sideToMove][pieceDest->type]++;
    }
  }

  if (move->isNormal() || move->isPromotion()) {
    src = *(move->getSource());
    PieceData *pieceSrc = &((*table)[getCol(src)][getRow(src)]);

    *pieceDest = *pieceSrc;
    pieceDest->moved = true;
    pieceSrc->type = EMPTY;

    if (move->isPromotion()) {
      pieceDest->type = *(move->getReplacement());
      pieceDest->promoted = true;
    } else if (pieceDest->type == KING) {  // testare rocada + actualizare kingPos
      int8_t kingRow;
      switch(sideToMove) {
        case BLACK:
          kingRow = 8;
          break;
        case WHITE:
          kingRow = 1;
          break;
        default:
          kingRow = 0;
          break;
      }

      if (getCol(src) == E && getRow(src) == kingRow) {
        if (getCol(dest) == G && getRow(dest) == kingRow) {  // rocada mica
          (*table)[F][kingRow] = (*table)[H][kingRow];
          (*table)[F][kingRow].moved = true;
          (*table)[H][kingRow].type = EMPTY;
        } else if (getCol(dest) == C && getRow(dest) == kingRow) {  // rocada mare
          (*table)[D][kingRow] = (*table)[A][kingRow];
          (*table)[D][kingRow].moved = true;
          (*table)[A][kingRow].type = EMPTY;
        }
      }

      /* Update kingPos */
      if (updateAux) {
        tableStruc->kingPos[sideToMove].first = getCol(dest);
        tableStruc->kingPos[sideToMove].second = getRow(dest);
      }

    } else if (pieceDest->type == PAWN) {  // testare En Passant
      PieceData *pieceBehind;
      switch(sideToMove) {
        case BLACK:
          pieceBehind = &((*table)[getCol(dest)][getRow(dest) + 1]);
          break;
        case WHITE:
          pieceBehind = &((*table)[getCol(dest)][getRow(dest) - 1]);
          break;
        default:
          break;
      }

      if (pieceBehind->enPassantEligible == true && pieceBehind->color != sideToMove) {
        if (updateAux)
          (tableStruc->captured)[sideToMove][PAWN]++;
        pieceBehind->type = EMPTY;
      }
    }
  } else if (move->isDropIn()) {
    *pieceDest = PieceData(*(move->getReplacement()), sideToMove, false);
    pieceDest->moved = true;
    if (updateAux)
      (tableStruc->captured)[sideToMove][pieceDest->type]--;
  }

  for (int i = A; i <= H; ++i)
    for (int j = 1; j <= 8; ++j)
      (*table)[i][j].enPassantEligible = false;

  if (pieceDest->type == PAWN && move->isNormal())  // check En Passant Eligibility
    if (getCol(src) == getCol(dest)) {
      int8_t startRow, endRow;
      switch(sideToMove) {
        case BLACK:
          startRow = 7;
          endRow = 5;
          break;
        case WHITE:
          startRow = 2;
          endRow = 4;
          break;
        default:
          break;
      }

    if (getRow(src) == startRow && getRow(dest) == endRow) {
      pieceDest->enPassantEligible = true;
    }
  }

  // printTable(tableStruc);
}


Move* Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */
  std::pair<Move*, int> p = Bot::calculateNextMove(&currentTable, getSideToMove(), 0);

  if (p.first == NULL)
    return Move::resign();

  recordMove(p.first, getSideToMove());
  return p.first;
}

std::pair<Move*, int> Bot::calculateNextMove(Table *tableStruc, bool sideToMove, int depth, int alpha, int beta) {
  // bool rocade = false;
  // queueCount = 0;
  std::queue<Move*> Q;
  // int queueCount = 0;

  if (depth == MAX_DEPTH)
    return std::pair<Move*, int>(NULL, Bot::heuristic(tableStruc, sideToMove));

  currentSide = (enum PlaySide)sideToMove;

  for (int i = A; i <= H; ++i) {
    for (int j = 1; j <= 8; ++j) {
      if (tableStruc->table[i][j].color == sideToMove &&
          tableStruc->table[i][j].type != EMPTY) {
        switch (tableStruc->table[i][j].type) {
          case PAWN:
            Bot::checkPawnMoves(tableStruc, i, j, Q);
            break;
          case KNIGHT:
            Bot::checkKnightMoves(tableStruc, i, j, Q);
            break;
          case QUEEN:
            Bot::checkBishopMoves(tableStruc, i, j, Q);
            Bot::checkRookMoves(tableStruc, i, j, Q);
            break;
          case KING:
            Bot::checkKingMoves(tableStruc, i, j, Q);
            break;
          case ROOK:
            Bot::checkRookMoves(tableStruc, i, j, Q);
            break;
          case BISHOP:
            Bot::checkBishopMoves(tableStruc, i, j, Q);
            break;
          default:
            continue;
        };
      
        // if (rocade) {// force rocade
        //   Move *m = NULL;
        //   while (!Q.empty()) {
        //     delete m;
        //     m = Q.front();
        //     Q.pop();
        //   }
        //   Q.push(m);
        //   queueCount = 1;
        //   break;
        // }
      } else if (tableStruc->table[i][j].type == EMPTY) {
        for (int p = PAWN; p <= QUEEN; ++p)
            if (tableStruc->captured[sideToMove][p] != 0)
              add(checkDropIn(tableStruc, i, j, (enum Piece)p), Q);
      }
    }
  }

  // /* shit random number getter */
  // if (Q.empty()) {
  //   return Move::resign();
  // }
  // Move *m1;
  // int move_index = (rand() % queueCount); 
  // int count = 0;

  // while (!Q.empty()) {
  //   Move *m = Q.front();
  //   if (count == move_index) 
  //     m1 = m;
  //   count++;
  //   f << serializeMove(m) << ", ";
  //   Q.pop();
  // }

  // f << "\n------------\nCaptured:\n";
  // for (int i = BLACK; i <= WHITE; ++i) {
  //   f << (enum Piece)i <<" - ";
  //   for (int p = PAWN; p <= QUEEN; ++p)
  //     f << (int)(currentTable.captured[i][p]) << ' ';
    
  //   f << '\n';
  // }

  // f << "------------\nChosen move: " << serializeMove(m1) << " Pozitia regelui: ";
  // f << (int)currentTable.kingPos[getSideToMove()].first << " " << (int)currentTable.kingPos[getSideToMove()].second << ' ';
  // f << getSideToMove() << '\n';


  std::pair<Move*, int> best = std::pair<Move*, int>(NULL, -INF);

  while (!Q.empty()) {
    Move *m = Q.front();
    Q.pop();

    Table *newTable = createModifiedTable(m, *tableStruc);

    std::pair<Move*, int> p = calculateNextMove(newTable, 1 ^ sideToMove, depth + 1);
    int score = - p.second;

    if (score > best.second || (score == best.second && rand() % 2)) {
      delete best.first;
      best.first = m;
      best.second = score;
    } else {
      delete m;
    }

    delete newTable;
    delete p.first;
  }

  currentSide = (enum PlaySide)(1 ^ sideToMove);

  if (isCheck(*tableStruc, tableStruc->kingPos[1 ^ sideToMove].first, tableStruc->kingPos[1 ^ sideToMove].second))
    best.second += CHECK_BONUS;

  return best;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

int Bot::heuristic(Table *tableStruc, bool sideToMove) {
  int score = 0;
  for (int i = A; i <= H; ++i) {
    for (int j = 1; j <= 8; ++j) {
      if (tableStruc->table[i][j].color == sideToMove)
        score += values[tableStruc->table[i][j].type];
      else
        score -= values[tableStruc->table[i][j].type];
    }
  }

  for (int i = PAWN; i <= QUEEN; ++i) {
    score += capturedValues[i] * tableStruc->captured[sideToMove][i];
    score -= capturedValues[i] * tableStruc->captured[1 ^ sideToMove][i];
  }


  // printTable(tableStruc);
  // f << "\n---------------  Score: " << score << "  ---------------\n";

  return score;
}
