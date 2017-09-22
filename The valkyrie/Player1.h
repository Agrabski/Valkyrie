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
	Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator = {.1f,.1f,.1f,.1f}, int recursion = 5);
    ~Valkyrie();
    struct Move makeMove(struct Move lastMove);

	private:
		int recursionDepth=5;
		bool firstMove;
		ChessBoard::Board *currBoardState=new ChessBoard::Board();
		bool amIWhite;
		ChessEvaluator::ChessEvaluation Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite,ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const;
		ChessEvaluator::ChessEvaluator evaluator;
};
class GAME_ENDED :std::exception
{
public:
	GAME_ENDED(bool tie, bool white, bool black)
	{
		this->tie = tie;
		this->blackWon = black;
		this->whiteWon = white;
	}
	bool tie;
	bool whiteWon;
	bool blackWon;
};
#endif // PLAYER1_H_INCLUDED
