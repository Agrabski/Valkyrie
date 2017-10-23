#include "Chessboard.h"
#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#pragma once

#pragma region MOVE_MACROS
#define MOVE_CHECK_MACRO(x,y) ((currentRank.first + x<8&&currentRank.first + x>0&&currentRank.second + y<8&&currentRank.second + y>0&&(parent->fields[currentRank.first + x][currentRank.second + y].rank.type == Empty || parent->fields[currentRank.first + x][currentRank.second + y].rank.isWhite != isWhite)))
#define MOVE_MACRO(x,y) 	state=InternalMove(currentRank, std::pair<short, short>(currentRank.first +x, currentRank.second +y), Standard);
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

	const InternalMove Board::Moves::endState = InternalMove(std::pair<short, short>(0, 8), std::pair<short, short>(0, 8), Standard);

	const std::pair<short, short> Board::KnightMovementArray[8] = { { -2,-1 },{ -2,1 },{ -1,2 },{ 1,2 },{ 2,1 },{ 2,-1 },{1,-2 },{ -1,-2 } };

	const std::pair<short, short> Board::QueenMovementArray[8] = { std::pair<short, short>(-1,0),std::pair<short, short>(-1,1),std::pair<short, short>(0,1),std::pair<short, short>(1,1),std::pair<short, short>(1,0),std::pair<short, short>(1,-1),std::pair<short, short>(0,-1),std::pair<short, short>(-1,-1) };

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
		PrevBoardElement::hashType tmphash = PrevBoardElement::CreateHash(fields);
		auto tmp = prevBoard.equal_range(tmphash);

		if (tmp.first == prevBoard.end())
		{
			prevBoard.insert(std::pair<PrevBoardElement::hashType, PrevBoardElement>(PrevBoardElement::CreateHash(fields), PrevBoardElement(fields, 1)));
			return true;
		}
		else
		{
			while (tmp.first != tmp.second)
			{
				if (tmp.first->second == fields)
				{
					++(tmp.first->second);
					return tmp.first->second.count < 3;
				}
				tmp.first++;
			}
		}
		prevBoard.insert(std::pair<PrevBoardElement::hashType, PrevBoardElement>(PrevBoardElement::CreateHash(fields), PrevBoardElement(fields, 1)));
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

	void ChessBoard::Board::PaintTheMap()
	{
		bool blackCheck = false;
		bool whiteCheck = false;
		for(int x=0;x<8;x++)
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
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>=0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>=0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
						if (x + KnightMovementArray[direction].first >= 0 && x + KnightMovementArray[direction].first < 8 && y + KnightMovementArray[direction].second>=0 && y + KnightMovementArray[direction].second < 8)
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
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>=0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>=0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>=0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>=0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
		if (blackCheck || whiteCheck)
			throw KING_IN_DANGER(blackCheck, whiteCheck);
	}

	void ChessBoard::Board::Revert()
	{

		nextMoveIsWhite = !nextMoveIsWhite;
		removeBoard(fields);
		StackElement tmp = MoveStack.top();
		moveCounter = tmp.movesCounter;
		InternalMove lastMove =tmp.move;
		Rank beaten = tmp.pieceType;
		rightBlack = tmp.rightBlack;
		leftBlack = tmp.leftBlack;
		rightWhite = tmp.rightWhite;
		leftWhite = tmp.leftWhite;
		MoveStack.pop();
		if (lastMove.movetype == Standard)
		{
			fields[lastMove.from.first][lastMove.from.second].rank = fields[lastMove.to.first][lastMove.to.second].rank;
			fields[lastMove.to.first][lastMove.to.second].rank = beaten;
			try
			{
				PaintTheMap();
			}
			catch (KING_IN_DANGER& err)
			{
				blackCheck = err.isBlack;
				whiteCheck = err.isWhite;
			}
			return;
		}
		switch (lastMove.movetype)
		{
		case(PromotionKnight):
		case(PromotionBishop):
		case(PromotionTower):
		case(PromotionQueen):
			fields[lastMove.from.first][lastMove.from.second].rank.type = Pawn;
			fields[lastMove.to.first][lastMove.to.second].rank = beaten;
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
				fields[2][7].rank = { Empty,false };
				fields[3][7].rank = { Empty,false };
			}
			break;
		default:
			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Storred move type was corrupted (invalid internal move type)"));
		}
		blackCheck = false;
		whiteCheck = false;
		try
		{
			PaintTheMap();
		}
		catch (KING_IN_DANGER& err)
		{
			blackCheck = err.isBlack;
			whiteCheck = err.isWhite;
		}
	}

	void Board::ClearData()
	{
		ClearStack();
		prevBoard.clear();
	}

	void Board::removeBoard(Field board[8][8])
	{
		PrevBoardElement::hashType t = PrevBoardElement::CreateHash(board);
		auto tmp = prevBoard.equal_range(t);

		if (tmp.first == prevBoard.end())
			throw std::runtime_error("map not found");
		else
		{
			while (tmp.first != tmp.second)
			{
				if (tmp.first->second == board)
				{
					--(tmp.first->second);
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

		prevBoard =toCopy->prevBoard;
		MoveStack = std::stack<StackElement>(toCopy->MoveStack);
		nextMoveIsWhite = toCopy->nextMoveIsWhite;
		leftWhite = toCopy->leftWhite;
		rightWhite = toCopy->rightWhite;
		leftBlack = toCopy->leftBlack;
		rightBlack = toCopy->rightBlack;
		this->nextMoveIsWhite = toCopy->nextMoveIsWhite;
	}

	void ChessBoard::Board::ChangeState(ChessBoard::InternalMove lastMove)
	{
		if (moveCounter == 0)
			throw FIFTY_MOVES();
		StackElement tmp(lastMove, { Empty,false }, moveCounter,leftWhite,rightWhite,leftBlack,rightBlack);
		Rank currentlyMoved = { Empty,false };
		std::pair<short, short> relativeMove;
		if (lastMove.from != std::pair<short, short>(-1, -1))
		{
			currentlyMoved = fields[lastMove.from.first][lastMove.from.second].rank;
			if (currentlyMoved.isWhite != nextMoveIsWhite)
				throw WRONG_COLOR();
			relativeMove = { lastMove.to.first - lastMove.from.first,lastMove.to.second - lastMove.from.second };
			tmp.pieceType= fields[lastMove.to.first][lastMove.to.second].rank;
		}
		else
		{
			currentlyMoved = { Tower,nextMoveIsWhite };
		}
		

		//TODO:check move legitimacy
		switch (lastMove.movetype)
		{
		case(Standard):
			switch (currentlyMoved.type)
			{
			case(ChessBoard::Pawn):
			{
				if (relativeMove != std::pair<short, short>(-1, 1) && relativeMove != std::pair<short, short>(1, 1) && relativeMove != std::pair<short, short>(-1, -1) && relativeMove != std::pair<short, short>(1, -1) && relativeMove != std::pair<short, short>(0, 1) && relativeMove != std::pair<short, short>(0, -1) && relativeMove != std::pair<short, short>(0, 2) && relativeMove != std::pair<short, short>(0, -2))
					throw INVALID_MOVE();
				if (currentlyMoved.isWhite)
				{
					if (lastMove.to.second == 7)
						throw INVALID_MOVE();
					//No beating
					if (relativeMove.first == 0)
					{
						if (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
							throw INVALID_MOVE();
						//Double move
						if (relativeMove.second == 2)
						{
							if (lastMove.from.second != 1)
								throw INVALID_MOVE();
							if (fields[lastMove.to.first][lastMove.from.second + 1].rank.type != ChessBoard::Empty|| fields[lastMove.to.first][lastMove.from.second + 2].rank.type != ChessBoard::Empty)
								throw MOVE_BLOCKED();
						}
					}
					//Beating
					else
					{
						if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite))
							throw INVALID_MOVE();
					}
				}
				else
				{
					if (lastMove.to.second == 0)
						throw INVALID_MOVE();
					//No beating
					if (relativeMove.first == 0)
					{
						if (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
							throw INVALID_MOVE();
						//Double move
						if (relativeMove.second == -2)
						{
							if (lastMove.from.second != 6)
								throw INVALID_MOVE();
							if (fields[lastMove.from.first][lastMove.from.second - 1].rank.type != ChessBoard::Empty|| fields[lastMove.from.first][lastMove.from.second - 2].rank.type != ChessBoard::Empty)
								throw MOVE_BLOCKED();
						}
					}
					//Beating
					else
					{
						if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&!fields[lastMove.to.first][lastMove.to.second].rank.isWhite))
							throw INVALID_MOVE();
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
					for (int i = 1; abs(lastMove.from.second + i*sign(relativeMove.second)) < abs(lastMove.to.second); i++)
						if (fields[lastMove.from.first][lastMove.from.second + i*sign(relativeMove.second)].rank.type != Empty)
							throw MOVE_BLOCKED();
				}
				else
				{
					if (relativeMove.second == 0)
					{
						for (int i = 1; abs(lastMove.from.first + i*sign(relativeMove.first)) < abs(lastMove.to.first); i++)
							if (fields[lastMove.from.first + i*sign(relativeMove.first)][lastMove.from.second].rank.type != Empty)
								throw INVALID_MOVE();
					}
					else
						throw INVALID_MOVE();
				}
			}
			break;
			case(ChessBoard::Bishop):
			{
				if ((fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw MOVE_BLOCKED();
				if (abs(relativeMove.first) != abs(relativeMove.second))
					throw INVALID_MOVE();
				for (int i = 1; (lastMove.from.second + i*sign(relativeMove.second)) < (lastMove.to.second) && (lastMove.from.second + i*sign(relativeMove.second)) > 0; i++)
					if (fields[lastMove.from.first + i*sign(relativeMove.first)][lastMove.from.second + i*sign(relativeMove.second)].rank.type != Empty)
						throw MOVE_BLOCKED();
			}
			break;
			case(ChessBoard::Queen):
			{
				if ((fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw MOVE_BLOCKED();
				if (abs(relativeMove.first) != abs(relativeMove.second) && relativeMove.first != 0 && relativeMove.second != 0)
					throw INVALID_MOVE();
				for (int i = 1; lastMove.from.first + sign(relativeMove.first)*i != lastMove.to.first&&lastMove.from.second + sign(relativeMove.second)*i != lastMove.to.second; i++)
					if (fields[lastMove.from.first + sign(relativeMove.first)*i][lastMove.from.second + sign(relativeMove.second)*i].rank.type != Empty)
						throw MOVE_BLOCKED();
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
					throw INVALID_MOVE();
				if (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite)
					throw MOVE_BLOCKED();
			}
			break;
			case(King):
			{
				if (!(relativeMove.first >= -1 && relativeMove.first <= 1))
					if (!(relativeMove.second >= -1 && relativeMove.second <= 1))
						throw INVALID_MOVE();
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
			fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
			fields[lastMove.to.first][lastMove.to.second].rank = currentlyMoved;
			break;
		case(PromotionQueen):
		{
			if (relativeMove.first == 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
				throw MOVE_BLOCKED();
			if(relativeMove.first!=0&& fields[lastMove.to.first][lastMove.to.second].rank.type!=Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite==currentlyMoved.isWhite)
				throw MOVE_BLOCKED();
			if ((currentlyMoved.isWhite && lastMove.to.second != 7) || (!currentlyMoved.isWhite && lastMove.to.second != 0) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Queen,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		break;
		case(PromotionBishop):
		{
			if (relativeMove.first == 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
				throw MOVE_BLOCKED();
			if (relativeMove.first != 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite)
				throw MOVE_BLOCKED();
			if ((currentlyMoved.isWhite && lastMove.to.second != 7) || (!currentlyMoved.isWhite && lastMove.to.second != 0) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Bishop,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		break;
		case(PromotionKnight):
		{
			if (relativeMove.first == 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
				throw MOVE_BLOCKED();
			if (relativeMove.first != 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite)
				throw MOVE_BLOCKED();
			if ((currentlyMoved.isWhite && lastMove.to.second != 7) || (!currentlyMoved.isWhite && lastMove.to.second != 0) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Knight,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		break;
		case(PromotionTower):
		{
			if (relativeMove.first == 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
				throw MOVE_BLOCKED();
			if (relativeMove.first != 0 && fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite)
				throw MOVE_BLOCKED();
			if ((currentlyMoved.isWhite && lastMove.to.second != 7) || (!currentlyMoved.isWhite && lastMove.to.second != 0) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Tower,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		break;
		case(RochadeLeft):
			if (nextMoveIsWhite)
			{
				if (rightWhite && fields[3][0].coveredByBlack == 0 && fields[3][0].rank.type == Empty && fields[2][0].coveredByBlack == 0 && fields[2][0].rank.type == Empty)
				{
					fields[4][0].rank.type = Empty;
					fields[0][0].rank.type = Empty;
					fields[2][0].rank = { King, true };
					fields[3][0].rank = { Tower,true };
					rightWhite = false;
					leftWhite = false;
				}
				else
					throw INVALID_MOVE();
			}
			else
			{
				if (rightBlack && fields[3][7].coveredByWhite == 0 && fields[3][7].rank.type == Empty && fields[2][7].coveredByWhite == 0 && fields[2][7].rank.type == Empty)
				{
					fields[4][7].rank.type = Empty;
					fields[0][7].rank.type = Empty;
					fields[2][7].rank = { King, false };
					fields[3][7].rank = { Tower,false };
					rightBlack = false;
					leftBlack = false;
				}
				else
					throw INVALID_MOVE();
			}

			break;
		case(RochadeRight):
			if (nextMoveIsWhite)
			{
				if (rightWhite && fields[5][0].coveredByBlack == 0 && fields[5][0].rank.type == Empty && fields[6][0].coveredByBlack == 0 && fields[6][0].rank.type == Empty)
				{
					fields[4][0].rank.type = Empty;
					fields[7][0].rank.type = Empty;
					fields[6][0].rank = { King, true };
					fields[5][0].rank = { Tower,true };
					rightWhite = false;
					leftWhite = false;
				}
				else
					throw INVALID_MOVE();
			}
			else
			{
				if (rightBlack && fields[5][7].coveredByWhite == 0 && fields[5][7].rank.type == Empty && fields[6][7].coveredByWhite == 0 && fields[6][7].rank.type == Empty)
				{
					fields[4][7].rank.type = Empty;
					fields[7][7].rank.type = Empty;
					fields[6][7].rank = { King, false };
					fields[5][7].rank = { Tower,false };
					rightBlack = false;
					leftBlack = false;
				}
				else
					throw INVALID_MOVE();
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

		switch (int n=lastMove.from.first)
		{
		case 0:
			if (n == 0)
				leftWhite = false;
			else if (lastMove.from.second == 7)
				leftBlack = false;
			break;
		case 7:
			if (n == 0)
				rightWhite = false;
			else if (n == 7)
				rightBlack = false;
		default:
			break;
		}
		switch (int n = lastMove.to.first)
		{
		case 0:
			if (n == 0)
				leftWhite = false;
			else if (lastMove.from.second == 7)
				leftBlack = false;
			break;
		case 7:
			if (n == 0)
				rightWhite = false;
			else if (n == 7)
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
		if (!addBoard())
		{
			throw THREEFOLD_REPETITON();
		}
		try
		{
			PaintTheMap();
		}
		catch (KING_IN_DANGER err)
		{
			if ((err.isWhite && currentlyMoved.isWhite) || (err.isBlack && !currentlyMoved.isWhite))
			{
				this->Revert();
				throw err;
			}
			else
			{
				blackCheck = err.isBlack;
				whiteCheck = err.isWhite;
			}
		}
	}

	void Board::ChangeState(InternalMove lastMove, int)
	{
		if(lastMove.from!=std::pair<short,short>(-1,-1))
			if (fields[lastMove.from.first][lastMove.from.second].rank.type == Pawn || fields[lastMove.to.first][lastMove.to.second].rank.type != Empty)
			{
				ClearStack();
				prevBoard.clear();
			}
		ChangeState(lastMove);
	}

	bool ChessBoard::Board::operator==(const Board & rightOne) const
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				if (fields[x][y].rank != rightOne.fields[x][y].rank)
					return false;
		if (MoveStack.size() != rightOne.MoveStack.size() || moveCounter != rightOne.moveCounter)
			return false;
		return true;
	}

	bool Board::operator!=(const Board & right) const
	{
		return !(*this==right);
	}

	//TODO:paint the map
	void Board::PaintTheMap(InternalMove lastMove, Rank currentlyMoved)
	{
		bool isWhite = fields[lastMove.from.first][lastMove.from.second].rank.isWhite;
		if(fields[lastMove.from.first][lastMove.from.second].coveredByWhite|| fields[lastMove.from.first][lastMove.from.second].coveredByBlack)
		{
			for (short i = 0; i < 8; i++)
			{
				if(IN_RANGE(lastMove.from.first + QueenMovementArray[i].first)&&IN_RANGE(lastMove.from.second + QueenMovementArray[i].second))
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
					if (fields[lastMove.from.first + QueenMovementArray[i].first][lastMove.from.second + QueenMovementArray[i].second].coveredByWhite|| fields[lastMove.from.first + QueenMovementArray[i].first][lastMove.from.second + QueenMovementArray[i].second].coveredByBlack)
					{
						for (short k = 0; IN_RANGE(lastMove.from.first + QueenMovementArray[i].first*k) && IN_RANGE(lastMove.from.second + QueenMovementArray[i].second*k); k++)
							if (!fields[lastMove.from.first + QueenMovementArray[i].first*k][lastMove.from.second + QueenMovementArray[i].second*k].coveredByWhite && !fields[lastMove.from.first + QueenMovementArray[i].first*k][lastMove.from.second + QueenMovementArray[i].second*k].coveredByBlack)
								break;
							else
							{

							}
					}
				}
			}
		}
	}

	ChessBoard::InternalMove::InternalMove(Move toConvert)
	{
		from = toConvert.from;
		to = toConvert.to;
		if (toConvert.special)
			switch (toConvert.type)
			{
			case('L'):
				from = std::pair<short, short>(-1, -1);
				movetype = RochadeLeft;
				break;
			case('R'):
				from = std::pair<short, short>(-1, -1);
				movetype = RochadeRight;
				break;
			case('B'):
				movetype = PromotionBishop;
				break;
			case('K'):
				movetype = PromotionKnight;
				break;
			case('Q'):
				movetype = PromotionQueen;
				break;
			case('T'):
				movetype = PromotionTower;
				break;
			default:
				throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Recieved move was corrupted (external move type was invalid)"));
				break;
			}
		else
			movetype = Standard;

	}

	InternalMove::InternalMove()
	{
	}

	Move ChessBoard::InternalMove::ConvertToExternal(bool amIWhite)
	{
		Move returnValue;
		returnValue.whiteMove = amIWhite;
		//TODO: hnadle conversion to external
		if (returnValue.special = movetype != Standard)
			switch (movetype)
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
		returnValue.from = from;
		returnValue.to = to;
		return returnValue;
	}

	bool InternalMove::operator!=(const InternalMove & other) const
	{
		return from != other.from || to != other.to || movetype != other.movetype;
	}


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
				state = InternalMove({ -1,-1 }, { -1,-1 }, RochadeLeft);
				moveIterator++;
				return*this;
			case(1):
				state = InternalMove({ -1,-1 }, { -1,-1 }, RochadeLeft);
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
					if (currentRank.first - 1 > 0&&IN_RANGE(currentRank.second + 1) && (!parent->fields[currentRank.first - 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionTower);
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
							state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), Standard);
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
					if (currentRank.first - 1 > 0 &&IN_RANGE(currentRank.second - 1)&& (parent->fields[currentRank.first - 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 0)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionTower);
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
							state = InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), Standard);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), Standard);
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
									state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 2), Standard);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), PromotionTower);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), Standard);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), Standard);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 2), Standard);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), PromotionTower);
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
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), Standard);
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
					if (IN_RANGE( currentRank.first + 1)&& IN_RANGE(currentRank.second + 1) && (!parent->fields[currentRank.first + 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionTower);
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
							state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), Standard);
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
					if (IN_RANGE(currentRank.first + 1)&&IN_RANGE(currentRank.second - 1) && (parent->fields[currentRank.first + 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 6)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionTower);
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
							state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), Standard);
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
					state = InternalMove(currentRank, std::pair<short, short>(currentRank.first + KnightMovementArray[direction].first, currentRank.second + KnightMovementArray[direction].second), Standard);
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
		hashType hash=0;
		for (short x = 0; x < 8; ++x)
			for (short y = 0; y < 8; ++y)
				if (toHash[x][y].rank.type != Empty)
					hash =hash | shift(x ,y);
		return hash;
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