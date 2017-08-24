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
	std::vector<ChessBoard::Board>*boardArray = new std::vector<ChessBoard::Board>(recursionDepth+1);
	(*boardArray)[0] = *currBoardState;
	Play(boardArray, 0, recursionDepth, buffer, amIWhite);
	currBoardState->ChangeState(*buffer);
	Move tmp = buffer->ConvertToExternal(amIWhite);
	delete buffer;
	delete boardArray;
	return tmp;
}

ChessEvaluator::ChessEvaluation JudgeDredd::Valkyrie::Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove * chosenMove, bool isWhite)
{
	std::cout << currentRecursion << std::endl;
	if (currentRecursion == maxRecursion) 
	{
		return evaluator.evaluate((*boardVector)[currentRecursion]);

	}
	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove, newbestMove;
	bool firstFlag = true;
	(*boardVector)[currentRecursion + 1] = ((*boardVector)[currentRecursion]);
	ChessBoard::Board::Moves moveIterator(&(*boardVector)[currentRecursion+1]);
	moveIterator.Reset(isWhite);
	++(moveIterator);
	for (; *moveIterator != nullptr; ++moveIterator)
	{
		newbestMove = **moveIterator;
		
		try
		{
			(*boardVector)[currentRecursion + 1].ChangeState(newbestMove);
			newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite);
			if (firstFlag || newBest > Best)
			{
				Best = newBest;
				bestMove = newbestMove;
				firstFlag = false;
			}
			(*boardVector)[currentRecursion + 1].Revert();
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
