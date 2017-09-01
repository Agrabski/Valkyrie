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
	tmp.isNull = false;
	tmp.gameHasEnded = false;
	return tmp;
}

bool ChessEvaluator::ChessEvaluation::operator>(ChessEvaluation & right)
{
	//null saveguards
	if ((isNull && !right.isNull)||(isNull&&right.isNull))
		return false;
	if (!isNull&&right.isNull)
		return true;

	//endstate
	if (gameHasEnded || right.gameHasEnded)
	{
		if (gameHasEnded && right.gameHasEnded)
			return endState > right.endState;
		if (gameHasEnded)
		{
			if (endState == 1)
				return true;
			if (endState == -1)
				return false;
			if (right.victoryPossible)
				return false;
			return true;
		}
		else
		{
			if (right.endState == 1)
				return true;
			if (right.endState == -1)
				return false;
			if (victoryPossible)
				return true;
			return false;
		}
	}
	if (victoryPossible == right.victoryPossible)
		return value > right.value;
	if (victoryPossible)
		return true;
	return false;
}

bool ChessEvaluator::ChessEvaluation::operator<(ChessEvaluation & right)
{
	return!(*this > right);
}

bool ChessEvaluator::ChessEvaluation::operator==(ChessEvaluation & right)
{
	return value == right.value&&gameHasEnded == right.gameHasEnded&&victoryPossible == right.victoryPossible;
}

bool ChessEvaluator::ChessEvaluation::operator>=(ChessEvaluation & right)
{
	return *this > right || *this == right;
}

bool ChessEvaluator::ChessEvaluation::operator<=(ChessEvaluation & right)
{
	return *this < right || *this == right;
}

ChessEvaluator::ChessEvaluation & ChessEvaluator::ChessEvaluation::operator=(ChessEvaluation & tocopy)
{
	if (tocopy.isNull)
		return *this;
	this->isNull = false;
	value = tocopy.value;
	this->endState = tocopy.endState;
	gameHasEnded = tocopy.gameHasEnded;
	victoryPossible = tocopy.victoryPossible;
	return*this;
}

ChessEvaluator::ChessEvaluation::ChessEvaluation()
{
	value = 0;
	endState = 0;
	victoryPossible = true;
	gameHasEnded = false;
}

