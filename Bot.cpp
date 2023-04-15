#include "Bot.h"
#include "PlaySide.h"

#include <bits/stdc++.h>

extern enum PlaySide getEngineSide();
extern enum PlaySide getSideToMove();

const std::string Bot::BOT_NAME = "El_Prostovano"; /* Edit this, escaped characters are forbidden */

std ::ofstream f("myLog.txt");

Bot::Bot() { /* Initialize custom fields here */
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
    /* You might find it useful to also separately
     * record last move in another custom field */
    f << "in recordMove function...\n";
    fflush(stdout);
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
