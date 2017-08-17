#include "ChessEvaluator.h"

ChessEvaluator::Evaluation ChessEvaluator::ChessEvaluator::evaluate(const ChessBoard::Board & board)
{
	return Evaluation();
}
bool ChessEvaluator::Evaluation::operator>(Evaluation & right)
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

bool ChessEvaluator::Evaluation::operator<(Evaluation & right)
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

bool ChessEvaluator::Evaluation::operator=(Evaluation & right)
{
	return false;
}