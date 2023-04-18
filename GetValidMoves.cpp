#include "Bot.h"
#include "PlaySide.h"

std :: queue<Move*> Q;
//Bot b;
extern std::ofstream f;

bool betweenBorders (int8_t x, int8_t y) {

    if (x > 0 && x <= 8 && y > 0 && y <= 8) {
        return true;
    }
    
    return false;
}

void Bot::checkPosition(int8_t col, int8_t row, int8_t i, int8_t j, bool type = false) {
    if (betweenBorders(i, j))
        if (currentTable[i][j].type == EMPTY || currentTable[i][j].color != getSideToMove()) {
            Move* m = Move :: moveTo(coordToStr(col, row), coordToStr(i, j));
            if (type) {
                if (isCheck(*(Bot::createModifiedTable(m, currentTable)), i, j) == false)
                    Q.push(m);
            } else {
                if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                    Q.push(m);
            }
        }
}

/* Check possible moves for BISHOP */
void Bot::checkBishopMoves(int8_t col, int8_t row) {
    for (int i = col + 1, j = row + 1; betweenBorders(i ,j); ++i, ++j) { // up right
        checkPosition(col, row, i, j);
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col + 1, j = row - 1; betweenBorders(i ,j); ++i, --j) { // down right
        checkPosition(col, row, i, j);
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col - 1, j = row + 1; betweenBorders(i ,j); --i, ++j) { // up left
        checkPosition(col, row, i, j);
        if (currentTable[i][j].type != EMPTY)
            break;
    }

    for (int i = col - 1, j = row - 1; betweenBorders(i ,j); --i, --j) { // down left
        checkPosition(col, row, i, j);
        if (currentTable[i][j].type != EMPTY)
            break;
    }
}

/* Possible moves for KNIGHT */

void Bot::checkKnightMoves(int8_t col, int8_t row) {
    if (betweenBorders(col + 2, row + 1) &&
        (currentTable[col + 2][row + 1].type == EMPTY || currentTable[col + 2][row + 1].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 2, row + 1));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col - 2, row + 1) &&
        (currentTable[col - 2][row + 1].type == EMPTY || currentTable[col - 2][row + 1].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 2, row + 1));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col + 2, row - 1) &&
        (currentTable[col + 2][row - 1].type == EMPTY || currentTable[col + 2][row - 1].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 2, row - 1));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col - 2, row - 1) &&
        (currentTable[col - 2][row - 1].type == EMPTY || currentTable[col - 2][row - 1].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 2, row - 1));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
        
    if (betweenBorders(col + 1, row + 2) &&
        (currentTable[col + 1][row + 2].type == EMPTY || currentTable[col + 1][row + 2].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 1, row + 2));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col - 1, row + 2) &&
        (currentTable[col - 1][row + 2].type == EMPTY || currentTable[col - 1][row + 2].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 1, row + 2));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col + 1, row - 2) &&
        (currentTable[col + 1][row - 2].type == EMPTY || currentTable[col + 1][row - 2].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col + 1, row - 2));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    if (betweenBorders(col - 1, row - 2) &&
        (currentTable[col - 1][row - 2].type == EMPTY || currentTable[col - 1][row - 2].color != getSideToMove())) {
            Move *m = Move::moveTo(coordToStr(col, row), coordToStr(col - 1, row - 2));
            if (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false)
                Q.push(m);
        }
    
}

/* For THE ROOOOOOOK */
void Bot::checkRookMoves(int8_t col, int8_t row) {
    bool empty, canCapture, check;
    /*check this shit ca nu da break*/
    /* Check left - if black, else check right*/
    for (int i = col + 1; i <= H; i++) {
        empty = (currentTable[i][row].type == EMPTY);
        canCapture = (currentTable[i][row].color != getSideToMove()); 

        if (empty || canCapture) {
            Move* m = Move::moveTo(coordToStr(col, row), coordToStr(i, row));
            check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

            /* If we can move to a position, we'll */
            if (check)
                Q.push(m);
        } else if (!empty && !canCapture) {
            break;
        }
    }

    /* Check right - if black, else check left */
    for (int i = col - 1; i >= A; i--) {
        empty = (currentTable[i][row].type == EMPTY);
        canCapture = (currentTable[i][row].color != getSideToMove()); 
        
        if (empty || canCapture) {
            Move* m = Move::moveTo(coordToStr(col, row), coordToStr(i, row));
            check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

            if (check)
                Q.push(m);
        } else if (!empty && !canCapture) {
            break;
        }
    }

    /* Check down - if black, else check up */
    for (int i = row + 1; i <= 8; i++) {
        empty = (currentTable[col][i].type == EMPTY);
        canCapture = (currentTable[col][i].color != getSideToMove()); 
       
        if (empty || canCapture) {
            Move* m = Move::moveTo(coordToStr(col, row), coordToStr(col, i));
            check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);

            if (check)
                Q.push(m);
        } else if (!empty && !canCapture) {
            break;
        }
    }

    /* Check up - if black, else check down */
    for (int i = row - 1; i >= 1; i--) {
        empty = (currentTable[col][i].type == EMPTY);
        canCapture = (currentTable[col][i].color != getSideToMove()); 
        f << "in rook\n";

        if (empty || canCapture) {
            Move* m = Move::moveTo(coordToStr(col, row), coordToStr(col, i));
            check = (isCheck(*(Bot::createModifiedTable(m, currentTable))) == false);
            if (check)
                Q.push(m);
        } else if (!empty && !canCapture) {
            break;
        }
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
    int offset_x[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    int offset_y[8] = {1, 2, 2, 1, -1, -2, -2, -1};

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

    return false;
}
