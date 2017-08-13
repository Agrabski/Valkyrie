#pragma once

#include <stdlib.h>
#include <vector>
#include "judgeDredd.h"
#include <math.h>

namespace ChessBoard
{
	enum Type { Pawn = 2, Knight = 4, Bishop = 5, Tower = 10, Queen = 15, King = 1, Empty = 0 };

	enum MoveTypes { Standard, EnPassan, PromotionQueen, PromotionTower, PromotionKnight, PromotionBishop, RochadeLeft, RochadeRight };
	struct InternalMove
	{
		InternalMove(Move toConvert);
		InternalMove();
		InternalMove(std::pair<short, short> from, std::pair<short, short> to, MoveTypes type)
		{
			this->from = from;
			this->to = to;
			this->movetype = type;
		}
		std::pair<short, short> from;
		std::pair<short, short> to;
		MoveTypes movetype;
		Move ConvertToExternal(bool amIWhite);
	};

	int sign(int x);

	struct Rank
	{
		Type type;
		bool isWhite;
		bool const operator!=(const Rank& right);
	};
	struct Field
	{
		Rank rank;
		int coveredByWhite = 0;
		int coveredByBlack = 0;
	};
	struct Board
	{
		Board() :moveIterator(this)
		{
			for (int y = 0; y < 8; y++)
				for (int x = 0; x < 8; x++)
					fields[x][y].rank = { Empty, false };

			for (int x = 0; x < 8; x++)
			{
				fields[x][1].rank = { Pawn,true };
				fields[x][6].rank = { Pawn,false };
			}
			fields[0][0].rank = { Tower,true };
			fields[7][0].rank = { Tower,true };
			fields[0][7].rank = fields[7][7].rank = { Tower,false };
			fields[1][0].rank = fields[6][0].rank = { Knight,true };
			fields[1][7].rank = fields[6][7].rank = { Knight,false };
			fields[2][0].rank = fields[5][0].rank = { Bishop,true };
			fields[2][7].rank = fields[5][7].rank = { Bishop,false };
			fields[3][0].rank = { Queen,true };
			fields[3][7].rank = { Queen,false };
			fields[4][0].rank = { King,true };
			fields[4][7].rank = { King,false };
		}
		const std::pair<short, short> QueenMovementArray[8] = { { -1,0 },{ -1,1 },{ 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 } };
		const std::pair<short, short> KnightMovementArray[8] = { { -2,-1 },{ -2,1 },{ -1,2 },{ 1,2 },{ 2,1 },{ 2,-1 },{ -2,1 },{ -2,-1 } };
		void ChangeState(InternalMove lastMove);
		bool operator==(Board& right);
		void PaintTheMap(InternalMove lastMove);
		void PaintTheMap();
		void Revert(bool isWhite);
		void removeBoard(Board board);
		Field fields[8][8];
		std::vector<std::pair<Board, int>> prevBoard;
		bool nextMoveIsWhite = true;
		bool leftWhite = true;
		bool rightWhite = true;
		bool leftBlack = true;
		bool rightBlack = true;
		struct Moves
		{
			Moves(Board *Parent)
			{
				this->parent = parent;
			}
			Moves();
			Moves& operator++();
			const InternalMove* operator*();
			void Reset(bool isWhite);
		private:
			std::pair<short, short>currentRank;
			InternalMove * state;
			bool isWhite;
			short direction = 0;
			short moveIterator = 0;
			Board * parent;
		};
		Moves moveIterator;
		Board& operator=(const Board & toCopy);
	private:
		//returns true if game is not over
		bool addBoard();
		InternalMove lastMove;
		Rank Beaten;
	};

	class INVALID_MOVE : std::exception
	{

	};

	class KING_IN_DANGER :INVALID_MOVE
	{
	public:
		bool isBlack;
		bool isWhite;
		KING_IN_DANGER(bool black, bool white)
		{
			isBlack = black;
			isWhite = white;
		}
	};

	class THREEFOLD_REPETITON : INVALID_MOVE
	{

	};

	class MOVE_BLOCKED : INVALID_MOVE
	{

	};

	class WRONG_COLOR : INVALID_MOVE
	{
		
	};
}