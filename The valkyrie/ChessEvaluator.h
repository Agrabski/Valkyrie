#pragma once
#include "chessboard.h"

namespace ChessEvaluator
{
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
	class ChessEvaluator
	{
	public:
		ChessEvaluator();
		Evaluation evaluate(const ChessBoard::Board &board);
	};
}