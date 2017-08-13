#ifndef PLAYER1_H_INCLUDED
#define PLAYER1_H_INCLUDED

#include "chessboard.h"
#include "judgeDredd.h"
#include <vector>
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
		ChessBoard::Board currBoardState;
		bool amIWhite;
		struct Evaluation
		{
			bool gameHasEnded;
			bool victoryPossible;
			short int endState;//-1 means loss, 0 tie, 1 victory
			double value;
			bool operator>(Evaluation & right);
			bool operator<(Evaluation & right);
			bool operator=(Evaluation & right);

		};
		Evaluation Play(ChessBoard::Board currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite);
		Evaluation Evaluate(ChessBoard::Board currBoard);
};

#endif // PLAYER1_H_INCLUDED
