#include "Bot.h"
#include "PlaySide.h"

std::queue<Move*> Q;
//Bot b;
extern std::ofstream f;

inline bool betweenBorders (int8_t x, int8_t y) {
    return x > 0 && x <= 8 && y > 0 && y <= 8;
}

void Bot::add(Move *m) {
    if (m != NULL)
        Q.push(m);
}

Move* Bot::checkDropIn(int8_t col, int8_t row, enum Piece p) {
    if (!betweenBorders(col, row) || currentTable[col][row].type != EMPTY)
        return NULL;
    
    if (p == PAWN && (row == 1 || row == 8))
        return NULL;

    Move *m = Move::dropIn(coordToStr(col, row), p);
    Table *table = Bot::createModifiedTable(m, currentTable);

    bool check = isCheck(*table);
    delete table;

    if (!check)
        return m;

    delete m;
    return NULL;
}

Move* Bot::checkPosition(int8_t col, int8_t row, int8_t i, int8_t j, bool king = false) {
    if (betweenBorders(i, j)) {
        if (currentTable[i][j].type == EMPTY || currentTable[i][j].color != getSideToMove()) {
            Move* m = Move::moveTo(coordToStr(col, row), coordToStr(i, j));
            Table *table = Bot::createModifiedTable(m, currentTable);
            if (king) {
                if (isCheck(*table, i, j) == false) {
                    delete table;
                    return m;
                }
            } else {
                if (isCheck(*table) == false) {
                    delete table;
                    return m;
                }
            }
            delete table;
            delete m;
        }
    }
    return NULL;
}

bool Bot::checkKingMoves(int8_t col, int8_t row) {
    int offset_x[8] = { 1,  1, -1, -1,  0,  0, -1,  1};
    int offset_y[8] = { 1, -1,  1, -1,  1, -1,  0,  0};
    int i;

    if (currentTable[col][row].moved == false && !isCheck(currentTable)) {
        if (currentTable[A][row].type == ROOK && currentTable[A][row].moved == false) {// rocada mare
            if (currentTable[B][row].type==EMPTY &&
                currentTable[C][row].type==EMPTY && currentTable[D][row].type==EMPTY) {
                Move *m = checkPosition(col, row, col - 2, row, true);
                Move *m1 = checkPosition(col, row, col - 3, row, true);
                Move *m2 = checkPosition(col, row, col - 1, row, true);
                delete m1;
                delete m2;
                if (m1 && m2 && m) {
                    add(m);
                    return true; // se forteaza rocada
                }
                delete m;
            }
        }
        if (currentTable[H][row].type == ROOK && currentTable[H][row].moved == false && !isCheck(currentTable)) { // rocada mica
            if (currentTable[F][row].type==EMPTY && currentTable[G][row].type==EMPTY) {
                Move *m = checkPosition(col, row, col + 2, row, true);
                Move *m1 = checkPosition(col, row, col + 1, row, true);
                delete m1;
                if (m1 && m) {
                    add(m);
                    return true; // se forteaza rocada
                }
                delete m;
            }
        }
    }

    for (i = 0; i < 8; i++)
        add(checkPosition(col, row, col + offset_x[i], row + offset_y[i], true));

    return false; // nu se forteaza rocada
}

/* Check possible move for PAAAWN - NOT SIGMA PIECE */
void Bot::checkPawnMoves(int8_t col, int8_t row) {
    int8_t nextRow, nextNextRow, firstRow;

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
            f << "aratura\n";
            exit(-69);
    }

    Move *m;

    /* just one basic step ahead */
    if (betweenBorders(col, nextRow) && currentTable[col][nextRow].type == EMPTY) {
        m = checkPosition(col, row, col, nextRow);
        if (m != NULL && (nextRow == 1 || nextRow == 8)) {
            Q.push(Move::promote(m->getSource(), m->getDestination(), QUEEN));
            Q.push(Move::promote(m->getSource(), m->getDestination(), KNIGHT));
            delete m;
        } else add(m);
        /* 2 steps ahead OF YOU*/
        if (row == firstRow && betweenBorders(col, nextNextRow) &&
            currentTable[col][nextNextRow].type == EMPTY)
            add(checkPosition(col, row, col, nextNextRow));
    }

    for (int i = -1; i <= 1; i+=2) // left and right diagonal
        if (betweenBorders(col - i, nextRow)) {
            if ((currentTable[col - i][nextRow].type != EMPTY &&
                 currentTable[col - i][nextRow].color != getSideToMove()) ||
                (currentTable[col - i][nextRow].type == EMPTY &&
                 currentTable[col - i][row].enPassantEligible)) {
                    m = checkPosition(col, row, col - i, nextRow);
                    if (m != NULL && (nextRow == 1 || nextRow == 8)) {
                        Q.push(Move::promote(m->getSource(), m->getDestination(), QUEEN));
                        Q.push(Move::promote(m->getSource(), m->getDestination(), KNIGHT));
                        delete m;
                    } else add(m);
            }
        }
}

/* Check possible moves for BISHOP */
void Bot::checkBishopMoves(int8_t col, int8_t row) {
    for (int i = col + 1, j = row + 1; betweenBorders(i ,j); ++i, ++j) { // up right
        add(checkPosition(col, row, i, j));
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col + 1, j = row - 1; betweenBorders(i ,j); ++i, --j) { // down right
        add(checkPosition(col, row, i, j));
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col - 1, j = row + 1; betweenBorders(i ,j); --i, ++j) { // up left
        add(checkPosition(col, row, i, j));
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col - 1, j = row - 1; betweenBorders(i ,j); --i, --j) { // down left
        add(checkPosition(col, row, i, j));
        if (currentTable[i][j].type != EMPTY)
            break;
    }
}

/* Possible moves for KNIGHT */

void Bot::checkKnightMoves(int8_t col, int8_t row) {
    // if (betweenBorders(col + 2, row + 1) &&
    //     (currentTable[col + 2][row + 1].type == EMPTY || currentTable[col + 2][row + 1].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 2, row + 1));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col + 2, row + 1));
    // if (betweenBorders(col - 2, row + 1) &&
    //     (currentTable[col - 2][row + 1].type == EMPTY || currentTable[col - 2][row + 1].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 2, row + 1));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col - 2, row + 1));
    // if (betweenBorders(col + 2, row - 1) &&
    //     (currentTable[col + 2][row - 1].type == EMPTY || currentTable[col + 2][row - 1].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 2, row - 1));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col + 2, row - 1));
    // if (betweenBorders(col - 2, row - 1) &&
    //     (currentTable[col - 2][row - 1].type == EMPTY || currentTable[col - 2][row - 1].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 2, row - 1));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col - 2, row - 1));
    // if (betweenBorders(col + 1, row + 2) &&
    //     (currentTable[col + 1][row + 2].type == EMPTY || currentTable[col + 1][row + 2].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 1, row + 2));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col + 1, row + 2));
    // if (betweenBorders(col - 1, row + 2) &&
    //     (currentTable[col - 1][row + 2].type == EMPTY || currentTable[col - 1][row + 2].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 1, row + 2));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col - 1, row + 2));
    // if (betweenBorders(col + 1, row - 2) &&
    //     (currentTable[col + 1][row - 2].type == EMPTY || currentTable[col + 1][row - 2].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 1, row - 2));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col + 1, row - 2));
    // if (betweenBorders(col - 1, row - 2) &&
    //     (currentTable[col - 1][row - 2].type == EMPTY || currentTable[col - 1][row - 2].color != getSideToMove())) {
    //         Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 1, row - 2));
    //         if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
    //             Q.push(m);
    //     }
    add(checkPosition(col, row, col - 1, row - 2));
}

/* For THE ROOOOOOOK */
void Bot::checkRookMoves(int8_t col, int8_t row) {
    // bool empty, canCapture, check;
    /* Check left - if black, else check right*/
    for (int i = col + 1; i <= H; i++) {
        add(checkPosition(col, row, i, row));
        // empty = (currentTable[i][row].type == EMPTY);
        // canCapture = (currentTable[i][row].color != getSideToMove()); 

        // if (empty || canCapture) {
        //     Move* m = Move::moveTo(coordToStr(col, row), coordToStr(i, row));
        //     check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

        //     /* If we can move to a position, we'll add it to queue */
        //     if (check)
        //         Q.push(m);
        // }
        // if (!empty) {
        //     break;
        // }
        if (currentTable[i][row].type != EMPTY)
            break;
    }

    /* Check right - if black, else check left */
    for (int i = col - 1; i >= A; i--) {
        add(checkPosition(col, row, i, row));
        // empty = (currentTable[i][row].type == EMPTY);
        // canCapture = (currentTable[i][row].color != getSideToMove()); 
        
        // if (empty || canCapture) {
        //     Move* m = Move::moveTo(coordToStr(col, row), coordToStr(i, row));
        //     check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

        //     if (check)
        //         Q.push(m);
        // }
        // if (!empty) {
        //     break;
        // }
        if (currentTable[i][row].type != EMPTY)
            break;
    }

    /* Check down - if black, else check up */
    for (int i = row + 1; i <= 8; i++) {
        add(checkPosition(col, row, col, i));
        // empty = (currentTable[col][i].type == EMPTY);
        // canCapture = (currentTable[col][i].color != getSideToMove()); 
       
        // if (empty || canCapture) {
        //     Move* m = Move::moveTo(coordToStr(col, row), coordToStr(col, i));
        //     check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

        //     if (check)
        //         Q.push(m);
        // }
        // if (!empty) {
        //     break;
        // }
        if (currentTable[col][i].type != EMPTY)
            break;
    }

    /* Check up - if black, else check down */
    for (int i = row - 1; i >= 1; i--) {
        add(checkPosition(col, row, col, i));
        // empty = (currentTable[col][i].type == EMPTY);
        // canCapture = (currentTable[col][i].color != getSideToMove()); 

        // if (empty || canCapture) {
        //     Move* m = Move::moveTo(coordToStr(col, row), coordToStr(col, i));
        //     check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);
        //     if (check)
        //         Q.push(m);
        // }
        // if (!empty) {
        //     break;
        // }
        if (currentTable[col][i].type != EMPTY)
            break;
    }
}

/* */

bool Bot::isCheck (Table crtTable) {
    int8_t king_x = kingPos[getSideToMove()].first;
    int8_t king_y = kingPos[getSideToMove()].second;
    return isCheck(crtTable, king_x, king_y);
}

bool Bot::isCheck (Table crtTable, int8_t king_x, int8_t king_y) {

    int8_t x, y;

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
    if ((abs(king_x - kingPos[1 ^ getSideToMove()].first) <= 1) &&
        (abs(king_y - kingPos[1 ^ getSideToMove()].second) <= 1))
            return true;

    return false;
}
