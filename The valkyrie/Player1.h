#ifndef PLAYER1_H_INCLUDED
#define PLAYER1_H_INCLUDED

#include "chessboard.h"
#include "judgeDredd.h"
#include <vector>
#include <atomic>
#include "Evaluator.h"
#include <concurrent_queue.h>
#include <thread>
#include <queue>
#include <atomic>
#include <condition_variable>
#include "ConcurentQueue.h"
#define PROGRAM_NAME "valkyrie"
//#define REVERSION
#pragma once


class JudgeDredd::Valkyrie
{
	int x;

public:
	Valkyrie(bool amIWhite);
	Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator = { .1f,.1f,.1f,.1f,.1f }, int recursion = 5);
	~Valkyrie();
	struct Move makeMove(struct Move lastMove);
	class Player
	{
	public:
		//Valkyrie *ref;
		//Player(Valkyrie * ref)
		//{
		//	this->ref = ref;
		//}
		void operator()(Valkyrie * ref, ChessBoard::Board *board, short int currentRecursion, short int maxRecursion, bool isWhite, ChessEvaluator::ChessEvaluation * alpha, ChessEvaluator::ChessEvaluation * beta, std::atomic<int>*counter, ConcurrentQueue<ChessBoard::InternalMove> *toEvaluate, Concurrency::concurrent_queue<std::pair<ChessBoard::InternalMove, ChessEvaluator::ChessEvaluation>> *evaluated)
		{
			ChessBoard::InternalMove buffer;
			ChessEvaluator::ChessEvaluation tmpEval;
			while (toEvaluate->pop(buffer))
			{


				tmpEval.isNull = true;
				switch (board->ChangeState(buffer))
				{
				case ChessBoard::Success:
					ref->Play(*board, currentRecursion, maxRecursion, &tmpEval, !isWhite, alpha, beta, *alpha, *beta);
					board->revert();
					break;
				case ChessBoard::Revert:
					tmpEval.gameHasEnded = true;
					tmpEval.isNull = false;
					tmpEval.endState = 0;
					board->revert();
					break;
				case ChessBoard::NoAction:
					break;
				case ChessBoard::WrongColor:
					throw std::runtime_error("Wrong Color!");
				}
					if (!tmpEval.isNull)
						evaluated->push(std::pair<ChessBoard::InternalMove, ChessEvaluator::ChessEvaluation>(buffer, tmpEval));
					*counter -= 1;
			}
		}
	};
private:
	std::vector<std::thread> threadVector;
	ChessBoard::Board *boardVector;
	ConcurrentQueue<ChessBoard::InternalMove> toEvaluate;
	Concurrency::concurrent_queue<std::pair<ChessBoard::InternalMove, ChessEvaluator::ChessEvaluation>> evaluated;
	ChessEvaluator::ChessEvaluation alpha, beta;
	std::atomic<int>evaluationCount = 0;
	ChessEvaluator::ChessEvaluation best;
	short recursionDepth = 5;
	short maxThreadCount;
	bool firstMove;
	ChessBoard::Board *currBoardState = new ChessBoard::Board();
	bool amIWhite;
	void Play(ChessBoard::Board &board, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite, ChessEvaluator::ChessEvaluation * alphaPointer, ChessEvaluator::ChessEvaluation * betaPointer, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const;
	ChessEvaluator::ChessEvaluator evaluator;

};

class GAME_ENDED :std::exception
{
public:
	GAME_ENDED(bool tie, bool white, bool black)
	{
		this->tie = tie;
		this->blackWon = black;
		this->whiteWon = white;
	}
	bool tie;
	bool whiteWon;
	bool blackWon;
};

#endif // PLAYER1_H_INCLUDED