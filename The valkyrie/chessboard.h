#pragma once

#include <stdlib.h>
#include <vector>
#include "judgeDredd.h"
#include <math.h>
#include <stack>
#include <array>
#include <unordered_map>
#include <mutex>
#define CONCURENCYDEBUG

typedef std::pair<short, short> point;

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
		bool operator!=(const InternalMove& other) const;
	};

	int sign(int x);

	struct Rank
	{
		Rank(Type type, bool IsWhite)
		{
			this->type = type;
			this->isWhite = IsWhite;
		}
		Rank()
		{

		}
		Type type;
		bool isWhite;
		bool operator!=(const Rank& right) const;
		bool operator==(const Rank& right) const;
	};

	struct Field
	{
		Rank rank;
		int coveredByWhite = 0;
		int coveredByBlack = 0;
		bool operator==(const Field &right) const;
		bool operator!=(const Field &right) const;
	};


	class Board
	{
	public:
		Board()
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
			MoveStack = std::stack<StackElement>();
			prevBoard.reserve(50);
			currentHash = PrevBoardElement::CreateHash(fields);
		}
		~Board();
		Board(const Board*toCopy);
		void ChangeState(InternalMove lastMove);
		void ChangeState(InternalMove lastMove, int);
		bool operator==(const Board& right) const;
		bool operator!=(const Board& right) const;
		void Revert();
		Field fields[8][8];
		struct Moves
		{
			Moves(const Board *Parent)
			{
				this->parent = Parent;
			}
			~Moves();
			Moves();
			Moves& operator++();
			const InternalMove& operator*();
			const InternalMove& cend();
			void Reset(bool isWhite);
		private:
			static const InternalMove endState;
			std::pair<short, short>currentRank;
			InternalMove  state;
			bool isWhite;
			short direction = 0;
			short moveIterator = 0;
			const Board * parent;
		};
		Board& operator=(const Board & toCopy);
		bool IsBlackChecked() const;
		bool IsWhiteChecked() const;

	private:
		struct PrevBoardElement
		{
		public:
			typedef unsigned long long int hashType;
			PrevBoardElement(const Field toCopy[8][8], short count);
			PrevBoardElement();
			Field map[8][8];
			short count;
			static hashType CreateHash(const Field toHash[8][8]);
			static hashType ReHash(hashType old, InternalMove move, bool isWhite);
			PrevBoardElement operator++();
			PrevBoardElement operator--();
			static hashType shift(short x, short y);
			bool operator==(const Field right[8][8]) const;
			bool operator==(const PrevBoardElement&right)const;
		};

		struct StackElement
		{
			StackElement(InternalMove move, Rank piece, int moves, bool lWhite, bool rWhite, bool lBlack, bool rBlack, unsigned long long int hash)
			{
				this->move = move;
				pieceType = piece;
				movesCounter = moves;
				leftWhite = lWhite;
				rightWhite = rWhite;
				leftBlack = lBlack;
				rightBlack = rBlack;
				this->hash = hash;
			}
			unsigned long long int hash;
			InternalMove move;
			Rank pieceType;
			int movesCounter;
			bool leftWhite;
			bool rightWhite;
			bool leftBlack;
			bool rightBlack;
		};

		//returns true if game is not over
		int moveCounter = 100;
		bool addBoard();
		void ClearData();
		void removeBoard(Field board[8][8]);
		void Board::PaintTheMap(InternalMove lastMove, Rank currentlyMoved);
		void PaintTheMap();
		bool nextMoveIsWhite = true;
		bool leftWhite = true;
		bool rightWhite = true;
		bool leftBlack = true;
		bool rightBlack = true;
		bool blackCheck = false;
		bool whiteCheck = false;
		void ClearStack();
		std::unordered_multimap<PrevBoardElement::hashType, PrevBoardElement>  prevBoard;
		std::stack<StackElement> MoveStack;
		static const std::pair<short, short> QueenMovementArray[8];
		static const std::pair<short, short> KnightMovementArray[8];
		PrevBoardElement::hashType currentHash;

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

	class FIFTY_MOVES : INVALID_MOVE
	{

	};

}