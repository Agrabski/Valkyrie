#include "Evaluator.h"

ChessEvaluator::ChessEvaluator::ChessEvaluator()
{
}

ChessEvaluator::ChessEvaluation ChessEvaluator::ChessEvaluator::evaluate(const ChessBoard::Board & board)
{
	ChessEvaluation tmp = ChessEvaluation();
	for(short x=0;x<8;x++)
		for (short y = 0; y < 8; y++)
		{
			tmp.value += board.fields[x][y].rank.type*(board.fields[x][y].coveredByWhite + 1);
			tmp.value -= board.fields[x][y].rank.type*(board.fields[x][y].coveredByBlack + 1);
		}
	
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
	return value == right.value&&gameHasEnded == right.gameHasEnded&&victoryPossible == right.victoryPossible;
}

ChessEvaluator::ChessEvaluation::ChessEvaluation()
{
	value = 0;
	endState = 0;
	victoryPossible = true;
	gameHasEnded = false;
}

