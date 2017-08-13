#ifndef JUDGEDREDD_H_INCLUDED
#define JUDGEDREDD_H_INCLUDED
#pragma once
#include <stdlib.h>

struct Move
{
    bool whiteMove;
    bool special;
    std::pair<int, int> from;
    std::pair<int, int> to;
    char type;
};

class JudgeDredd
{
    class Player1;
    class Player2;

    enum rank{empty, pownW, pownB, knightW, knightB, bishopW, bishopB, towerW, towerB, queenW, queenB, kingW, kingB};
    public:
    rank board[8][8];
    std::pair<bool, rank> prevBoard[2048][8][8];
    int fiftyMoves;
    bool castleKW;
    bool castleQW;
    bool castleKB;
    bool castleQB;
    std::pair<int, int> enPassan;
    std::pair<int, int> score;
    std::pair<int, int> time;
    float maxTime;
    Move lastMove;
    bool isWhiteP1;
    Player1* player1;
    Player2* player2;

    JudgeDredd(bool isWhite, int time); //odpala 2 constructory naszych klas
    ~JudgeDredd();
    void resetBoard(); //destructor naszych klas, odwraca isWhiteP1, construktor naszych klas
    bool move(); // if(isWhiteP1){p1.move}; while z is Over { p2.move; p1.move}, maxtime+=0,5
    bool isOver(); //
    bool validMove();
    void printScore();
    void printBoard();



};


#endif // JUDGEDREDD_H_INCLUDED
