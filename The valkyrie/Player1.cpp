#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"
#include <thread>
#include <atomic>
#include <concurrent_queue.h>
#define CUT
//#define REVERSION
//#define MAPDEBUG

JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	maxThreadCount = std::thread::hardware_concurrency();
	threadVector = std::vector<std::thread>();
	threadVector.reserve(maxThreadCount);
	this->amIWhite = amIWhite;
	firstMove = amIWhite;
	maxThreadCount =  std::thread::hardware_concurrency();
	for (int i = 0; i < maxThreadCount; i++)
	{
		boardVector.emplace_back((*currBoardState));
		threadVector.push_back(std::thread(Player(), this, i, 0, recursionDepth, amIWhite, amIWhite ? &best : &alpha, amIWhite ? &beta : &best, &evaluationCount, &toEvaluate, &evaluated));
	}
}

JudgeDredd::Valkyrie::Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator, int recursion)
{
	maxThreadCount =  std::thread::hardware_concurrency();
	threadVector=std::vector<std::thread>();
	threadVector.reserve(maxThreadCount);
	amIWhite = amIwhite;
	firstMove = amIWhite;
	recursionDepth = recursion;
	this->evaluator = evaluator;
	for (int i = 0; i < maxThreadCount; i++)
	{
		boardVector.emplace_back((*currBoardState));
		threadVector.push_back(std::thread( Player(), this, i, 0, recursionDepth, amIWhite, amIWhite ? &best : &alpha, amIWhite ? &beta : &best, &evaluationCount, &toEvaluate, &evaluated));
	}
}

JudgeDredd::Valkyrie::~Valkyrie()
{
	toEvaluate.finish();
	for (int i = 0; i < maxThreadCount; i++)
		if (threadVector[i].joinable())
			threadVector[i].join();
	delete currBoardState;
}

Move JudgeDredd::Valkyrie::makeMove(Move lastMove)
{
	best.isNull = true;
	if (evaluationCount.load() != 0)
		throw std::runtime_error("too many threads");
	if (!firstMove)
	{
		ChessBoard::InternalMove tmp(lastMove);
		currBoardState->ChangeState(tmp, 0);
		if (evaluationCount != 0)
		{
			std::cout << "error";
			throw std::runtime_error("threads running!");
		}
		for (int i = 0; i < maxThreadCount; i++)
		{
			boardVector[i].ChangeState(tmp, 0);

		}
	}
	evaluationCount = 0;
	firstMove = false;
	volatile bool completionFlag = false;
	ChessBoard::InternalMove bestMove;
	ChessBoard::Board::Moves moveIterator(currBoardState);
	moveIterator.Reset(amIWhite);
	++moveIterator;



	while (*moveIterator != moveIterator.cend())
	{
		toEvaluate.push(*moveIterator);
		++moveIterator;
		evaluationCount++;
	}

	std::pair<ChessBoard::InternalMove, ChessEvaluator::ChessEvaluation> pairBuffer;
	while (evaluationCount > 0 || !evaluated.empty())
	{
		if (evaluated.try_pop(pairBuffer))
		{
			if (amIWhite)
			{
				if (!pairBuffer.second.isNull && (pairBuffer.second > best))
				{
					best = pairBuffer.second;
					bestMove = pairBuffer.first;
				}
			}
			else
			{
				if (!pairBuffer.second.isNull && (best.isNull || best.gameHasEnded || !pairBuffer.second.gameHasEnded) && (best.isNull || ((best.gameHasEnded && !pairBuffer.second.gameHasEnded) || pairBuffer.second < best)))
				{
					best = pairBuffer.second;
					bestMove = pairBuffer.first;
				}
			}

		}
	}

	if (best.isNull)
		throw GAME_ENDED((!currBoardState->IsWhiteChecked()) && (!currBoardState->IsBlackChecked()), currBoardState->IsWhiteChecked(), currBoardState->IsBlackChecked());

		
	try
	{
		currBoardState->ChangeState(bestMove, 0);
		for (int i = 0; i < maxThreadCount; i++)
		{
			boardVector[i].ChangeState(bestMove, 0);
		}
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

	return tmp;
}

void JudgeDredd::Valkyrie::Play(ChessBoard::BoardConcurencyLock &board, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const
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
			*value= evaluator.evaluate(*board);
		return;

	}


	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove;
	ChessBoard::InternalMove newbestMove;
	bool StealmateFlag = true;
	ChessBoard::Board::Moves moveIterator(*board);

	moveIterator.Reset(isWhite);
	++(moveIterator);
#ifdef REVERSION
	ChessBoard::Board tmp(board);
#endif

	if (isWhite)
	{
		for (; *moveIterator != moveIterator.cend(); ++moveIterator)
		{
			try
			{
				newBest.isNull = true;
				newbestMove =ChessBoard::InternalMove( *moveIterator);
#ifdef REVERSION
				if (tmp != board)
					throw std::runtime_error("reversion fail");
#endif
				board.ChangeState(*moveIterator);

				Play(board, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alpha, beta);


				board.Revert();
#ifdef REVERSION
				if (tmp != board)
					throw std::runtime_error("reversion fail");
#endif
				StealmateFlag = false;

			}
			catch (ChessBoard::KING_IN_DANGER)
			{

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
				board.Revert();
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
#ifdef REVERSION
				if (tmp != board)
					throw std::runtime_error("reversion fail");
#endif
				board.ChangeState(*moveIterator);
				Play(board, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alpha, beta);
				StealmateFlag = false;

				board.Revert();
#ifdef REVERSION
				if (tmp != board)
					throw std::runtime_error("reversion fail");
#endif
			}
			catch (ChessBoard::KING_IN_DANGER err) 
			{
				
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
				board.Revert();
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
			}
			catch (ChessBoard::INVALID_MOVE)
			{

			}

#ifdef REVERSION
			if (tmp != board)
				throw std::runtime_error("reversion fail");
#endif



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
		Best.endState = (board.isWhiteChecked() ? -1 : (board.isBlackChecked() ? 1 : 0));
		if (value != nullptr)
			*value = Best;
	}
	if(value!=nullptr)
		*value = isWhite ? alpha : beta;

}

