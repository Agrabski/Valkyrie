#ifndef PLAYER1_H_INCLUDED
#define PLAYER1_H_INCLUDED

#include "chessboard.h"
#include "judgeDredd.h"
#include <vector>
#include "Evaluator.h"
#define PROGRAM_NAME "valkyrie"
#pragma once

class JudgeDredd::Valkyrie
{
    int x;

    public:
    Valkyrie(bool amIWhite);
	Valkyrie(bool amIwhite, int recursion);
    ~Valkyrie();
    struct Move makeMove(struct Move lastMove);
	private:
		int recursionDepth=1;
		bool firstMove;
		ChessBoard::Board *currBoardState=new ChessBoard::Board();
		bool amIWhite;
		ChessEvaluator::ChessEvaluation Play(ChessBoard::Board *currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite);
		ChessEvaluator::ChessEvaluator evaluator;
};

#endif // PLAYER1_H_INCLUDED
