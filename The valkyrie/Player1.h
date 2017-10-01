#ifndef PLAYER1_H_INCLUDED
#define PLAYER1_H_INCLUDED

#include "chessboard.h"
#include "judgeDredd.h"
#include <vector>
#include <atomic>
#include "Evaluator.h"
#define PROGRAM_NAME "valkyrie"
#pragma once

class JudgeDredd::Valkyrie
{
    int x;

    public:
    Valkyrie(bool amIWhite);
	Valkyrie(bool amIwhite, ChessEvaluator::ChessEvaluator evaluator = {.1f,.1f,.1f,.1f}, int recursion = 5);
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
			void operator()(Valkyrie * ref,std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite, ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta, std::atomic<int>*counter)
			{
				ref->Play(boardVector, currentRecursion, maxRecursion, value, isWhite, alpha, beta);
				*counter += 1;
			}
		};
	private:
		short recursionDepth=5;
		short maxThreadCount;
		bool firstMove;
		ChessBoard::Board *currBoardState=new ChessBoard::Board();
		bool amIWhite;
		void Play(std::vector< ChessBoard::Board> *boardVector, short int currentRecursion, short int maxRecursion, ChessEvaluator::ChessEvaluation *value, bool isWhite,ChessEvaluator::ChessEvaluation alpha, ChessEvaluator::ChessEvaluation beta) const;
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
