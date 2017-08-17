#pragma once
#include "chessboard.h"

class AbstractBoard
{

};

struct Evaluation
{
	bool operator>(Evaluation & right);
	bool operator<(Evaluation & right);
	bool operator=(Evaluation & right);
};

class Evaluator
{
public:
	Evaluation virtual evaluate(const AbstractBoard &board) = 0;
};

namespace ChessEvaluator
{
	struct ChessEvaluation
	{
		bool gameHasEnded;
		bool victoryPossible;
		short int endState;//-1 means loss, 0 tie, 1 victory
		double value;
		bool operator>(ChessEvaluation & right);
		bool operator<(ChessEvaluation & right);
		bool operator=(ChessEvaluation & right);
	};
	class ChessEvaluator
	{
	public:
		ChessEvaluator();
		ChessEvaluation evaluate(const ChessBoard::Board &board);
	};
}