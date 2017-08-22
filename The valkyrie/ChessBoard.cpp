#include "Chessboard.h"
#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#pragma once

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "Valkirye"
#endif // !PROGRAM_NAME

namespace ChessBoard
{
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
		for(int i=0;i<prevBoard->size();i++)
		{
			bool flag = true;
			for(int x=0;x<8&&flag;x++)
				for(int y=0;y<8;y++)
					if ((*prevBoard)[i].first[x][y] != fields[x][y])
					{
						flag = false;
						break;
					}
			if (flag)
			{
				(*prevBoard)[i].second++;
				return (*prevBoard)[i].second < 3;
			}
		}
		prevBoard->emplace_back(std::pair<std::vector<std::vector<Field>>, int>(fields, 1));
		return true;
	}

	Board & Board::operator=(const Board & toCopy)
	{
		// TODO: insert return statement here
		return *this;
	}

	void Board::ClearStack()
	{
		while (!MoveStack->empty())
		{
			MoveStack->pop();
		}
	}

	void ChessBoard::Board::PaintTheMap()
	{
		bool blackCheck = false;
		bool whiteCheck = false;
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

						if (x - 1 > 0 && y + 1 < 8)
						{
							fields[x - 1][y + 1].coveredByWhite += 1;
							if (fields[x - 1][y + 1].rank.type == King && !fields[x - 1][y + 1].rank.isWhite)
								blackCheck = true;
						}

					}
					else
					{
						if (x + 1 < 8 && y - 1 > 0)
						{
							fields[x + 1][y - 1].coveredByBlack += 1;
							if (fields[x + 1][y - 1].rank.type == King && fields[x + 1][y - 1].rank.isWhite)
								whiteCheck = true;
						}
						if (x - 1 > 0 && y - 1 > 0)
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
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
				case(Knight):
				{
					for (int direction = 0; direction < 8; ++direction)
					{
						if (x + KnightMovementArray[direction].first > 0 && x + KnightMovementArray[direction].first < 8 && y + KnightMovementArray[direction].second>0 && y + KnightMovementArray[direction].second < 8)
						{
							if (isWhite)
								fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].coveredByWhite += 1;
							else
								fields[x + KnightMovementArray[direction].first][y + KnightMovementArray[direction].second].coveredByBlack += 1;
						}
					}
					break;
				}
				break;
				case(Tower):
				{
					for (int direction = 0; direction < 8; direction += 2)
					{
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
					for (int direction = 0; direction < 8; direction += 2)
					{
						for (int i = 1; i < 8 && x + QueenMovementArray[direction].first*i>0 && x + QueenMovementArray[direction].first*i < 8 && y + QueenMovementArray[direction].second*i>0 && y + QueenMovementArray[direction].second*i < 8; ++i)
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
		InternalMove lastMove = MoveStack->top().first;
		Rank beaten = MoveStack->top().second;
		MoveStack->pop();
		if (lastMove.movetype == Standard)
		{
			fields[lastMove.from.first][lastMove.from.second].rank = fields[lastMove.to.first][lastMove.to.second].rank;
			fields[lastMove.to.first][lastMove.to.second].rank = beaten;
			PaintTheMap();
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
			return;
		case(RochadeLeft):
			if (!nextMoveIsWhite)
			{
				fields[0][0].rank = { Tower, true };
				fields[4][0].rank = { King,true };
				fields[2][0].rank = { Empty,true };
				fields[3][0].rank = { Empty,true };
				leftWhite = true;
				rightWhite = true;
			}
			else
			{
				leftBlack = true;
				rightBlack = true;
			}
			break;
		case(RochadeRight):
			if (!nextMoveIsWhite)
			{
				fields[7][0].rank = { Tower, true };
				fields[4][0].rank = { King,true };
				fields[6][0].rank = { Empty,true };
				fields[5][0].rank = { Empty,true };
				leftWhite = true;
				rightWhite = true;
			}
			else
			{
				leftBlack = true;
				rightBlack = true;
			}
			break;
		default:
			throw std::runtime_error(PROGRAM_NAME + std::string(" ERROR:Storred move type was corrupted (invalid internal move type)"));
		}
	}

	void Board::ClearData()
	{
		delete MoveStack;
		delete prevBoard;
	}

	void ChessBoard::Board::removeBoard(std::vector<std::vector<Field>> board)
	{
		for (std::vector<std::pair<std::vector<std::vector<Field>>, int>>::iterator i = prevBoard->begin(); i != prevBoard->end(); ++i)
		{
			bool flag = true;
			for (int x = 0; x < 8&&flag; x++)
				for (int y = 0; y < 8; y++)
					if (i->first[x][y] != fields[x][y])
					{
						flag = false;
						break;
					}
			if (flag)
			{
				if (i->second == 1)
					prevBoard->erase(i);
				else
					i->second--;
				return;
			}
		}
	}

	Board::~Board()
	{
		ClearData();
	}

	Board::Board(const Board * toCopy)
	{
		fields.resize(8);
		for (int i = 0; i < 8; i++)
			fields[i].resize(8);

		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				fields[x][y] = toCopy->fields[x][y];

		prevBoard = new std::vector<std::pair<std::vector<std::vector<Field>>, int>>(*toCopy->prevBoard);
		MoveStack = new std::stack<std::pair<InternalMove, Rank>>(*toCopy->MoveStack);
		nextMoveIsWhite = toCopy->nextMoveIsWhite;
		leftWhite = toCopy->leftWhite;
		rightWhite = toCopy->rightWhite;
		leftBlack = toCopy->leftBlack;
		rightBlack = toCopy->rightBlack;
		this->nextMoveIsWhite = toCopy->nextMoveIsWhite;
	}

	void ChessBoard::Board::ChangeState(ChessBoard::InternalMove lastMove)
	{
		Rank currentlyMoved = { Empty,false };
		std::pair<short, short> relativeMove;
		if (lastMove.from != std::pair<short, short>(-1, -1))
		{
			currentlyMoved = fields[lastMove.from.first][lastMove.from.second].rank;
			if (currentlyMoved.isWhite != nextMoveIsWhite)
				throw WRONG_COLOR();
			relativeMove = { lastMove.to.first - lastMove.from.first,lastMove.to.second - lastMove.from.second };
		}
		std::pair<InternalMove, Rank>tmp = std::pair<InternalMove, Rank>();

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
							if (fields[lastMove.to.first][lastMove.from.second + 1].rank.type != ChessBoard::Empty)
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
							if (fields[lastMove.from.first][lastMove.from.second - 1].rank.type != ChessBoard::Empty)
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
				if (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite != nextMoveIsWhite)
					throw MOVE_BLOCKED();
			}
			break;
			case(King):
			{
				if (!(relativeMove.first >= -1 && relativeMove.first <= 1))
					if (!(relativeMove.second >= -1 && relativeMove.second <= 1))
						throw INVALID_MOVE();
			}
			break;
			}
			fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
			fields[lastMove.to.first][lastMove.to.second].rank = currentlyMoved;
			break;
		case(PromotionQueen):
		{
			if ((currentlyMoved.isWhite && lastMove.to.second != 6) || (!currentlyMoved.isWhite && lastMove.to.second != 1) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Queen,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		tmp.second = fields[lastMove.to.first][lastMove.to.second].rank;
		break;
		case(PromotionBishop):
		{
			if ((currentlyMoved.isWhite && lastMove.to.second != 6) || (!currentlyMoved.isWhite && lastMove.to.second != 1) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Bishop,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		tmp.second = fields[lastMove.to.first][lastMove.to.second].rank;
		break;
		case(PromotionKnight):
		{
			if ((currentlyMoved.isWhite && lastMove.to.second != 6) || (!currentlyMoved.isWhite && lastMove.to.second != 1) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Knight,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		tmp.second = fields[lastMove.to.first][lastMove.to.second].rank;
		break;
		case(PromotionTower):
		{
			if ((currentlyMoved.isWhite && lastMove.to.second != 6) || (!currentlyMoved.isWhite && lastMove.to.second != 1) || currentlyMoved.type != Pawn)
				throw INVALID_MOVE();
			if (relativeMove.first == 1 || relativeMove.first == -1)
				if (fields[lastMove.to.first][lastMove.to.second].rank.type == Empty || (fields[lastMove.to.first][lastMove.to.second].rank.type != Empty&&fields[lastMove.to.first][lastMove.to.second].rank.isWhite == currentlyMoved.isWhite))
					throw INVALID_MOVE();
		}
		fields[lastMove.to.first][lastMove.to.second].rank = { Tower,currentlyMoved.isWhite };
		fields[lastMove.from.first][lastMove.from.second].rank.type = Empty;
		tmp.second = fields[lastMove.to.first][lastMove.to.second].rank;
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
		tmp.first = lastMove;
		MoveStack->push(tmp);
		nextMoveIsWhite = !nextMoveIsWhite;
		if (!addBoard())
		{
			Revert();
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
		}
	}

	bool ChessBoard::Board::operator==(Board & rightOne)
	{
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				if (fields[x][y].rank != rightOne.fields[x][y].rank)
					return false;
		return true;
	}

	//TODO:paint the map
	void Board::PaintTheMap(InternalMove lastMove)
	{
	}

	ChessBoard::InternalMove::InternalMove(Move toConvert)
	{
		from = toConvert.from;
		to = toConvert.to;
		if (toConvert.special)
			switch (toConvert.type)
			{
			case('L'):
				movetype = RochadeLeft;
				break;
			case('R'):
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


#pragma region MOVE_MACROS
#define MOVE_CHECK_MACRO(x,y) ((currentRank.first + x<8&&currentRank.first + x>0&&currentRank.second + y<8&&currentRank.second + y>0&&(parent->fields[currentRank.first + x][currentRank.second + y].rank.type == Empty || parent->fields[currentRank.first + x][currentRank.second + y].rank.isWhite != isWhite)))
#define MOVE_MACRO(x,y) 	state=new InternalMove(currentRank, std::pair<short, short>(currentRank.first +x, currentRank.second +y), Standard);
#define COMPLETE_MOVE_MACRO(x,y) if(MOVE_CHECK_MACRO(x,y)) {MOVE_MACRO(x,y)} else { direction++; moveIterator = 0; return ++(*this);}
#pragma endregion

#pragma region SAFETY_MACROS
#define IN_RANGE(x) (0<=x&&x<8)
#pragma endregion

	Board::Moves::Moves()
	{
		delete state;
		state = nullptr;
	}

	ChessBoard::Board::Moves & ChessBoard::Board::Moves::operator++()
	{
		delete state;
		state = nullptr;
		if (currentRank == std::pair<short, short>(-1, -1))
		{
			switch (moveIterator)
			{
			case(0):
				state = new InternalMove({ -1,-1 }, { -1,-1 }, RochadeLeft);
				moveIterator++;
				return*this;
			case(1):
				state = new InternalMove({ -1,-1 }, { -1,-1 }, RochadeLeft);
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
			for (; currentRank.second < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite); currentRank.second++)
			{
				for (; currentRank.first < 8 && (parent->fields[currentRank.first][currentRank.second].rank.type == Empty || parent->fields[currentRank.first][currentRank.second].rank.isWhite != this->isWhite); currentRank.first++)
				{ }
				if (currentRank.first == 8)
					currentRank.first = 0;
			}
		}

		if ((currentRank.first == 8 || currentRank.second == 8))
		{
			state = nullptr;
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
					if (currentRank.first - 1 > 0 && (!parent->fields[currentRank.first - 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 7)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), PromotionTower);
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
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second + 1), Standard);
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
					if (currentRank.first - 1 > 0 && (!parent->fields[currentRank.first - 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first - 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 7)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), PromotionTower);
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
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first - 1, currentRank.second - 1), Standard);
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
					switch (moveIterator)
					{
					case(0):
						if (parent->fields[currentRank.first][currentRank.second + 1].rank.type == Empty)
						{
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 1), Standard);
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
						if (currentRank.second == 1)
						{
							if (parent->fields[currentRank.first][currentRank.second + 2].rank.type == Empty)
							{
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second + 2), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
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
					switch (moveIterator)
					{
					case(0):
						if (parent->fields[currentRank.first][currentRank.second - 1].rank.type == Empty)
						{
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 1), Standard);
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
						if (currentRank.second == 6)
						{
							if (parent->fields[currentRank.first][currentRank.second - 2].rank.type == Empty)
							{
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first, currentRank.second - 2), Standard);
								moveIterator++;
								return *this;
							}
							else
							{
								direction++;
								moveIterator = 0;
								return ++(*this);
							}
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
			}
			case(2)://to the right
			{
				if (this->isWhite)
				{
					//check if this move is possible
					if (IN_RANGE( currentRank.first + 1) && (!parent->fields[currentRank.first + 1][currentRank.second + 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second + 1].rank.type == Empty))
						if (currentRank.second == 7)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionBishop);
								break;
							case(1):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionKnight);
								break;
							case(2):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionQueen);
								break;
							case(3):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), PromotionTower);
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
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second + 1), Standard);
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
					if (IN_RANGE(currentRank.first + 1) && (!parent->fields[currentRank.first + 1][currentRank.second - 1].rank.isWhite || parent->fields[currentRank.first + 1][currentRank.second - 1].rank.type == Empty))
						if (currentRank.second == 7)
						{
							//this pawn will be promoted, check the promotions
							switch (moveIterator)
							{
							case(0):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionBishop);
								break;
							case(1):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionKnight);
								break;
							case(2):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionQueen);
								break;
							case(3):
								state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), PromotionTower);
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
							state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + 1, currentRank.second - 1), Standard);
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
					state = new InternalMove(currentRank, std::pair<short, short>(currentRank.first + KnightMovementArray[direction].first, currentRank.second + KnightMovementArray[direction].second), Standard);
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

	const ChessBoard::InternalMove * ChessBoard::Board::Moves::operator*()
	{
		return state;
	}

	void ChessBoard::Board::Moves::Reset(bool isWhite)
	{
		currentRank = { -1,-1 };
		this->isWhite = isWhite;
	}

	bool const Rank::operator!=(Rank const & right)
	{
		if (type != right.type)
			return true;
		if (type == Empty)
			return false;
		if (isWhite != right.isWhite)
			return true;
		return false;
	}

	bool Field::operator!=(const Field & right)
	{
		return rank!=right.rank;
	}

	bool Field::operator==(const Field & right)
	{
		return false;
	}

	bool Field::operator>(const Field & right)
	{
		return false;
	}

	bool Field::operator<(const Field & right)
	{
		return false;
	}

}