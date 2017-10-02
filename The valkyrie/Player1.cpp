#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"
#include <thread>
#include <atomic>
#define CUT
//#define MAPDEBUG

JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	this->amIWhite = amIWhite;
	firstMove = amIWhite;
	maxThreadCount =  std::thread::hardware_concurrency();
}

JudgeDredd::Valkyrie::Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator, int recursion)
{
	amIWhite = amIwhite;
	firstMove = amIWhite;
	recursionDepth = recursion;
	this->evaluator = evaluator;
	maxThreadCount =  std::thread::hardware_concurrency();
}

JudgeDredd::Valkyrie::~Valkyrie()
{
	delete currBoardState;
}

Move JudgeDredd::Valkyrie::makeMove(Move lastMove)
{
	if (!firstMove)
	{
		ChessBoard::InternalMove tmp(lastMove);
		currBoardState->ChangeState(tmp, 0);
	}
	bool stealmateFlag = true;
	firstMove = false;
	ChessEvaluator::ChessEvaluation alpha, beta;
	std::vector<std::vector<ChessBoard::Board>>boardArray(maxThreadCount);
	std::vector<ChessEvaluator::ChessEvaluation>resultVector(maxThreadCount);
	std::vector<std::pair<std::thread, ChessBoard::InternalMove>>threadVector(maxThreadCount);
	threadVector.resize(maxThreadCount);
	std::atomic<int>evaluationCount = 0;
	ChessEvaluator::ChessEvaluation best;
	ChessBoard::InternalMove bestMove;
	for (int i = 0; i < maxThreadCount; i++)
	{
		boardArray[i].resize(recursionDepth + 1);
		boardArray[i][0] = *currBoardState;
	}

	ChessBoard::Board::Moves moveIterator(currBoardState);
	moveIterator.Reset(amIWhite);
	++moveIterator;

	for (int i = 0; i < maxThreadCount && *moveIterator != moveIterator.cend();++moveIterator, i++)
	{
		for(bool successFlag=false;!successFlag&&*moveIterator != moveIterator.cend();++moveIterator)
		{
			try
			{
				boardArray[i][0].ChangeState(*moveIterator);

				threadVector[i] =std::pair<std::thread, ChessBoard::InternalMove>(std::thread(Player(), this, &(boardArray[i]), 0, recursionDepth, &(resultVector[i]), !amIWhite, alpha, beta,&evaluationCount),*moveIterator);

				successFlag = true;
				stealmateFlag = false;

			}
			catch (ChessBoard::FIFTY_MOVES)
			{
				resultVector[i].gameHasEnded = true;
				resultVector[i].isNull = false;
				resultVector[i].endState = 0;
			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				resultVector[i].gameHasEnded = true;
				resultVector[i].isNull = false;
				resultVector[i].endState = 0;
				boardArray[i][0].Revert();
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
			}
			catch (ChessBoard::INVALID_MOVE)
			{

			}
			catch (ChessBoard::KING_IN_DANGER)
			{

			}
		}
	}

	if (stealmateFlag)
		throw GAME_ENDED((!currBoardState->IsWhiteChecked()) && (!currBoardState->IsBlackChecked()), currBoardState->IsWhiteChecked(), currBoardState->IsBlackChecked());

	while (*moveIterator!=moveIterator.cend())
	{
		while (evaluationCount == 0)
			std::this_thread::yield();
		++moveIterator;
		int threadId = 0;
		while (threadId<maxThreadCount)
		{
			if (!resultVector[threadId].isNull)
			{
				evaluationCount--;
				break;
			}
			threadId++;
		}

		if (amIWhite)
		{
			if (!resultVector[threadId].isNull && (resultVector[threadId] > best))
			{
				best = resultVector[threadId];
				bestMove = threadVector[threadId].second;
				resultVector[threadId].isNull = true;
			}
		}
		else
		{
			if (!resultVector[threadId].isNull && (best.isNull || best.gameHasEnded || !resultVector[threadId].gameHasEnded) && (best.isNull || ((best.gameHasEnded && !resultVector[threadId].gameHasEnded) || resultVector[threadId] < best)))
			{

				best = resultVector[threadId];
				bestMove = threadVector[threadId].second;
				resultVector[threadId].isNull = true;
			}
		}

				if (threadVector[threadId].first.joinable())
					threadVector[threadId].first.join();

		boardArray[threadId][0] = *currBoardState;


		while (*moveIterator!=moveIterator.cend())
		{
			try
			{	
				boardArray[threadId][0].ChangeState(*moveIterator);
				threadVector[threadId]=std::pair<std::thread,ChessBoard::InternalMove>( std::thread(Player(), this, &(boardArray[threadId]), 0, recursionDepth, &(resultVector[threadId]), !amIWhite, best, beta,&evaluationCount), ChessBoard::InternalMove(*moveIterator));
				break;

			}
			catch (ChessBoard::FIFTY_MOVES)
			{
				resultVector[threadId].gameHasEnded = true;
				resultVector[threadId].isNull = false;
				resultVector[threadId].endState = 0;
				if (amIWhite)
				{
					if (!resultVector[threadId].isNull && (resultVector[threadId] > best))
					{
						best = resultVector[threadId];
						bestMove = threadVector[threadId].second;
						resultVector[threadId].isNull = true;
					}
				}
				else
				{
					if (!resultVector[threadId].isNull && (best.isNull || best.gameHasEnded || !resultVector[threadId].gameHasEnded) && (best.isNull || ((best.gameHasEnded && !resultVector[threadId].gameHasEnded) || resultVector[threadId] < best)))
					{

						best = resultVector[threadId];
						bestMove = threadVector[threadId].second;
						resultVector[threadId].isNull = true;
					}
				}

			}
			catch (ChessBoard::THREEFOLD_REPETITON)
			{
				resultVector[threadId].gameHasEnded = true;
				resultVector[threadId].isNull = false;
				resultVector[threadId].endState = 0;
				if (amIWhite)
				{
					if (!resultVector[threadId].isNull && (resultVector[threadId] > best))
					{
						best = resultVector[threadId];
						bestMove = threadVector[threadId].second;
						resultVector[threadId].isNull = true;
					}
				}
				else
				{
					if (!resultVector[threadId].isNull && (best.isNull || best.gameHasEnded || !resultVector[threadId].gameHasEnded) && (best.isNull || ((best.gameHasEnded && !resultVector[threadId].gameHasEnded) || resultVector[threadId] < best)))
					{

						best = resultVector[threadId];
						bestMove = threadVector[threadId].second;
						resultVector[threadId].isNull = true;
					}
				}

				boardArray[threadId][0].Revert();
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
			}
			catch (ChessBoard::INVALID_MOVE)
			{

			}
			catch (ChessBoard::KING_IN_DANGER)
			{

			}
			++moveIterator;
		}
	}
	for (int i = 0; i < maxThreadCount; i++)
		if (threadVector[i].first.joinable())
			threadVector[i].first.join();

	for (int i = 0; i < maxThreadCount; i++)
	{
		if (amIWhite)
		{
			if (!resultVector[i].isNull && (resultVector[i] > best))
			{
				best = resultVector[i];
				bestMove = threadVector[i].second;
				resultVector[i].isNull = true;
			}
		}
		else
		{
			if (!resultVector[i].isNull && (best.isNull || best.gameHasEnded || !resultVector[i].gameHasEnded) && (best.isNull || ((best.gameHasEnded && !resultVector[i].gameHasEnded) || resultVector[i] < best)))
			{

				best = resultVector[i];
				bestMove = threadVector[i].second;
				resultVector[i].isNull = true;
			}
		}

	}


	try
	{
		currBoardState->ChangeState(bestMove, 0);
	}
	catch (ChessBoard::THREEFOLD_REPETITON)
	{
#ifdef MAPDEBUG
		std::cout << "Threefold repetition!" << std::endl;
#endif // MAPDEBUG
		throw GAME_ENDED(true, false, false);
	}
	catch (ChessBoard::FIFTY_MOVES)
	{
#ifdef MAPDEBUG
		std::cout << "Fifty moves!" << std::endl;
#endif // MAPDEBUG
		throw GAME_ENDED(true, false, false);
	}


	Move tmp = bestMove.ConvertToExternal(amIWhite);
	boardArray.clear();

	return tmp;
}

void JudgeDredd::Valkyrie::Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const
{
#ifdef DEBUG
	std::cout << currentRecursion<<(currentRecursion!=maxRecursion?"----":"");
#endif // DEBUG
	if (currentRecursion == maxRecursion) 
	{
#ifdef DEBUG
		std::cout << std::endl;
#endif // DEBUG
		if(value!=nullptr)
			*value= evaluator.evaluate((*boardVector)[currentRecursion]);
		return;

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
		for (; *moveIterator != moveIterator.cend(); ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove =ChessBoard::InternalMove( *moveIterator);
				(*boardVector)[currentRecursion + 1].ChangeState(*moveIterator);
				Play(boardVector, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alpha, beta);


				(*boardVector)[currentRecursion + 1].Revert();
				StealmateFlag = false;

			}
			catch (ChessBoard::FIFTY_MOVES)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
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
			catch (ChessBoard::KING_IN_DANGER)
			{

			}

			if (newbestMove.from!= std::pair<short, short>(8, 0) &&!newBest.isNull&&(newBest > alpha))
			{
				alpha = (Best = newBest);
				bestMove = newbestMove;
			}

#ifdef MAPDEBUG
			if ((*boardVector)[currentRecursion + 1] != (*boardVector)[currentRecursion])
				throw std::runtime_error("MAP REVERSION FAILED!");
#endif

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
		for (; *moveIterator != moveIterator.cend(); ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove = *moveIterator;
				(*boardVector)[currentRecursion + 1].ChangeState(*moveIterator);
				Play(boardVector, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alpha, beta);
				StealmateFlag = false;

				(*boardVector)[currentRecursion + 1].Revert();
			}
			catch (ChessBoard::FIFTY_MOVES)
			{
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
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
			catch (ChessBoard::KING_IN_DANGER err) 
			{
				
			}

			if (newbestMove.from != std::pair<short, short>(8, 0) && !newBest.isNull&&(beta.isNull||beta.gameHasEnded||!newBest.gameHasEnded)&&(beta.isNull||((beta.gameHasEnded&&!newBest.gameHasEnded)||newBest < beta)))
			{

				beta = (Best = newBest);
				bestMove = newbestMove;
			}
#ifdef MAPDEBUG
			if ( (*boardVector)[currentRecursion + 1] != (*boardVector)[currentRecursion])
				throw std::runtime_error("MAP REVERSION FAILED!");
#endif

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
		Best.endState = (*boardVector)[currentRecursion + 1].IsWhiteChecked() ? -1 : ((*boardVector)[currentRecursion + 1].IsBlackChecked() ? 1 : 0);
		if (value != nullptr)
			*value = Best;
	}
	if(value!=nullptr)
		*value = isWhite ? alpha : beta;

}

