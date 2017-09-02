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
		float friendlyValue=0.1f;
		float enemyValue=0.1f;
		float friendlyCheck=0.1f;
		float enemyCheck=0.1f;
		ChessEvaluator();
		ChessEvaluator(float a,float b,float c, float d);
		ChessEvaluation evaluate(const ChessBoard::Board &board) const;
	};
}