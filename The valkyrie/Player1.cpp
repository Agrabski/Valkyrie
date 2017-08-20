#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"


JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	this->amIWhite = amIWhite;
}

JudgeDredd::Valkyrie::~Valkyrie()
{
	delete currBoardState;
}

Move JudgeDredd::Valkyrie::makeMove(Move lastMove)
{
	ChessBoard::InternalMove tmp(lastMove);
	currBoardState->ChangeState(tmp);
	Play(currBoardState, 0, 6, &tmp, amIWhite);
	return tmp.ConvertToExternal(amIWhite);
}

ChessEvaluator::ChessEvaluation JudgeDredd::Valkyrie::Play(ChessBoard::Board *currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove * chosenMove, bool isWhite)
{
	if (currentRecursion == maxRecursion)
		return evaluator.evaluate(*currentBoard);
	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove, newbestMove;
	bool exceptionFlag = false;
	ChessBoard::Board *temporary(currentBoard);
	temporary->moveIterator->Reset(isWhite);
	for (; **(temporary->moveIterator) != nullptr; ++(temporary->moveIterator))
	{
		newbestMove = ***temporary->moveIterator;
		
		try
		{
			temporary->ChangeState(newbestMove);
		}
		catch (ChessBoard::INVALID_MOVE)
		{
			exceptionFlag = true;
		}
		if (!exceptionFlag)
		{
			newBest = Play(temporary, currentRecursion + 1, maxRecursion, nullptr, !isWhite);
			if (newBest > Best)
			{
				Best = newBest;
				bestMove = newbestMove;
			}
			temporary->Revert();
		}
		else
			exceptionFlag = false;
	}


	return Best;
}
