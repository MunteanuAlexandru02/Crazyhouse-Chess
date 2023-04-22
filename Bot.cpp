#include "Bot.h"
#include "PlaySide.h"

#include <bits/stdc++.h>

// std::ofstream f("myLog.txt");
extern std :: ofstream f;


const std::string Bot::BOT_NAME = "El_Prostovano"; /* Edit this, escaped characters are forbidden */

std :: vector <std :: pair<int8_t, int8_t>> kingPos(2);

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
void Bot::printTable(Table *table) {
  for (int i = A; i <= H; ++i) {
    f << '|';

    for (int j = 1; j <= 8; ++j) {
      char ch;
      switch ((*table)[i][j].type) {
        case PAWN:
          ch = 'P';
          break;
        case KNIGHT:
          ch = 'N';
          break;
        case QUEEN:
          ch = 'Q';
          break;
        case KING:
          ch = 'K';
          break;
        case ROOK:
          ch = 'R';
          break;
        case BISHOP:
          ch = 'B';
          break;
        default:
          ch = ch;
          f << "  |";
          continue;
      };
      f << ch << ((*table)[i][j].color?'w':'b') << '|';
    }
    f << '\n';
  }
  f << "\n\n";
}

Bot::Bot() {
  f << "Before constructing table\n\n";
  currentTable = std :: vector(N + 1, std :: vector <PieceData>(N + 1, PieceData()));
  //f << "After constructing table\n\n";

  /* Add pawns */
  for (int i = A; i <= H; ++i) {
    currentTable[i][2] = PieceData(PAWN, WHITE);
    currentTable[i][7] = PieceData(PAWN, BLACK);
  }

  /* Add ROOOOOOOOOOOOOOOOOOOOOOOOOOOOK */
  currentTable[H][8] = currentTable[A][8] = PieceData(ROOK, BLACK);
  currentTable[H][1] = currentTable[A][1] = PieceData(ROOK, WHITE);

  /* Add Bishops */
  currentTable[C][8] = currentTable[F][8] = PieceData(BISHOP, BLACK);
  currentTable[C][1] = currentTable[F][1] = PieceData(BISHOP, WHITE);
  
  /* Add Knights */
  currentTable[B][1] = PieceData(KNIGHT, WHITE);
  currentTable[G][1] = PieceData(KNIGHT, WHITE);
  
  currentTable[B][8] = PieceData(KNIGHT, BLACK);
  currentTable[G][8] = PieceData(KNIGHT, BLACK);

  /* Add Queens */
  currentTable[D][8] = PieceData(QUEEN, BLACK);
  currentTable[D][1] = PieceData(QUEEN, WHITE);

  /* Add Kings */
  currentTable[E][8] = PieceData(KING, BLACK);
  currentTable[E][1] = PieceData(KING, WHITE);
  kingPos.push_back(std :: pair<int8_t, int8_t>(E, 8));
  kingPos.push_back(std :: pair<int8_t, int8_t>(E, 1));

  f << "Initial table:\n";
  printTable();
}

Table* Bot::createModifiedTable(Move* move, Table table) {
  Table *ret = new Table;
  *ret = table; 
  Bot::recordMove(move, getSideToMove(), ret);
  return ret;
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  recordMove(move, sideToMove, &currentTable);
}

void Bot::recordMove(Move* move, PlaySide sideToMove, Table *table) {
  /* You might find it useful to also separately
    * record last move in another custom field */
  f << "\n" << (sideToMove == BLACK ? "BLACK":"WHITE") << " moves\n";

  std::__cxx11::basic_string<char> src;
  std::__cxx11::basic_string<char> dest = *(move->getDestination());
  PieceData *pieceDest = &((*table)[getCol(dest)][getRow(dest)]);

  if (pieceDest->type != EMPTY) {
    if (pieceDest->promoted == true) {
      captured[sideToMove][PAWN]++;
    } else {
      captured[sideToMove][pieceDest->type]++;
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
    } else if (pieceDest->type == KING) {  // testare rocada
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
        captured[sideToMove][PAWN]++;
        pieceBehind->type = EMPTY;
      }
    }
  } else if (move->isDropIn()) {
    *pieceDest = PieceData(*(move->getReplacement()), sideToMove, false);
    pieceDest->moved = true;
    captured[sideToMove][pieceDest->type]--;
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

  printTable(table);
}

Move* Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */

  //f << "in calculateNextMove function...\n";

  // if (getEngineSide() == getSideToMove()) {
    
  //   //f << "engine side == side to move\n";
  //   fflush(stdout);

  //   if (getEngineSide() == WHITE) {
  //     return Move::moveTo("a2", "a4");
  //   } else {
  //     return Move::moveTo("h7", "h5");
  //   }
  // } else {
  //   //f << "Wtf happened here? (engine side != side to move)\n";
  // }

  // fflush(stdout);

  for (int i = A; i <= H; ++i) {
    for (int j = 1; j <= 8; ++j) {
      if (currentTable[i][j].color == getSideToMove()) {
          switch (currentTable[i][j].type) {
          case PAWN:
            Bot::checkPawnMoves(i, j);
            break;
          case KNIGHT:
            Bot::checkKnightMoves(i, j);
            break;
          case QUEEN:
            Bot::checkRookMoves(i, j);
            Bot::checkBishopMoves(i, j);
            break;
          case KING:
            Bot::checkKingMoves(i, j);
            break;
          case ROOK:
            Bot::checkRookMoves(i, j);
            break;
          case BISHOP:
            Bot::checkBishopMoves(i, j);
            break;
          default:
          
            continue;
        };
      }
    }
  }

  Move *m1 = Q.front();

  while (!Q.empty()) {
    Move *m = Q.front();
    f << m->source.value() << "->" << m->destination.value() << ", ";
    Q.pop();
  }
  f << '\n';
  /* check if the king was moved - update kingPos */
  if (getCol(*(m1->getSource())) == kingPos[getSideToMove()].first &&
        getRow(*(m1->getSource())) == kingPos[getSideToMove()].second) {
          kingPos[getSideToMove()].first = getCol(*(m1->getDestination()));
          kingPos[getSideToMove()].second = getRow(*(m1->getDestination()));
        }

  recordMove(m1, getSideToMove());
  return m1;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }
