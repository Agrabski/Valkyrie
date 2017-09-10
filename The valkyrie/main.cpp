#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Evaluator.h"
#include <windows.system.h>
#include <fstream>

std::ostream & operator<<(std::ostream & stream, ChessEvaluator::ChessEvaluator const toWrite)
{
	return stream << toWrite.friendlyValue << " " << toWrite.enemyValue << " " << toWrite.friendlyCheck << " " << toWrite.enemyCheck << std::endl;
}

int main()
{
	std::ofstream stream;
	stream.open("debug.txt", 'w');
	JudgeDredd::Valkyrie *player1, *player2;
	Move move, buffer;
	ChessEvaluator::ChessEvaluator currBest = ChessEvaluator::ChessEvaluator();
	float a, b, c, d;
	bool firstIsWhite = true;
	move.from = { 0,0 };
	move.to = { 0,0 };
	for (d = 300; d > 0; d -= 10)
	{
		for (c = 300; c > 0; c -= 10)
		{
			for (b = 10; b > 0; b -= .1f)
			{
				for (a = 0; a < 10; a += .1f)
				{
					int firstWin = 0, secondWin = 0;
					for (int i = 0; i < 2; i++)
					{
						std::cout << "Game number:" << i << " Configuration:" << a << "," << b << "," << c << "," << d << std::endl;
						stream << currBest << "----" << ChessEvaluator::ChessEvaluator(a, b, c, d) << "--\n";
						player1 = new JudgeDredd::Valkyrie(firstIsWhite, currBest);
						player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d));
						try
						{
							for (buffer = player1->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
							{
								buffer = player2->makeMove(buffer);
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								buffer = player1->makeMove(buffer);
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
						player1 = new JudgeDredd::Valkyrie(!firstIsWhite, currBest);
						player2 = new JudgeDredd::Valkyrie(firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d));

						try
						{
							for (buffer = player2->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
							{
								buffer = player1->makeMove(buffer);
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								buffer = player2->makeMove(buffer);
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
	return 0;
}