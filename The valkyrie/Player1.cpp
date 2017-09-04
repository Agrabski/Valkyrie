#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"
#define CUT

JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	this->amIWhite = amIWhite;
	firstMove = amIWhite;
}

JudgeDredd::Valkyrie::Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator, int recursion)
{
	amIWhite = amIwhite;
	firstMove = amIWhite;
	recursionDepth = recursion;
	this->evaluator = evaluator;
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
		currBoardState->ChangeState(tmp);
	}
	firstMove = false;
	ChessEvaluator::ChessEvaluation alpha, beta;
	std::vector<ChessBoard::Board>boardArray(recursionDepth+1);
	boardArray[0] = *currBoardState;
	alpha = Play(&boardArray, 0, recursionDepth, buffer, amIWhite, alpha, beta);
#ifdef DEBUG
	std::cout << alpha.value;
#endif // DEBUG
	currBoardState->ChangeState(*buffer);

	Move tmp = buffer->ConvertToExternal(amIWhite);
	delete buffer;
	boardArray.clear();
	return tmp;
}

ChessEvaluator::ChessEvaluation JudgeDredd::Valkyrie::Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessBoard::InternalMove *chosenMove, bool isWhite, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const
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
	ChessBoard::InternalMove bestMove;
	ChessBoard::InternalMove newbestMove;
	bool StealmateFlag = true;
	(*boardVector)[currentRecursion + 1] = ((*boardVector)[currentRecursion]);
	ChessBoard::Board::Moves moveIterator(&(*boardVector)[currentRecursion+1]);

	moveIterator.Reset(isWhite);
	++(moveIterator);


	if (isWhite)
	{
		for (; (*moveIterator).from != std::pair<short,short>(8,0); ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove =ChessBoard::InternalMove( *moveIterator);
				(*boardVector)[currentRecursion + 1].ChangeState(*moveIterator);
				newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite, alpha, beta);


				(*boardVector)[currentRecursion + 1].Revert();
				StealmateFlag = false;

			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch(ChessBoard::MOVE_BLOCKED)
			{ }
			catch (ChessBoard::INVALID_MOVE)
			{

			}

			if (newbestMove.from!= std::pair<short, short>(8, 0) &&!newBest.isNull&&(newBest > alpha))
			{
				alpha = (Best = newBest);
				bestMove = newbestMove;
				if (chosenMove != nullptr)
					*chosenMove = bestMove;
			}


			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
			{
#ifdef DEBUG
				std::cout << "!!!TREE CUT!!!("<<currentRecursion<<")\n";
#endif // DEBUG
				break;
			}


		}

	}
	else
	{
		for (; (*moveIterator).from != std::pair<short, short>(8, 0); ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove = *moveIterator;
				(*boardVector)[currentRecursion + 1].ChangeState(*moveIterator);
				newBest = Play(boardVector, currentRecursion + 1, maxRecursion, nullptr, !isWhite, alpha, beta);
				StealmateFlag = false;

				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
			}
			catch (ChessBoard::INVALID_MOVE)
			{

			}
			catch (ChessBoard::KING_IN_DANGER &err) {}

			if (newbestMove.from != std::pair<short, short>(8, 0) && !newBest.isNull&&(beta.isNull||beta.gameHasEnded||!newBest.gameHasEnded)&&(beta.isNull||((beta.gameHasEnded&&!newBest.gameHasEnded)||newBest < beta)))
			{

				beta = (Best = newBest);
				bestMove = newbestMove;
				if (chosenMove != nullptr)
					*chosenMove = bestMove;
			}


			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
			{
#ifdef DEBUG
				std::cout << "!!!TREE CUT!!!(" << currentRecursion << ")\n";
#endif // DEBUG
				break;
			}


		}
	}

	if (StealmateFlag)
	{
		Best.isNull = false;
		Best.gameHasEnded = true;
		Best.endState = (*boardVector)[currentRecursion + 1].IsWhiteChecked() ? -1 : (*boardVector)[currentRecursion + 1].IsBlackChecked() ? 1 : 0;
		if (currentRecursion == 0)
			throw GAME_ENDED(!((*boardVector)[1].IsWhiteChecked() && (*boardVector)[1].IsBlackChecked()), (*boardVector)[1].IsWhiteChecked(), (*boardVector)[1].IsBlackChecked());
		return Best;
	}

	return isWhite?alpha:beta;

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