#include "Bot.h"
#include "PlaySide.h"

#include <bits/stdc++.h>

#define N 8

enum Column { Z, A, B, C, D, E ,F, G, H };

extern enum PlaySide getEngineSide();
extern enum PlaySide getSideToMove();

const std::string Bot::BOT_NAME = "El_Prostovano"; /* Edit this, escaped characters are forbidden */

std ::ofstream f("myLog.txt");
std :: vector <std :: vector <PieceData> > table(N + 1, std :: vector <PieceData>(N + 1, PieceData()));
int8_t captured[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

enum Column getCol(std::__cxx11::basic_string<char> s) {
  return (enum Column)(s.at(0) - 'a' + 1);
}

int8_t getRow(std::__cxx11::basic_string<char> s) {
  return (int8_t)(s.at(1) - '0');
}

void printTable() {
  for (int i = A; i <= H; ++i) {
    f << '|';

    for (int j = 1; j <= 8; ++j) {
      char ch;
      switch (table[i][j].type) {
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
          f << "  |";
          continue;
      };
      f << ch << (table[i][j].color?'w':'b') << '|';
    }
    f << '\n';
  }
  f << "\n\n";
}

Bot::Bot() {
  /* Add pawns */
  for (int i = A; i <= H; ++i) {
    table[i][2] = PieceData(PAWN, WHITE);
    table[i][7] = PieceData(PAWN, BLACK);
  }

  /* Add ROOOOOOOOOOOOOOOOOOOOOOOOOOOOK */
  table[H][8] = table[A][8] = PieceData(ROOK, BLACK);
  table[H][1] = table[A][1] = PieceData(ROOK, WHITE);

  /* Add Bishops */
  table[C][8] = table[F][8] = PieceData(BISHOP, BLACK);
  table[C][1] = table[F][1] = PieceData(BISHOP, WHITE);
  
  /* Add Knights */
  table[B][1] = PieceData(KNIGHT, WHITE);
  table[G][1] = PieceData(KNIGHT, WHITE);
  
  table[B][8] = PieceData(KNIGHT, BLACK);
  table[G][8] = PieceData(KNIGHT, BLACK);

  /* Add Queens */
  table[D][8] = PieceData(QUEEN, BLACK);
  table[D][1] = PieceData(QUEEN, WHITE);

  /* Add Kings */
  table[E][8] = PieceData(KING, BLACK);
  table[E][1] = PieceData(KING, WHITE);

  f << "Initial table:\n";
  printTable();
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  /* You might find it useful to also separately
    * record last move in another custom field */
  f << "\n" << (sideToMove == BLACK ? "BLACK":"WHITE") << " moves\n";

  std::__cxx11::basic_string<char> src;
  std::__cxx11::basic_string<char> dest = *(move->getDestination());
  PieceData *pieceDest = &(table[getCol(dest)][getRow(dest)]);

  if (pieceDest->type != EMPTY) {
    if (pieceDest->promoted == true) {
      captured[sideToMove][PAWN]++;
    } else {
      captured[sideToMove][pieceDest->type]++;
    }
  }

  if (move->isNormal() || move->isPromotion()) {
    src = *(move->getSource());
    PieceData *pieceSrc = &(table[getCol(src)][getRow(src)]);

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
          table[F][kingRow] = table[H][kingRow];
          table[F][kingRow].moved = true;
          table[H][kingRow].type = EMPTY;
        } else if (getCol(dest) == C && getRow(dest) == kingRow) {  // rocada mare
          table[D][kingRow] = table[A][kingRow];
          table[D][kingRow].moved = true;
          table[A][kingRow].type = EMPTY;
        }
      }
    } else if (pieceDest->type == PAWN) {  // testare En Passant
      PieceData *pieceBehind;
      switch(sideToMove) {
        case BLACK:
          pieceBehind = &(table[getCol(dest)][getRow(dest) + 1]);
          break;
        case WHITE:
          pieceBehind = &(table[getCol(dest)][getRow(dest) - 1]);
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
    captured[sideToMove][pieceDest->type]--;
  }

  for (int i = A; i <= H; ++i)
    for (int j = 1; j <= 8; ++j)
      table[i][j].enPassantEligible = false;

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

  printTable();
}

Move* Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */

  f<< "in calculateNextMove function...\n";

  if (getEngineSide() == getSideToMove()) {
    
    f << "engine side == side to move\n";
    fflush(stdout);

    if (getEngineSide() == WHITE) {
      return Move::moveTo("a2", "a4");
    } else {
      return Move::moveTo("h7", "h5");
    }
  } else {
    f << "Wtf happened here? (engine side != side to move)\n";
  }

  fflush(stdout);
  return Move::resign();
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }
