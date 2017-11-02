#include "Chessboard.h"
#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#pragma once

#pragma region MOVE_MACROS
#define MOVE_CHECK_MACRO(x,y) ((currentRank.first + x<8&&currentRank.first + x>0&&currentRank.second + y<8&&currentRank.second + y>0&&(parent->fields[currentRank.first + x][currentRank.second + y].rank.type == Empty || parent->fields[currentRank.first + x][currentRank.second + y].rank.isWhite != isWhite)))
#define MOVE_MACRO(x,y) 	state=MakeMove(currentRank, std::pair<short, short>(currentRank.first +x, currentRank.second +y), Standard);
#define COMPLETE_MOVE_MACRO(x,y) if(MOVE_CHECK_MACRO(x,y)) {MOVE_MACRO(x,y)} else { direction++; moveIterator = 0; return ++(*this);}
#pragma endregion

#pragma region SAFETY_MACROS
#define IN_RANGE(x) (0<=x&&x<8)
#pragma endregion


#ifndef PROGRAM_NAME
#define PROGRAM_NAME "Valkirye"
#endif // !PROGRAM_NAME

#define DEBUG

namespace ChessBoard
{
	auto invert = [](int i) {return i - 4 > 0 ? i - 4 : 4 + i; };

	const InternalMove Board::Moves::endState = ((InternalMove)0) - 1;

	const std::pair<short, short> Board::KnightMovementArray[8] = { { -2,-1 },{ -2,1 },{ -1,2 },{ 1,2 },{ 2,1 },{ 2,-1 },{ 1,-2 },{ -1,-2 } };

	const std::pair<short, short> Board::QueenMovementArray[8] = { std::pair<short, short>(-1,0),std::pair<short, short>(-1,1),std::pair<short, short>(0,1),std::pair<short, short>(1,1),std::pair<short, short>(1,0),std::pair<short, short>(1,-1),std::pair<short, short>(0,-1),std::pair<short, short>(-1,-1) };

	InternalMove MakeMove(std::pair<short, short>from, std::pair<short, short>to, short type)
	{
		InternalMove result = 0;
		result |= from.first << fromXShift;
		result |= from.second << fromYShift;
		result |= to.first << toXShift;
		result |= to.second << toYShift;
		result |= type;
		return result;
	}

	Move convertToExternal(InternalMove move, bool amIWhite)
	{
		Move returnValue;
		returnValue.whiteMove = amIWhite;
		//TODO: hnadle conversion to external
		if (returnValue.special = (move&TypeMask) != Standard)
			switch (move&TypeMask)
			{
			case(PromotionBishop):
				returnValue.type = 'B';
				break;
			case(PromotionKnight):
				returnValue.type = 'K';
				break;
			case(PromotionQueen):
				returnValue.type = 'Q';
				break;
			case(PromotionTower):
				returnValue.type = 'T';
				break;
			case(RochadeLeft):
				returnValue.type = 'L';
				return returnValue;
			case(RochadeRight):
				returnValue.type = 'R';
				return returnValue;
			default:
				throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Generated move was corrupted (invalid internal move type)"));
				break;
			}
		returnValue.from.first = (move&fromXMask) >> fromXShift;
		returnValue.from.second = (move&fromYMask)>>fromYShift;
		returnValue.to.first = (move&toXMask) >> toXShift;
		returnValue.to.second = (move&toYMask) >> toYShift;
		return returnValue;

	}

	InternalMove ConvertFromExternal(Move toConvert)
	{
		InternalMove result = 0;
		result |= ((InternalMove)toConvert.from.first) << 13;
		result |= ((InternalMove)toConvert.from.second) << 10;
		result |= ((InternalMove)toConvert.to.first) << 7;
		result |= ((InternalMove)toConvert.to.second) << 4;
		if (toConvert.special)
			switch (toConvert.type)
			{
			case('L'):
				result |= RochadeLeft;
				break;
			case('R'):
				result |= RochadeRight;
				break;
			case('B'):
				result |= PromotionBishop;
				break;
			case('K'):
				result |= PromotionKnight;
				break;
			case('Q'):
				result |= PromotionQueen;
				break;
			case('T'):
				result |= PromotionTower;
				break;
			default:
				throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Recieved move was corrupted (external move type was invalid)"));
				break;
			}
		else
			result |= Standard;
		return result;
	}

	int sign(int x)
	{
		if (x > 0)
			return 1;
		else
			if (x == 0)
				return 0;
			else
				return -1;
	}

	bool Board::addBoard()
	{
		auto tmp = prevBoard.equal_range(currentHash);

		if (tmp.first == prevBoard.end())
		{
			prevBoard.insert(std::pair<PrevBoardElement::hashType, PrevBoardElement>(currentHash, PrevBoardElement(fields, 1)));
			return true;
		}
		else
		{
			while (tmp.first != tmp.second)
			{
				if (tmp.first->second == fields)
				{
					(tmp.first->second).count += 1;
					return tmp.first->second.count < 3;
				}
				tmp.first++;
			}
		}
		prevBoard.insert(std::pair<PrevBoardElement::hashType, PrevBoardElement>(currentHash, PrevBoardElement(fields, 1)));
		return true;
	}


	Board & Board::operator=(const Board & toCopy)
	{
		if (this == &toCopy)
			return *this;
		ClearData();
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				fields[x][y] = toCopy.fields[x][y];


		prevBoard = toCopy.prevBoard;

		//prevBoard = std::vector<std::pair<std::vector<std::vector<Field>>, int>>(toCopy.prevBoard);
		moveCounter = toCopy.moveCounter;
		MoveStack = std::stack<StackElement>(toCopy.MoveStack);
		nextMoveIsWhite = toCopy.nextMoveIsWhite;
		leftWhite = toCopy.leftWhite;
		rightWhite = toCopy.rightWhite;
		leftBlack = toCopy.leftBlack;
		rightBlack = toCopy.rightBlack;
		return *this;
	}

	bool Board::IsBlackChecked() const
	{
		return blackCheck;
	}

	bool Board::IsWhiteChecked() const
	{
		return whiteCheck;
	}

	void Board::ClearStack()
	{
		while (!MoveStack.empty())
		{
			MoveStack.pop();
		}
	}

	ChessBoard::KING_IN_DANGER ChessBoard::Board::PaintTheMap()
	{
		bool blackCheck = false;
		bool whiteCheck = false;
		for (int x = 0; x<8; x++)
			for (int y = 0; y < 8; y++)
			{
				fields[x][y].coveredByBlack = 0;
				fields[x][y].coveredByWhite = 0;
			}
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				bool isWhite = fields[x][y].rank.isWhite;
				switch (fields[x][y].rank.type)
				{
				case(Pawn):
				{
					if (fields[x][y].rank.isWhite)
					{
						if (x + 1 < 8 && y + 1 < 8)
						{
							fields[x + 1][y + 1].coveredByWhite += 1;
							if (fields[x + 1][y + 1].rank.type == King && !fields[x + 1][y + 1].rank.isWhite)
								blackCheck = true;
						}

						if (x - 1 >= 0 && y + 1 < 8)
						{
							fields[x - 1][y + 1].coveredByWhite += 1;
							if (fields[x - 1][y + 1].rank.type == King && !fields[x - 1][y + 1].rank.isWhite)
								blackCheck = true;
						}

					}
					else
					{
						if (x + 1 < 8 && y - 1 >= 0)
						{
							fields[x + 1][y - 1].coveredByBlack += 1;
							if (fields[x + 1][y - 1].rank.type == King && fields[x + 1][y - 1].rank.isWhite)
								whiteCheck = true;
						}
						if (x - 1 >= 0 && y - 1 >= 0)
						{
							fields[x - 1][y - 1].coveredByBlack += 1;
							if (fields[x - 1][y - 1].rank.type == King && fields[x - 1][y - 1].rank.isWhite)
								whiteCheck = true;
						}
					}
					break;
				}
				case(Queen):
				{
					for (int direction = 0; direction < 8; ++direction)
					{
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i >= 0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i >= 0 && y + QueenMovementArray[direction].second*i < 8; ++i)
						{
							if (isWhite)
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByWhite += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
									blackCheck = true;
							}
							else
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByBlack += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
								{
									whiteCheck = true;
								}
							}
							if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type != Empty)
							{
								break;
							}
						}
					}
					break;
				}
				case(Knight):
				{
					for (int direction = 0; direction < 8; ++direction)
					{
						if (x + KnightMovementArray[direction].first >= 0 && x + KnightMovementArray[direction].first < 8 && y + KnightMovementArray[direction].second >= 0 && y + KnightMovementArray[direction].second < 8)
						{
							if (isWhite)
							{
								fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].coveredByWhite += 1;
								if (fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].rank == Rank(King, false))
									blackCheck = true;
							}
							else
							{
								fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].coveredByBlack += 1;
								if (fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].rank == Rank(King, true))
									whiteCheck = true;
							}
						}
					}
					break;
				}
				break;
				case(Tower):
				{
					for (int direction = 0; direction < 8; direction += 2)
					{
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i >= 0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i >= 0 && y + QueenMovementArray[direction].second*i < 8; ++i)
						{
							if (isWhite)
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByWhite += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
									blackCheck = true;
							}
							else
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByBlack += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
									whiteCheck = true;
							}
							if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type != Empty)
								break;
						}
					}
					break;
				}
				break;
				case(King):
					break;
				case(Bishop):
				{
					for (int direction = 1; direction < 8; direction += 2)
					{
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i >= 0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i >= 0 && y + QueenMovementArray[direction].second*i < 8; ++i)
						{
							if (isWhite)
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByWhite += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
									blackCheck = true;
							}
							else
							{
								fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].coveredByBlack += 1;
								if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type == King&&fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.isWhite != isWhite)
									whiteCheck = true;
							}
							if (fields[x + QueenMovementArray[direction].first*i][y + QueenMovementArray[direction].second*i].rank.type != Empty)
								break;
						}
					}

				}
				break;
				}
			}
		}
		return KING_IN_DANGER(blackCheck, whiteCheck);
	}

	void ChessBoard::Board::revert()
	{

		nextMoveIsWhite = !nextMoveIsWhite;
		removeBoard(fields);
		StackElement tmp = MoveStack.top();
		moveCounter = tmp.movesCounter;
		InternalMove lastMove = tmp.move;
		Rank beaten = tmp.pieceType;
		rightBlack = tmp.rightBlack;
		leftBlack = tmp.leftBlack;
		rightWhite = tmp.rightWhite;
		leftWhite = tmp.leftWhite;
		MoveStack.pop();
		currentHash = tmp.hash;
		if ((lastMove&TypeMask) == Standard)
		{
			fields[(lastMove&fromXMask) >> fromXShift][(lastMove&fromYMask) >> fromYShift].rank = fields[(lastMove&toXMask) >> toXShift][(lastMove&toYMask) >> toYShift].rank;
			fields[(lastMove&toXMask) >> toXShift][(lastMove&toYMask) >> toYShift].rank = beaten;

			KING_IN_DANGER err = PaintTheMap();

			blackCheck = err.isBlack;
			whiteCheck = err.isWhite;
			return;
		}
		switch (lastMove&TypeMask)
		{
		case(PromotionKnight):
		case(PromotionBishop):
		case(PromotionTower):
		case(PromotionQueen):
			fields[(lastMove&fromXMask)>>fromXShift][(lastMove&fromYMask)>>fromYShift].rank.type = Pawn;
			fields[(lastMove&toXMask)>>toXShift][(lastMove&toYMask)>>toYShift].rank = beaten;
			break;
		case(RochadeLeft):
			if (nextMoveIsWhite)
			{
				fields[0][0].rank = { Tower, true };
				fields[4][0].rank = { King,true };
				fields[2][0].rank = { Empty,true };
				fields[3][0].rank = { Empty,true };
			}
			else
			{
				fields[0][7].rank = { Tower, false };
				fields[4][7].rank = { King,false };
				fields[2][7].rank = { Empty,false };
				fields[3][7].rank = { Empty,false };
			}
			break;
		case(RochadeRight):
			if (nextMoveIsWhite)
			{
				fields[7][0].rank = { Tower, true };
				fields[4][0].rank = { King,true };
				fields[6][0].rank = { Empty,true };
				fields[5][0].rank = { Empty,true };
			}
			else
			{
				fields[0][7].rank = { Tower, false };
				fields[4][7].rank = { King,false };
				fields[6][7].rank = { Empty,false };
				fields[5][7].rank = { Empty,false };
			}
			break;
		default:
			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Storred move type was corrupted (invalid internal move type)"));
		}
		blackCheck = false;
		whiteCheck = false;

		KING_IN_DANGER err = PaintTheMap();
		blackCheck = err.isBlack;
		whiteCheck = err.isWhite;
	}

	void Board::ClearData()
	{
		ClearStack();
		prevBoard.clear();
	}

	void Board::removeBoard(Field board[8][8])
	{
		std::pair<std::unordered_multimap<PrevBoardElement::hashType, PrevBoardElement>::iterator, std::unordered_multimap<PrevBoardElement::hashType, PrevBoardElement>::iterator> tmp = prevBoard.equal_range(currentHash);

		if (tmp.first == prevBoard.end())
		{
			throw std::runtime_error("map not found");
		}

		else
		{
			while (tmp.first != tmp.second)
			{
				if (tmp.first->second == board)
				{
					tmp.first->second.count -= 1;
					if (tmp.first->second.count == 0)
					{
						prevBoard.erase(tmp.first);
					}
					return;
				}
				tmp.first++;
			}
		}
		throw std::runtime_error("map not found");
	}

	Board::~Board()
	{
		ClearData();
	}

	Board::Board(const Board * toCopy)
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				fields[x][y] = toCopy->fields[x][y];

		prevBoard = toCopy->prevBoard;
		MoveStack = std::stack<StackElement>(toCopy->MoveStack);
		nextMoveIsWhite = toCopy->nextMoveIsWhite;
		leftWhite = toCopy->leftWhite;
		rightWhite = toCopy->rightWhite;
		leftBlack = toCopy->leftBlack;
		rightBlack = toCopy->rightBlack;
		this->nextMoveIsWhite = toCopy->nextMoveIsWhite;
	}


	int ChessBoard::Board::ChangeState(ChessBoard::InternalMove lastMove)
	{
		if (moveCounter == 0)
			return NoAction;
		StackElement tmp(lastMove, { Empty,false }, moveCounter, leftWhite, rightWhite, leftBlack, rightBlack, currentHash);
		Rank currentlyMoved = { Empty,false };
		std::pair<short, short> relativeMove;
		int fromX = (lastMove&fromXMask) >> fromXShift;
		int fromY = (lastMove&fromYMask) >> fromYShift;
		int toX = (lastMove&toXMask) >> toXShift;
		int toY = (lastMove&toYMask) >> toYShift;
		if ((lastMove&TypeMask) != RochadeLeft && (lastMove&TypeMask) != RochadeRight)
		{
			currentlyMoved = fields[fromX][fromY].rank;
			if (currentlyMoved.isWhite != nextMoveIsWhite)
			{
				std::cerr << "Wrong color!";
				return WrongColor;
			}
			relativeMove = { toX - fromX,toY - fromY };
			tmp.pieceType = fields[toX][toY].rank;
		}
		else
		{
			currentlyMoved = { Tower,nextMoveIsWhite };
		}


		//TODO:check move legitimacy
		switch (lastMove&TypeMask)
		{
		case(Standard):
			switch (currentlyMoved.type)
			{
			case(ChessBoard::Pawn):
			{
				if (relativeMove != std::pair<short, short>(-1, 1) && relativeMove != std::pair<short, short>(1, 1) && relativeMove != std::pair<short, short>(-1, -1) && relativeMove != std::pair<short, short>(1, -1) && relativeMove != std::pair<short, short>(0, 1) && relativeMove != std::pair<short, short>(0, -1) && relativeMove != std::pair<short, short>(0, 2) && relativeMove != std::pair<short, short>(0, -2))
					return NoAction;
				if (currentlyMoved.isWhite)
				{
					if (toY == 7)
						return NoAction;
					//No beating
					if (relativeMove.first == 0)
					{
						if (fields[toX][toY].rank.type != Empty)
							return NoAction;
						//Double move
						if (relativeMove.second == 2)
						{
							if (fromY != 1)
								return NoAction;
							if (fields[toX][fromY + 1].rank.type != ChessBoard::Empty || fields[toX][fromY + 2].rank.type != ChessBoard::Empty)
								return NoAction;
						}
					}
					//Beating
					else
					{
						if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite))
							return NoAction;
					}
				}
				else
				{
					if (toY == 0)
						return NoAction;
					//No beating
					if (relativeMove.first == 0)
					{
						if (fields[toX][toY].rank.type != Empty)
							return NoAction;
						//Double move
						if (relativeMove.second == -2)
						{
							if (fromY != 6)
								return NoAction;
							if (fields[fromX][fromY - 1].rank.type != ChessBoard::Empty || fields[fromX][fromY - 2].rank.type != ChessBoard::Empty)
								return NoAction;
						}
					}
					//Beating
					else
					{
						if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty && !fields[toX][toY].rank.isWhite))
							return NoAction;
					}
				}
			default:
				break;
			}
			break;
			case(ChessBoard::Tower):
			{
				if (relativeMove.first == 0)
				{
					for (int i = 1; abs(fromY + i*sign(relativeMove.second)) < abs(toY); i++)
						if (fields[fromX][fromY + i*sign(relativeMove.second)].rank.type != Empty)
							return NoAction;
				}
				else
				{
					if (relativeMove.second == 0)
					{
						for (int i = 1; abs(fromX + i*sign(relativeMove.first)) < abs(toX); i++)
							if (fields[fromX + i*sign(relativeMove.first)][fromY].rank.type != Empty)
								return NoAction;
					}
					else
						return NoAction;
				}
			}
			break;
			case(ChessBoard::Bishop):
			{
				if ((fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
				if (abs(relativeMove.first) != abs(relativeMove.second))
					return NoAction;
				for (int i = 1; (fromY + i*sign(relativeMove.second)) < (toY) && (fromY + i*sign(relativeMove.second)) > 0; i++)
					if (fields[fromX + i*sign(relativeMove.first)][fromY + i*sign(relativeMove.second)].rank.type != Empty)
						return NoAction;
			}
			break;
			case(ChessBoard::Queen):
			{
				if ((fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
				if (abs(relativeMove.first) != abs(relativeMove.second) && relativeMove.first != 0 && relativeMove.second != 0)
					return NoAction;
				for (int i = 1; fromX + sign(relativeMove.first)*i != toX&&fromY + sign(relativeMove.second)*i != toY; i++)
					if (fields[fromX + sign(relativeMove.first)*i][fromY + sign(relativeMove.second)*i].rank.type != Empty)
						return NoAction;
			}
			break;
			case(Knight):
			{
				int direction = 0;
				for (; direction < 8; ++direction)
				{
					if (KnightMovementArray[direction] == relativeMove)
						break;
				}
				if (KnightMovementArray[direction] != relativeMove)
					return NoAction;
				if (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite)
					return NoAction;
			}
			break;
			case(King):
			{
				if (!(relativeMove.first >= -1 && relativeMove.first <= 1))
					if (!(relativeMove.second >= -1 && relativeMove.second <= 1))
						return NoAction;
			}
			if (currentlyMoved.isWhite)
			{
				rightWhite = false;
				leftWhite = false;
			}
			else
			{
				rightBlack = false;
				leftBlack = false;
			}

			break;
			}
			fields[fromX][fromY].rank.type = Empty;
			fields[toX][toY].rank = currentlyMoved;
			break;
		case(PromotionQueen):
		{
			if (relativeMove.first == 0 && fields[toX][toY].rank.type != Empty)
				return NoAction;
			if (relativeMove.first != 0 && fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite)
				return NoAction;
			if ((currentlyMoved.isWhite && toY != 7) || (!currentlyMoved.isWhite && toY != 0) || currentlyMoved.type != Pawn)
				return NoAction;
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
		}
		fields[toX][toY].rank = { Queen,currentlyMoved.isWhite };
		fields[fromX][fromY].rank.type = Empty;
		break;
		case(PromotionBishop):
		{
			if (relativeMove.first == 0 && fields[toX][toY].rank.type != Empty)
				return NoAction;
			if (relativeMove.first != 0 && fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite)
				return NoAction;
			if ((currentlyMoved.isWhite && toY != 7) || (!currentlyMoved.isWhite && toY != 0) || currentlyMoved.type != Pawn)
				return NoAction;
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
		}
		fields[toX][toY].rank = { Bishop,currentlyMoved.isWhite };
		fields[fromX][fromY].rank.type = Empty;
		break;
		case(PromotionKnight):
		{
			if (relativeMove.first == 0 && fields[toX][toY].rank.type != Empty)
				return NoAction;
			if (relativeMove.first != 0 && fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite)
				return NoAction;
			if ((currentlyMoved.isWhite && toY != 7) || (!currentlyMoved.isWhite && toY != 0) || currentlyMoved.type != Pawn)
				return NoAction;
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
		}
		fields[toX][toY].rank = { Knight,currentlyMoved.isWhite };
		fields[fromX][fromY].rank.type = Empty;
		break;
		case(PromotionTower):
		{
			if (relativeMove.first == 0 && fields[toX][toY].rank.type != Empty)
				return NoAction;
			if (relativeMove.first != 0 && fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite)
				return NoAction;
			if ((currentlyMoved.isWhite && toY != 7) || (!currentlyMoved.isWhite && toY != 0) || currentlyMoved.type != Pawn)
				return NoAction;
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[toX][toY].rank.type == Empty || (fields[toX][toY].rank.type != Empty&&fields[toX][toY].rank.isWhite == currentlyMoved.isWhite))
					return NoAction;
		}
		fields[toX][toY].rank = { Tower,currentlyMoved.isWhite };
		fields[fromX][fromY].rank.type = Empty;
		break;
		case(RochadeLeft):
			if (nextMoveIsWhite)
			{
				if (rightWhite && fields[3][0].coveredByBlack == 0 && fields[3][0].rank.type == Empty && fields[2][0].coveredByBlack == 0 && fields[2][0].rank.type == Empty)
					if (fields[4][0].rank == Rank(King, true) && fields[0][0].rank == Rank(Tower, true))
					{
						fields[4][0].rank.type = Empty;
						fields[0][0].rank.type = Empty;
						fields[2][0].rank = { King, true };
						fields[3][0].rank = { Tower,true };
						rightWhite = false;
						leftWhite = false;
					}
					else
						return NoAction;
				else
					return NoAction;
			}
			else
			{
				if (rightBlack && fields[3][7].coveredByWhite == 0 && fields[3][7].rank.type == Empty && fields[2][7].coveredByWhite == 0 && fields[2][7].rank.type == Empty)
					if (fields[4][7].rank == Rank(King, false) && fields[0][7].rank == Rank(Tower, false))
					{
						fields[4][7].rank.type = Empty;
						fields[0][7].rank.type = Empty;
						fields[2][7].rank = { King, false };
						fields[3][7].rank = { Tower,false };
						rightBlack = false;
						leftBlack = false;
					}
					else
						return NoAction;
				else
					return NoAction;
			}

			break;
		case(RochadeRight):
			if (nextMoveIsWhite)
			{
				if (rightWhite && fields[5][0].coveredByBlack == 0 && fields[5][0].rank.type == Empty && fields[6][0].coveredByBlack == 0 && fields[6][0].rank.type == Empty)
					if (fields[4][0].rank == Rank(King, true) && fields[7][0].rank == Rank(Tower, true))
					{
						fields[4][0].rank.type = Empty;
						fields[7][0].rank.type = Empty;
						fields[6][0].rank = { King, true };
						fields[5][0].rank = { Tower,true };
						rightWhite = false;
						leftWhite = false;
					}
					else
						return NoAction;
				else
					return NoAction;
			}
			else
			{
				if (rightBlack && fields[5][7].coveredByWhite == 0 && fields[5][7].rank.type == Empty && fields[6][7].coveredByWhite == 0 && fields[6][7].rank.type == Empty)
					if (fields[4][7].rank == Rank(King, false) && fields[7][7].rank == Rank(Tower, false))

					{
						fields[4][7].rank.type = Empty;
						fields[7][7].rank.type = Empty;
						fields[6][7].rank = { King, false };
						fields[5][7].rank = { Tower,false };
						rightBlack = false;
						leftBlack = false;
					}
					else
						return NoAction;
				else
					return NoAction;
			}
			break;
		default:
			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Move was corrupted (invalid internal move type)"));
			break;
		}
		nextMoveIsWhite = !nextMoveIsWhite;
#ifdef DEBUG
		if (tmp.pieceType.type == King)
			throw std::runtime_error("King Beaten");
#endif // DEBUG

		switch (int n = fromX)
		{
		case 0:
			if (fromY == 0)
				leftWhite = false;
			else
				if (fromY == 7)
					leftBlack = false;
			break;
		case 7:
			if (fromY == 0)
				rightWhite = false;
			else
				if (fromY == 7)
					rightBlack = false;
		default:
			break;
		}
		switch (int n = toX)
		{
		case 0:
			if (toY == 0)
				leftWhite = false;
			else
				if (toY == 7)
					leftBlack = false;
			break;
		case 7:
			if (toY == 0)
				rightWhite = false;
			else
				if (toY == 7)
					rightBlack = false;
		default:
			break;
		}


		if (currentlyMoved.type == Pawn || tmp.pieceType.type != Empty)
		{
			moveCounter = 100;
		}
		else
			moveCounter--;
		MoveStack.push(tmp);
		PrevBoardElement::ReHash(currentHash, lastMove, currentlyMoved.isWhite);
		if (!addBoard())
		{
			return Revert;
		}
		KING_IN_DANGER err = PaintTheMap();
		if ((err.isWhite && currentlyMoved.isWhite) || (err.isBlack && !currentlyMoved.isWhite))
		{
			this->revert();
			return NoAction;
		}
		else
		{
			blackCheck = err.isBlack;
			whiteCheck = err.isWhite;
		}
		return Success;
	}

	int Board::ChangeState(InternalMove lastMove, int)
	{
		if ((lastMove&TypeMask) != RochadeLeft&&(lastMove&TypeMask) != RochadeRight)
			if (fields[(lastMove&fromXMask)>>fromXShift][(lastMove&fromYMask)>>fromYShift].rank.type == Pawn || fields[(lastMove&toXMask)>>toXShift][(lastMove&toYMask)>>toYShift].rank.type != Empty)
			{
				ClearStack();
				prevBoard.clear();
			}
		return ChangeState(lastMove);
	}

	bool ChessBoard::Board::operator==(const Board & rightOne) const
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				if (fields[x][y].rank != rightOne.fields[x][y].rank)
					return false;
		if (MoveStack.size() != rightOne.MoveStack.size() || moveCounter != rightOne.moveCounter || prevBoard.size() != rightOne.prevBoard.size())
			return false;
		return true;
	}

	bool Board::operator!=(const Board & right) const
	{
		return !(*this == right);
	}

	//TODO:paint the map
	ChessBoard::KING_IN_DANGER Board::PaintTheMap(InternalMove lastMove, Rank currentlyMoved)
	{
		bool isWhite = fields[lastMove&fromXMask>>fromXShift][lastMove&fromYMask>>fromYShift].rank.isWhite;
		if (fields[lastMove&fromXMask>>fromXShift][lastMove&fromYMask>>fromYShift].coveredByWhite || fields[lastMove&fromXMask>>fromXShift][lastMove&fromYMask>>fromYShift].coveredByBlack)
		{
			for (short i = 0; i < 8; i++)
			{
				if (IN_RANGE(((lastMove&fromXMask)>>fromXShift) + QueenMovementArray[i].first) && IN_RANGE(((lastMove&fromYMask)>>fromYShift) + QueenMovementArray[i].second))
				{
					switch (currentlyMoved.type)
					{
					case(Pawn):
					case(Knight):
					case(Tower):
					case(King):
					case(Bishop):
					default:
						break;
					}
					if (fields[lastMove&fromXMask + QueenMovementArray[i].first][lastMove&fromYMask + QueenMovementArray[i].second].coveredByWhite || fields[lastMove&fromXMask + QueenMovementArray[i].first][lastMove&fromYMask + QueenMovementArray[i].second].coveredByBlack)
					{
						for (short k = 0; IN_RANGE(((lastMove&fromXMask)>>fromXShift) + QueenMovementArray[i].first*k) && IN_RANGE(((lastMove&fromYMask)>>fromYShift) + QueenMovementArray[i].second*k); k++)
							if (!fields[((lastMove&fromXMask)>>fromXShift) + QueenMovementArray[i].first*k][((lastMove&fromYMask)>>fromYShift) + QueenMovementArray[i].second*k].coveredByWhite && !fields[((lastMove&fromXMask)>>fromXShift) + QueenMovementArray[i].first*k][((lastMove&fromYMask)>>fromYShift) + QueenMovementArray[i].second*k].coveredByBlack)
								break;
							else
							{

							}
					}
				}
			}
		}
		return KING_IN_DANGER(true, true);
	}

	//ChessBoard::InternalMove::InternalMove(Move toConvert)
	//{
	//	from = toConvert.from;
	//	to = toConvert.to;
	//	if (toConvert.special)
	//		switch (toConvert.type)
	//		{
	//		case('L'):
	//			from = std::pair<short, short>(-1, -1);
	//			movetype = RochadeLeft;
	//			break;
	//		case('R'):
	//			from = std::pair<short, short>(-1, -1);
	//			movetype = RochadeRight;
	//			break;
	//		case('B'):
	//			movetype = PromotionBishop;
	//			break;
	//		case('K'):
	//			movetype = PromotionKnight;
	//			break;
	//		case('Q'):
	//			movetype = PromotionQueen;
	//			break;
	//		case('T'):
	//			movetype = PromotionTower;
	//			break;
	//		default:
	//			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Recieved move was corrupted (external move type was invalid)"));
	//			break;
	//		}
	//	else
	//		movetype = Standard;

	//}

	//InternalMove::InternalMove()
	//{
	//}

	//Move ChessBoard::InternalMove::ConvertToExternal(bool amIWhite)
	//{
	//	Move returnValue;
	//	returnValue.whiteMove = amIWhite;
	//	//TODO: hnadle conversion to external
	//	if (returnValue.special = movetype != Standard)
	//		switch (movetype)
	//		{
	//		case(PromotionBishop):
	//			returnValue.type = 'B';
	//			break;
	//		case(PromotionKnight):
	//			returnValue.type = 'K';
	//			break;
	//		case(PromotionQueen):
	//			returnValue.type = 'Q';
	//			break;
	//		case(PromotionTower):
	//			returnValue.type = 'T';
	//			break;
	//		case(RochadeLeft):
	//			returnValue.type = 'L';
	//			return returnValue;
	//		case(RochadeRight):
	//			returnValue.type = 'R';
	//			return returnValue;
	//		default:
	//			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Generated move was corrupted (invalid internal move type)"));
	//			break;
	//		}
	//	returnValue.from = from;
	//	returnValue.to = to;
	//	return returnValue;
	//}

	Board::Moves::~Moves()
	{

	}

	Board::Moves::Moves()
	{

	}

	ChessBoard::Board::Moves & ChessBoard::Board::Moves::operator++()
	{
		state = cend();
		if (currentRank == std::pair<short, short>(-1, -1))
		{
			switch (moveIterator)
			{
			case(0):
				state = RochadeRight;
				moveIterator++;
				return*this;
			case(1):
				state = RochadeRight;
				moveIterator++;
				return*this;
			default:
				currentRank = std::pair<short, short>(0, 0);
				moveIterator = 0;
				direction = 0;
				break;
			}
		}
		if (currentRank.second == 8)
			return *this;
		if (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite)
		{
			//for (; currentRank.second < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite); currentRank.second++)
			//{
			//	for (; currentRank.first < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite); currentRank.first++)
			//	{ }
			//	if (currentRank.first == 8)
			//		currentRank.first = 0;
			//}

			while (currentRank.second < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite))
			{
				while (currentRank.first < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite))
				{
					currentRank.first++;
				}
				if (currentRank.first == 8)
				{
					currentRank.first = 0;
					currentRank.second++;
				}
			}
		}

		if ((currentRank.first == 8 || currentRank.second == 8))
		{
			state = cend();
			return *this;
		}


		switch (parent->fields[currentRank.first][currentRank.second].rank.type)
		{
		case(Pawn):
		{
			switch (direction)
			{
			case(0)://to the left
			{
				if (this->isWhite)
				{
					//check if this move is possible
					if (currentRank.first - 1 > 0 && IN_RANGE(currentRank.second + 1) && (!parent->fields[currentRank.first - 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), Standard);
							direction++;
							return *this;
						}
					//this move is impossible, increment direction
					else
					{
						moveIterator = 0;
						direction++;
						return ++*this;
					}
				}
				else
				{
					if (currentRank.first - 1 > 0 && IN_RANGE(currentRank.second - 1) && (parent->fields[currentRank.first - 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 0)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							state = MakeMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), Standard);
							direction++;
							return *this;
						}
					//this move is impossible, increment current rank and continue going through the board
					else
					{
						moveIterator = 0;
						direction++;
						return ++*this;
					}

				}
				break;
			}
			case(1)://to the middle
			{
				if (this->isWhite)
				{
					if (currentRank.second == 1)
					{
						switch (moveIterator)
						{
						case(0):
							if (IN_RANGE(currentRank.second + 1) && parent->fields[currentRank.first][currentRank.second + 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
						case(1):
							if (IN_RANGE(currentRank.second + 2) && parent->fields[currentRank.first][currentRank.second + 2].rank.type == Empty&&parent->fields[currentRank.first][currentRank.second + 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 2), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
						default:
							direction++;
							moveIterator = 0;
							return ++(*this);
						}
					}
					else
					{
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							if (IN_RANGE(currentRank.second + 1) && parent->fields[currentRank.first][currentRank.second + 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), Standard);
								direction++;
								return *this;
							}
						}
					}
				}
				else
				{
					if (currentRank.second == 6)
					{
						switch (moveIterator)
						{
						case(0):
							if (IN_RANGE(currentRank.second - 1) && parent->fields[currentRank.first][currentRank.second - 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
						case(1):
							if (IN_RANGE(currentRank.second - 2) && parent->fields[currentRank.first][currentRank.second - 2].rank.type == Empty&&parent->fields[currentRank.first][currentRank.second - 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 2), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
						default:
							direction++;
							moveIterator = 0;
							return ++(*this);
						}
					}
					else
					{
						if (currentRank.second == 1)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							if (IN_RANGE(currentRank.second - 1) && parent->fields[currentRank.first][currentRank.second - 1].rank.type == Empty)
							{
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), Standard);
								direction++;
								return *this;
							}
						}
					}
				}
			}
			case(2)://to the right
			{
				if (this->isWhite)
				{
					//check if this move is possible
					if (IN_RANGE(currentRank.first + 1) && IN_RANGE(currentRank.second + 1) && (!parent->fields[currentRank.first + 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), Standard);
							direction++;
							return *this;
						}
					//this move is impossible, increment current rank and continue going through the board
					else
					{
						moveIterator = 0;
						direction++;
						return ++*this;
					}
				}
				else
				{
					if (IN_RANGE(currentRank.first + 1) && IN_RANGE(currentRank.second - 1) && (parent->fields[currentRank.first + 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionTower);
								break;
							default:
								direction++;
								moveIterator = 0;
								return++(*this);
							}
							moveIterator++;
							return *this;
						}
						else
						{
							state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), Standard);
							direction++;
							return *this;
						}
					//this move is impossible, increment current rank and continue going through the board
					else
					{
						moveIterator = 0;
						direction++;
						return ++*this;
					}

				}
				break;
			}
			default:
			{
				direction = 0;
				moveIterator = 0;
				if (currentRank.first < 7)
				{
					currentRank.first++;
					return ++(*this);
				}
				else
				{
					currentRank.first = 0;
					currentRank.second++;
					return ++(*this);
				}
			}
			}
			break;
		}
		case(Queen):
		{
			switch (direction)
			{
			case(0):
				COMPLETE_MOVE_MACRO(moveIterator, moveIterator)
			case(1):
				COMPLETE_MOVE_MACRO(moveIterator, 0)
			case(2):
				COMPLETE_MOVE_MACRO(moveIterator, -moveIterator)
			case(3):
				COMPLETE_MOVE_MACRO(0, -moveIterator)
			case(4):
				COMPLETE_MOVE_MACRO(-moveIterator, -moveIterator)
			case(5):
				COMPLETE_MOVE_MACRO(-moveIterator, 0)
			case(6):
				COMPLETE_MOVE_MACRO(-moveIterator, moveIterator)
			case(7):
				COMPLETE_MOVE_MACRO(0, moveIterator);
			default:
			{
				direction = 0;
				moveIterator = 0;
				if (currentRank.first < 7)
				{
					currentRank.first++;
					return ++(*this);
				}
				else
				{
					currentRank.first = 0;
					currentRank.second++;
					return ++(*this);
				}
			}
			}
			moveIterator++;
			return *this;
		}
		case(Bishop):
		{
			switch (direction)
			{
			case(0):
				COMPLETE_MOVE_MACRO(moveIterator, moveIterator)
			case(1):
				COMPLETE_MOVE_MACRO(moveIterator, -moveIterator)
			case(2):
				COMPLETE_MOVE_MACRO(-moveIterator, -moveIterator)
			case(3):
				COMPLETE_MOVE_MACRO(-moveIterator, moveIterator);
			default:
			{
				direction = 0;
				moveIterator = 0;
				if (currentRank.first < 7)
				{
					currentRank.first++;
					return ++(*this);
				}
				else
				{
					currentRank.first = 0;
					currentRank.second++;
					return ++(*this);
				}
			}
			}
			moveIterator++;
			return *this;
		}
		case(Tower):
		{
			switch (direction)
			{
			case(0):
				COMPLETE_MOVE_MACRO(0, moveIterator);
				break;
			case(1):
				COMPLETE_MOVE_MACRO(moveIterator, 0);
				break;
			case(2):
				COMPLETE_MOVE_MACRO(0, -moveIterator);
				break;
			case(3):
				COMPLETE_MOVE_MACRO(-moveIterator, 0);
				break;
			default:
			{
				direction = 0;
				moveIterator = 0;
				if (currentRank.first < 7)
				{
					currentRank.first++;
					return ++(*this);
				}
				else
				{
					currentRank.first = 0;
					currentRank.second++;
					return ++(*this);
				}
			}

			}
			moveIterator++;
			return *this;
		}
		case(Knight):
		{
			if (direction < 8)
			{
				if (IN_RANGE(currentRank.first + KnightMovementArray[direction].first) && IN_RANGE(currentRank.second + KnightMovementArray[direction].second) && (parent->fields[currentRank.first + KnightMovementArray[direction].first][currentRank.second + KnightMovementArray[direction].second].rank.type == Empty || parent->fields[currentRank.first + KnightMovementArray[direction].first][currentRank.second + KnightMovementArray[direction].second].rank.isWhite != parent->fields[currentRank.first][currentRank.second].rank.isWhite))
				{
					state = MakeMove(currentRank, std::pair<short, short>(currentRank.first + KnightMovementArray[direction].first, currentRank.second + KnightMovementArray[direction].second), Standard);
				}
				else
				{
					direction++;
					return ++*this;
				}
			}
			else
			{
				direction = 0;
				moveIterator = 0;
				if (currentRank.first < 7)
				{
					currentRank.first++;
					return ++(*this);
				}
				else
				{
					currentRank.first = 0;
					currentRank.second++;
					return ++(*this);
				}
			}
			direction++;
			return *this;
		}
		case(King):
		{
			switch (direction)
			{
			case(0):
				COMPLETE_MOVE_MACRO(-1, 1)
			case(1):
				COMPLETE_MOVE_MACRO(0, 1)
			case(2):
				COMPLETE_MOVE_MACRO(1, 1)
			case(3):
				COMPLETE_MOVE_MACRO(-1, 0)
			case(4):
				COMPLETE_MOVE_MACRO(1, 0)
			case(5):
				COMPLETE_MOVE_MACRO(-1, -1)
			case(6):
				COMPLETE_MOVE_MACRO(0, -1)
			case(7):
				COMPLETE_MOVE_MACRO(-1, -1)
			default:
				{
					direction = 0;
					moveIterator = 0;
					if (currentRank.first < 7)
					{
						currentRank.first++;
						return ++(*this);
					}
					else
					{
						currentRank.first = 0;
						currentRank.second++;
						return ++(*this);
					}
				}
				break;
			}
		}
		default:
			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Board was corrupted (Rank type was invalid)"));
		}
		direction++;
		return *this;
	}

	const ChessBoard::InternalMove & ChessBoard::Board::Moves::operator*()
	{
		return state;
	}

	const InternalMove & Board::Moves::cend()
	{
		return endState;
	}

	void ChessBoard::Board::Moves::Reset(bool isWhite)
	{
		currentRank = { -1,-1 };
		this->isWhite = isWhite;
	}

	bool Rank::operator!=(Rank const & right) const
	{
		if (type != right.type)
			return true;
		if (type == Empty)
			return false;
		if (isWhite != right.isWhite)
			return true;
		return false;
	}

	bool Rank::operator==(const Rank & right) const
	{
		if (type == Empty&& right.type == Empty)
			return true;
		else
			return type == right.type&&isWhite == right.isWhite;
	}

	Board::PrevBoardElement::PrevBoardElement(const Field toCopy[8][8], short count)
	{
		for (short x = 0; x < 8; x++)
			for (short y = 0; y < 8; y++)
				map[x][y] = toCopy[x][y];
		this->count = count;
	}

	Board::PrevBoardElement::PrevBoardElement()
	{
		count = 0;
	}

	unsigned long long int Board::PrevBoardElement::CreateHash(const Field toHash[8][8])
	{
		hashType hash = 0;
		for (short x = 0; x < 8; ++x)
			for (short y = 0; y < 8; ++y)
				if (toHash[x][y].rank.type != Empty)
					hash = hash | shift(x, y);
		return hash;
	}

	void inline Board::PrevBoardElement::ReHash(hashType &old,const InternalMove& move, bool isWhite)
	{
		if((move&TypeMask)<RochadeLeft)
		{
			old ^= shift((move&fromXMask)>>fromXShift, (move&fromYMask)>>fromYShift);
			old |= shift((move&toXMask)>>toXShift, (move&toYMask)>>toYShift);
		}
		else
			switch (move&TypeMask)
			{
			case RochadeLeft:
				if (isWhite)
				{
					old ^= 0b10001ull;
					old |= 0b1100ull;
				}
				else
				{
					old ^= 0b1000100000000000000000000000000000000000000000000000000000000ull;
					old |= 0b110000000000000000000000000000000000000000000000000000000000ull;
				}
				break;
			case RochadeRight:
				if (isWhite)
				{
					old ^= 0b10010000;
					old |= 0b1100000;
				}
				else
				{
					old ^= 0b1001000000000000000000000000000000000000000000000000000000000000ull;
					old |= 0b110000000000000000000000000000000000000000000000000000000000000ull;
				}
				break;
			}
	}

	Board::PrevBoardElement Board::PrevBoardElement::operator++()
	{
		count++;
		return *this;
	}

	Board::PrevBoardElement Board::PrevBoardElement::operator--()
	{
		count--;
		return *this;
	}

	Board::PrevBoardElement::hashType Board::PrevBoardElement::shift(short x, short y)
	{
		hashType result = 1;
		return result << (x + 8 * y);
	}

	bool Board::PrevBoardElement::operator==(const Field right[8][8]) const
	{
		for (short x = 0; x < 8; x++)
			for (short y = 0; y < 8; y++)
				if (map[x][y].rank != (right)[x][y].rank)
					return false;
		return true;
	}

	bool Board::PrevBoardElement::operator==(const PrevBoardElement & right) const
	{
		for (short x = 0; x < 8; x++)
			for (short y = 0; y < 8; y++)
				if (map[x][y] != (right).map[x][y])
					return false;
		return true;
	}

	bool Field::operator==(const Field & right) const
	{
		return rank == right.rank&&coveredByWhite == right.coveredByWhite&&coveredByBlack == right.coveredByBlack;
	}

	bool Field::operator!=(const Field & right) const
	{
		return coveredByWhite != right.coveredByWhite || coveredByBlack != right.coveredByBlack || rank != right.rank;
	}
}
