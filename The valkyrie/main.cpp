#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Evaluator.h"
#include <windows.system.h>
#include <fstream>
#include <time.h>

std::ostream & operator<<(std::ostream & stream, ChessEvaluator::ChessEvaluator const toWrite)
{
	return stream << toWrite.friendlyValue << " " << toWrite.enemyValue << " " << toWrite.friendlyCheck << " " << toWrite.enemyCheck << std::endl;
}

void tester1()
{
	bool displayMoves, displayTime;
	std::ofstream stream;
	stream.open("debug.txt", 'w');
	JudgeDredd::Valkyrie *player1, *player2;
	Move move, buffer;
	ChessEvaluator::ChessEvaluator currBest = ChessEvaluator::ChessEvaluator();
	float a, b, c, d;
	bool firstIsWhite = true;
	move.from = { 0,0 };
	move.to = { 0,0 };
	std::cout << "Display moves, display time?" << std::endl;
	std::cin >> displayMoves >> displayTime;
	for (d = 300; d >= 0; d -= 10)
	{
		for (c = 300; c >= 0; c -= 10)
		{
			for (b = 10; b >= -10; b -= .1f)
			{
				for (a = -10; a <= 10; a += .1f)
				{
					int firstWin = 0, secondWin = 0;
					for (int i = 2; i < 6; i++)
					{
						std::cout << "Game number:" << i << " Configuration:" << a << "," << b << "," << c << "," << d << std::endl << "Current Best:" << currBest << std::endl;
						stream << currBest << "----" << ChessEvaluator::ChessEvaluator(a, b, c, d) << "--\n";
						if (displayTime)
						{
							time_t t = time(0);   // get time now
							struct tm  now;
							localtime_s(&now, &t);
							std::cout << (now.tm_hour) << ':'
								<< (now.tm_min + 1) << ':'
								<< now.tm_sec
								<< std::endl;
						}
						player1 = new JudgeDredd::Valkyrie(firstIsWhite, currBest, i);
						player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d), i);
						try
						{
							for (buffer = player1->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
							{
								buffer = player2->makeMove(buffer);
								if (displayMoves)
									std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								buffer = player1->makeMove(buffer);
								if (displayMoves)
									std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
							}
						}
						catch (GAME_ENDED err)
						{
							std::cout << "Game Ended ";
							if (err.blackWon)
							{
								secondWin += 1;
								std::cout << "new won\n";
							}
							if (err.whiteWon)
							{
								firstWin += 1;
								std::cout << "old won\n";
							}
							if (err.tie)
								std::cout << "no one won\n";
							std::cout << firstWin << "-" << secondWin << std::endl;
						}
						std::cout << "Switching sides\n";
						delete player1;
						delete player2;
						if (displayTime)
						{
							time_t t = time(0);   // get time now
							struct tm  now;
							localtime_s(&now, &t);
							std::cout << (now.tm_hour) << ':'
								<< (now.tm_min + 1) << ':'
								<< now.tm_sec
								<< std::endl;
						}
						player1 = new JudgeDredd::Valkyrie(!firstIsWhite, currBest, i);
						player2 = new JudgeDredd::Valkyrie(firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d), i);

						try
						{
							for (buffer = player2->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
							{
								buffer = player1->makeMove(buffer);
								if (displayMoves)
									std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								buffer = player2->makeMove(buffer);
								if (displayMoves)
									std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
							}
						}
						catch (GAME_ENDED err)
						{
							std::cout << "Game Ended ";
							if (err.whiteWon)
							{
								secondWin += 1;
								std::cout << "new won\n";
							}
							if (err.blackWon)
							{
								firstWin += 1;
								std::cout << "old won\n";
							}
							if (err.tie)
								std::cout << "no one won\n";
							std::cout << firstWin << "-" << secondWin << std::endl;
						}
						delete player1;
						delete player2;
						std::cout << "Switching sides\n";
					}
					if (secondWin > firstWin)
						currBest = ChessEvaluator::ChessEvaluator(a, b, c, d);
				}
			}
		}
	}
	std::cout << std::endl << "RESULT:" << std::endl << a << "," << b << "," << c << "," << d << std::endl;
	system("pause");
	std::cin >> a;
}

void tester2()
{
	Move move, buffer;
	std::ofstream stream;
	JudgeDredd::Valkyrie *player1, *player2;
	stream.open("debug.txt", 'w');
	for (int i = 2; i < 10; i++)
	{
		std::cout << i << std::endl;
		time_t t1 = time(0);   // get time now
		player1 = new JudgeDredd::Valkyrie(true, ChessEvaluator::ChessEvaluator(), i);
		player2 = new JudgeDredd::Valkyrie(false, ChessEvaluator::ChessEvaluator(), i);
		try
		{
			for (buffer = player1->makeMove(move); true;)
			{
				buffer = player2->makeMove(buffer);
				buffer = player1->makeMove(buffer);
			}
		}
		catch (GAME_ENDED err)
		{
		}
		stream << time(0) - t1 << std::endl;
		delete player1;
		delete player2;
	}

}

int main()
{
	tester2();
	return 0;
}