#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"
#define DEBUG
#define CUT

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
	ChessEvaluator::ChessEvaluation alpha, beta;
	std::vector<ChessBoard::Board>*boardArray = new std::vector<ChessBoard::Board>(recursionDepth+1);
	(*boardArray)[0] = *currBoardState;
#ifdef DEBUG
	int evaluation = Play(boardArray, 0, recursionDepth, buffer, amIWhite,alpha,beta).value;
	std::cout << evaluation;
#endif // DEBUG
#ifndef DEBUG
	Play(boardArray, 0, recursionDepth, buffer, amIWhite, &alpha, &beta);

#endif // !DEBUG

	currBoardState->ChangeState(*buffer);
	Move tmp = buffer->ConvertToExternal(amIWhite);
	delete buffer;
	delete boardArray;
	return tmp;
}

ChessEvaluator::ChessEvaluation JudgeDredd::Valkyrie::Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta)
{
#ifdef DEBUG
	std::cout << currentRecursion<<(currentRecursion!=maxRecursion?"----":"");
#endif // DEBUG
	if (currentRecursion == maxRecursion) 
	{
#ifdef DEBUG
		std::cout << std::endl;
#endif // DEBUG

		return evaluator.evaluate((*boardVector)[currentRecursion]);

	}


	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove, newbestMove;
	bool firstFlag = true;
	(*boardVector)[currentRecursion + 1] = ((*boardVector)[currentRecursion]);
	ChessBoard::Board::Moves moveIterator(&(*boardVector)[currentRecursion+1]);

	moveIterator.Reset(isWhite);
	++(moveIterator);


	if (isWhite)
	{
		for (; *moveIterator != nullptr; ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove = **moveIterator;
				(*boardVector)[currentRecursion + 1].ChangeState(newbestMove);
				newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite, alpha, beta);


				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
			}
			catch(ChessBoard::MOVE_BLOCKED)
			{ }
			catch (ChessBoard::INVALID_MOVE)
			{

			}

			if (!newBest.isNull&&(newBest > alpha))
			{
				firstFlag = false;
				alpha = (Best = newBest);
				bestMove = newbestMove;
				if (chosenMove != nullptr)
					*chosenMove = bestMove;
			}


			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
			{
#ifdef DEBUG
				std::cout << "!!!TREE CUT!!!";
#endif // DEBUG
				break;
			}


		}

	}
	else
	{
		for (; *moveIterator != nullptr; ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove = **moveIterator;
				(*boardVector)[currentRecursion + 1].ChangeState(newbestMove);
				newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite, alpha, beta);


				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
			}
			catch (ChessBoard::INVALID_MOVE)
			{

			}

			if (!newBest.isNull&&(beta.isNull||newBest < beta))
			{
				firstFlag = false;
				beta = (Best = newBest);
				bestMove = newbestMove;
				if (chosenMove != nullptr)
					*chosenMove = bestMove;
			}


			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
			{
#ifdef DEBUG
				std::cout << "!!!TREE CUT!!!";
#endif // DEBUG
				break;
			}


		}
	}

	if (firstFlag)
	{
		Best.isNull = false;
		Best.gameHasEnded = true;
		Best.endState = (*boardVector)[currentRecursion + 1].IsWhiteChecked() ? -1 : (*boardVector)[currentRecursion + 1].IsBlackChecked() ? 1 : 0;
	}

	return Best;

}


#pragma region INCASE



//for (; *moveIterator != nullptr; ++moveIterator)
//{
//	newbestMove = **moveIterator;
//
//	try
//	{
//		(*boardVector)[currentRecursion + 1].ChangeState(newbestMove);
//		newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite, (firstFlag ? nullptr : &Best));
//		if (firstFlag || (isWhite ? newBest > Best:newBest < Best))
//		{
//			Best = newBest;
//			bestMove = newbestMove;
//			firstFlag = false;
//		}
//		(*boardVector)[currentRecursion + 1].Revert();
//#ifdef CUT
//
//		if (currBest != nullptr && ((!isWhite && Best < *currBest) || (isWhite&& Best > *currBest)))
//		{
//#ifdef DEBUG
//			std::cout << "!!!TREE CUT!!!";
//#endif // DEBUG
//
//			return Best;
//		}
//#endif //CUT
//	}
//	catch (ChessBoard::INVALID_MOVE)
//	{
//
//	}
//	catch (ChessBoard::WRONG_COLOR)
//	{
//	}
//	catch (ChessBoard::THREEFOLD_REPETITON)
//	{
//	}
//	catch (ChessBoard::MOVE_BLOCKED)
//	{
//	}
//}
//
//if (chosenMove != nullptr)
//*chosenMove = bestMove;
//
//#ifdef DEBUG
//if (firstFlag)
//std::cout << std::endl;
//#endif // DEBUG
//
//if (firstFlag)
//{
//	Best.gameHasEnded = true;
//	Best.endState = (*boardVector)[currentRecursion + 1].IsWhiteChecked() ? -1 : (*boardVector)[currentRecursion + 1].IsBlackChecked() ? 1 : 0;
//}
//return Best;
#pragma endregion