#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"


JudgeDredd::Player1::Player1(bool amIWhite)
{
	this->amIWhite = amIWhite;
}

JudgeDredd::Player1::~Player1()
{

}

Move JudgeDredd::Player1::makeMove(Move lastMove)
{
	ChessBoard::InternalMove tmp(lastMove);
	currBoardState.ChangeState(tmp);
	Play(currBoardState, 0, 6, &tmp, amIWhite);
	return tmp.ConvertToExternal(amIWhite);
}

JudgeDredd::Player1::Evaluation JudgeDredd::Player1::Play(ChessBoard::Board currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove * chosenMove, bool isWhite)
{
	if (currentRecursion == maxRecursion)
		return Evaluate(currentBoard);
	Evaluation Best, newBest;
	ChessBoard::InternalMove bestMove, newbestMove;
	bool exceptionFlag = false;
	ChessBoard::Board temporary(currentBoard);
	temporary.moveIterator.Reset(isWhite);
	for (; *temporary.moveIterator != nullptr; ++temporary.moveIterator)
	{
		newbestMove = **temporary.moveIterator;
		
		try
		{
			temporary.ChangeState(newbestMove);
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
			temporary.Revert(isWhite);
		}
		else
			exceptionFlag = false;
	}


	return Best;
}

JudgeDredd::Player1::Evaluation JudgeDredd::Player1::Evaluate(ChessBoard::Board currBoard)
{
	return Evaluation();
}

bool JudgeDredd::Player1::Evaluation::operator>(Evaluation & right)
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

bool JudgeDredd::Player1::Evaluation::operator<(Evaluation & right)
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

bool JudgeDredd::Player1::Evaluation::operator=(Evaluation & right)
{
	return false;
}
