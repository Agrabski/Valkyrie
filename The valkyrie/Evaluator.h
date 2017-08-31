#pragma once
#include "chessboard.h"


namespace ChessEvaluator
{
	struct ChessEvaluation
	{
		bool isNull = true;
		bool gameHasEnded;
		bool victoryPossible;
		short int endState;//-1 means loss, 0 tie, 1 victory
		double value;
		bool operator>(ChessEvaluation & right);
		bool operator<(ChessEvaluation & right);
		bool operator==(ChessEvaluation & right);
		bool operator>=(ChessEvaluation & right);
		bool operator<=(ChessEvaluation & right);
		ChessEvaluation&operator=(ChessEvaluation&tocopy);
		ChessEvaluation();
	};
	class ChessEvaluator
	{
	public:
		ChessEvaluator();
		ChessEvaluation evaluate(const ChessBoard::Board &board);
	};
}