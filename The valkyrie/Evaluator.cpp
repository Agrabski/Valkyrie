#include "Evaluator.h"

ChessEvaluator::ChessEvaluator::ChessEvaluator()
{
}

ChessEvaluator::ChessEvaluation ChessEvaluator::ChessEvaluator::evaluate(const ChessBoard::Board & board)
{
	ChessEvaluation tmp = ChessEvaluation();
	
	return tmp;
}

bool ChessEvaluator::ChessEvaluation::operator>(ChessEvaluation & right)
{
	if (gameHasEnded && endState == 1)
		return true;
	if (right.gameHasEnded && right.endState == 1)
		return false;
	if (gameHasEnded && endState == 0 && ((right.gameHasEnded&&right.endState == -1) || !right.victoryPossible))
		return true;
	if (right.gameHasEnded && right.endState == 0 && ((gameHasEnded&&endState == -1) || !victoryPossible))
		return false;
	if (victoryPossible && !right.victoryPossible)
		return true;
	if (!victoryPossible && right.victoryPossible)
		return false;
	return value > right.value;
}

bool ChessEvaluator::ChessEvaluation::operator<(ChessEvaluation & right)
{
	if (gameHasEnded && endState == 1)
		return false;
	if (right.gameHasEnded && right.endState == 1)
		return true;
	if (gameHasEnded && endState == 0 && ((right.gameHasEnded&&right.endState == -1) || !right.victoryPossible))
		return false;
	if (right.gameHasEnded && right.endState == 0 && ((gameHasEnded&&endState == -1) || !victoryPossible))
		return true;
	if (victoryPossible && !right.victoryPossible)
		return false;
	if (!victoryPossible && right.victoryPossible)
		return true;
	return value < right.value;
}

bool ChessEvaluator::ChessEvaluation::operator=(ChessEvaluation & right)
{
	throw std::runtime_error("Class not allowed");
	return false;
}

