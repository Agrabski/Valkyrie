#ifndef PLAYER1_H_INCLUDED
#define PLAYER1_H_INCLUDED

#include "chessboard.h"
#include "judgeDredd.h"
#include <vector>
#include "ChessEvaluator.h"
#define PROGRAM_NAME "valkyrie"
#pragma once
class JudgeDredd::Player1
{
    int x;

    public:
    Player1(bool amIWhite);
    ~Player1();
    struct Move makeMove(struct Move lastMove);
	private:
		ChessBoard::Board *currBoardState=new ChessBoard::Board();
		bool amIWhite;
		ChessEvaluator::Evaluation Play(ChessBoard::Board *currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite);
		ChessEvaluator::ChessEvaluator evaluator;
};

#endif // PLAYER1_H_INCLUDED
