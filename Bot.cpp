#include "Bot.h"
#include "PlaySide.h"
#include <bits/stdc++.h>

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
  Q = std::queue<Move*>();
  queueCount = 0;

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
  Bot::recordMove(move, getSideToMove(), ret, false);
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
  f << "\n" << (sideToMove == BLACK ? "BLACK":"WHITE") << " moves\n";

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

  printTable(tableStruc);
}

Move* Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */
  bool rocade = false;
  queueCount = 0;

  for (int i = A; i <= H && !rocade; ++i) {
    for (int j = 1; j <= 8 && !rocade; ++j) {
      if (currentTable.table[i][j].color == getSideToMove() &&
          currentTable.table[i][j].type != EMPTY) {
        switch (currentTable.table[i][j].type) {
          case PAWN:
            Bot::checkPawnMoves(&currentTable, i, j);
            break;
          case KNIGHT:
            Bot::checkKnightMoves(&currentTable, i, j);
            break;
          case QUEEN:
            Bot::checkBishopMoves(&currentTable, i, j);
            Bot::checkRookMoves(&currentTable, i, j);
            break;
          case KING:
            rocade = Bot::checkKingMoves(&currentTable, i, j);
            break;
          case ROOK:
            Bot::checkRookMoves(&currentTable, i, j);
            break;
          case BISHOP:
            Bot::checkBishopMoves(&currentTable, i, j);
            break;
          default:
            continue;
        };
      
        if (rocade) {// force rocade
          Move *m = NULL;
          while (!Q.empty()) {
            delete m;
            m = Q.front();
            Q.pop();
          }
          Q.push(m);
          queueCount = 1;
          break;
        }
      } else if (currentTable.table[i][j].type == EMPTY) {
        for (int p = PAWN; p <= QUEEN; ++p)
            if (currentTable.captured[getSideToMove()][p] != 0)
              add(checkDropIn(&currentTable, i, j, (enum Piece)p));
      }
    }
  }

  /* shit random number getter */
  if (Q.empty()) {
    return Move::resign();
  }
  Move *m1;
  int move_index = (rand() % queueCount); 
  int count = 0;

  while (!Q.empty()) {
    Move *m = Q.front();
    if (count == move_index) 
      m1 = m;
    count++;
    f << serializeMove(m) << ", ";
    Q.pop();
  }

  f << "\n------------\nCaptured:\n";
  for (int i = BLACK; i <= WHITE; ++i) {
    f << (enum Piece)i <<" - ";
    for (int p = PAWN; p <= QUEEN; ++p)
      f << (int)(currentTable.captured[i][p]) << ' ';
    
    f << '\n';
  }

  f << "------------\nChosen move: " << serializeMove(m1) << " Pozitia regelui: ";
  f << (int)currentTable.kingPos[getSideToMove()].first << " " << (int)currentTable.kingPos[getSideToMove()].second << ' ';
  f << getSideToMove() << '\n';

  recordMove(m1, getSideToMove());
  return m1;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }
