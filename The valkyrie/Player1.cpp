#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "Player1.h"
#include <thread>
#include <atomic>
#include <concurrent_queue.h>
#include <set>
#define CUT
//#define REVERSION
//#define MAPDEBUG




JudgeDredd::Valkyrie::Valkyrie(bool amIWhite)
{
	maxThreadCount =  std::thread::hardware_concurrency();
	boardVector = new ChessBoard::Board[maxThreadCount];
	threadVector = std::vector<std::thread>();
	threadVector.reserve(maxThreadCount);
	this->amIWhite = amIWhite;
	firstMove = amIWhite;
	maxThreadCount = std::thread::hardware_concurrency();
	for (int i = 0; i < maxThreadCount; i++)
	{
		boardVector[i] = *currBoardState;
		threadVector.push_back(std::thread(Player(), this, &boardVector[i], 0, recursionDepth, amIWhite, amIWhite ? &best : &alpha, amIWhite ? &beta : &best, &evaluationCount, &toEvaluate, &evaluated));
	}
}

JudgeDredd::Valkyrie::Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator, int recursion)
{
	maxThreadCount = std::thread::hardware_concurrency();
	boardVector = new ChessBoard::Board[maxThreadCount]();
	threadVector = std::vector<std::thread>();
	threadVector.reserve(maxThreadCount);
	amIWhite = amIwhite;
	firstMove = amIWhite;
	recursionDepth = recursion;
	this->evaluator = evaluator;
	for (int i = 0; i < maxThreadCount; i++)
	{
		boardVector[i] = *currBoardState;
		threadVector.push_back(std::thread(Player(), this, &boardVector[i], 0, recursionDepth, amIWhite, amIWhite ? &best : &alpha, amIWhite ? &beta : &best, &evaluationCount, &toEvaluate, &evaluated));
	}
}

JudgeDredd::Valkyrie::~Valkyrie()
{
	toEvaluate.finish();
	for (int i = 0; i < maxThreadCount; i++)
		if (threadVector[i].joinable())
			threadVector[i].join();
	delete[] boardVector;
	delete currBoardState;
}

Move JudgeDredd::Valkyrie::makeMove(Move lastMove)
{
	best.isNull = true;
	if (!firstMove)
	{
		ChessBoard::InternalMove tmp=ChessBoard::ConvertFromExternal(lastMove);
		currBoardState->ChangeState(tmp, 0);
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


	Move tmp = ChessBoard::convertToExternal(bestMove, amIWhite);

	return tmp;
}

void JudgeDredd::Valkyrie::Play(ChessBoard::Board &board, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite, ChessEvaluator::ChessEvaluation * alphaPointer, ChessEvaluator::ChessEvaluation * betaPointer, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const
{
#ifdef DEBUG
	std::cout << currentRecursion << (currentRecursion != maxRecursion ? "----" : "");
#endif // DEBUG
	if (currentRecursion == maxRecursion)
	{
#ifdef DEBUG
		std::cout << std::endl;
#endif // DEBUG
		if (value != nullptr)
			*value = evaluator.evaluate(board);
		return;

	}


	ChessEvaluator::ChessEvaluation Best, newBest;
	ChessBoard::InternalMove bestMove;
	ChessBoard::InternalMove newbestMove;
	bool StealmateFlag = true;
	ChessBoard::Board::Moves moveIterator(&board);

	moveIterator.Reset(isWhite);
	++(moveIterator);
#ifdef REVERSION
	ChessBoard::Board tmp(board);
#endif

	if (isWhite)
	{
		for (; *moveIterator != moveIterator.cend(); ++moveIterator)
		{
			newBest.isNull = true;
			newbestMove = ChessBoard::InternalMove(*moveIterator);
			switch (board.ChangeState(*moveIterator))
			{
			case ChessBoard::Success:
				Play(board, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alphaPointer, betaPointer, alpha, beta);
				board.revert();
				StealmateFlag = false;
				break;
			case ChessBoard::Revert:
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
				board.revert();
				break;
			case ChessBoard::NoAction:
				break;
			case ChessBoard::WrongColor:
				throw std::runtime_error("Wrong Color!");
			}


			if (alpha < *alphaPointer)
				alpha = *alphaPointer;
			if (!betaPointer->isNull && (beta.isNull || beta.gameHasEnded || !betaPointer->gameHasEnded) && (beta.isNull || ((beta.gameHasEnded && !betaPointer->gameHasEnded) || *betaPointer < beta)))
				beta = *betaPointer;

			if (newbestMove != moveIterator.cend() && !newBest.isNull && (newBest > alpha))
			{
				alpha = (Best = newBest);
				bestMove = newbestMove;
			}

			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
				break;
		}
	}
	else
	{
		for (; *moveIterator != moveIterator.cend(); ++moveIterator)
		{

			newBest.isNull = true;
			newbestMove = *moveIterator;

			switch (board.ChangeState(*moveIterator))
			{
			case ChessBoard::Success:
				Play(board, currentRecursion + 1, maxRecursion, &newBest, !isWhite, alphaPointer, betaPointer, alpha, beta);
				board.revert();
				StealmateFlag = false;
				break;
			case ChessBoard::Revert:
				newBest.gameHasEnded = true;
				newBest.isNull = false;
				newBest.endState = 0;
				board.revert();
				break;
			case ChessBoard::NoAction:
				break;
			case ChessBoard::WrongColor:
				throw std::runtime_error("Wrong Color!");
			}

			if (alpha < *alphaPointer)
				alpha = *alphaPointer;
			if (!betaPointer->isNull && (beta.isNull || beta.gameHasEnded || !betaPointer->gameHasEnded) && (beta.isNull || ((beta.gameHasEnded && !betaPointer->gameHasEnded) || *betaPointer < beta)))
				beta = *betaPointer;


			if (newbestMove != moveIterator.cend() && !newBest.isNull && (beta.isNull || beta.gameHasEnded || !newBest.gameHasEnded) && (beta.isNull || ((beta.gameHasEnded && !newBest.gameHasEnded) || newBest < beta)))
			{

				beta = (Best = newBest);
				bestMove = newbestMove;
			}
			if (!beta.isNull && !alpha.isNull&&beta <= alpha)
				break;
		}
	}
	if (StealmateFlag)
	{
		Best.isNull = false;
		Best.gameHasEnded = true;
		Best.endState = board.IsWhiteChecked() ? -1 : board.IsBlackChecked() ? 1 : 0;
		if (value != nullptr)
			*value = Best;
	}
	if (value != nullptr)
		*value = isWhite ? alpha : beta;

}

JudgeDredd::Valkyrie::KillerInstinct::KillerInstinct(short depth)
{
	vector.resize(depth);
}

void JudgeDredd::Valkyrie::KillerInstinct::operator++()
{
	delete vector[0];
	delete vector[1];
	for (int i = 0; i < vector.size() - 2; i++)
	{
		vector[i] = vector[i + 2];
	}
	vector[vector.size() - 2] = new std::set<ChessBoard::InternalMove>();
	vector[vector.size() - 1] = new std::set<ChessBoard::InternalMove>();
	return *this;
}

bool JudgeDredd::Valkyrie::KillerInstinct::contains(short depth, ChessBoard::InternalMove & move)
{
	return vector[depth]->find(move) != vector[depth]->end();
}

void JudgeDredd::Valkyrie::KillerInstinct::add(short depth, ChessBoard::InternalMove & move)
{
	vector[depth]->insert(move);
}

void JudgeDredd::Valkyrie::KillerInstinct::fillBuffer(short depth, std::vector<ChessBoard::InternalMove>& buffer, std::set<ChessBoard::InternalMove>::iterator & from)
{
	lock.lock();
	for (int i = 0; i < 5 && from != vector[depth]->end(); i++, from++)
		buffer.push_back(*from);
	lock.unlock();
}


