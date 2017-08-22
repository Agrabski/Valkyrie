#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"


JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	this->amIWhite = amIWhite;
	firstMove = amIWhite;
}

JudgeDredd::Valkyrie::Valkyrie(bool amIwhite, int recursion)
{
	amIWhite = amIwhite;
	recursionDepth = recursion;
}

JudgeDredd::Valkyrie::~Valkyrie()
{
	delete currBoardState;
}

Move JudgeDredd::Valkyrie::makeMove(Move lastMove)
{
	ChessBoard::InternalMove *buffer=new ChessBoard::InternalMove();
	if (!firstMove)
	{
		ChessBoard::InternalMove tmp(lastMove);
		firstMove = false;
		currBoardState->ChangeState(tmp);
	}
	Play(currBoardState, 0, recursionDepth, buffer, amIWhite);
	currBoardState->ChangeState(*buffer);
	Move tmp = buffer->ConvertToExternal(amIWhite);
	delete buffer;
	return tmp;
}

ChessEvaluator::ChessEvaluation JudgeDredd::Valkyrie::Play(const ChessBoard::Board *currentBoard, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove * chosenMove, bool isWhite)
{
	std::cout << currentRecursion << std::endl;
	if (currentRecursion == maxRecursion) 
	{
		return evaluator.evaluate(*currentBoard);

	}
	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove, newbestMove;
	bool firstFlag = true;
	ChessBoard::Board *temporary=new ChessBoard::Board(currentBoard);
	ChessBoard::Board::Moves moveIterator(temporary);
	moveIterator.Reset(isWhite);
	++(moveIterator);
	for (; *moveIterator != nullptr; ++moveIterator)
	{
		newbestMove = **moveIterator;
		
		try
		{
			temporary->ChangeState(newbestMove);
			newBest = Play(temporary, currentRecursion + 1, maxRecursion, nullptr, !isWhite);
			if (firstFlag || newBest > Best)
			{
				Best = newBest;
				bestMove = newbestMove;
				firstFlag = false;
			}
			temporary->Revert();
		}
		catch (ChessBoard::INVALID_MOVE)
		{

		}
		catch(ChessBoard::WRONG_COLOR)
		{ }
		catch(ChessBoard::THREEFOLD_REPETITON)
		{ }
		catch(ChessBoard::MOVE_BLOCKED)
		{ }
	}
	if (chosenMove != nullptr)
		*chosenMove = bestMove;
	return Best;
}
