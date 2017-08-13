#ifndef PLAYER2_H_INCLUDED
#define PLAYER2_H_INCLUDED

#include "judgeDredd.h"


class JudgeDredd::Player2
{
    int x;
    struct Move library[9];//only works when P1 is white, when P2 is white can be used to simulate bad moves

    public:
    Player2(bool amIWhite);
    ~Player2();
    struct Move makeMove(struct Move lastMove);
};

#endif // PLAYER2_H_INCLUDED
