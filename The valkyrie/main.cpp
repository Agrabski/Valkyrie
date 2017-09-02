#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main()
{
	JudgeDredd::Valkyrie *player1, *player2;
	Move move, buffer;
	ChessEvaluator::ChessEvaluator currBest;
	float a, b, c, d;
	bool firstIsWhite=true;
	move.from = { 0,0 };
	move.to = { 0,0 };
	for(a=.1f;a<10;a+=.1f)
		for(b=.1f;b<10;b+=.1f)
			for(c=.1f;c<10;c+=.1f)
				for (d = .2f; d < 10; d += .1f)
				{
					int firstWin = 0, secondWin = 0;
					for (int i = 0; i < 4; i++)
					{
						std::cout << "Game number:" << i << " Configuration:" << a << "," << b << "," << c << "," << d << std::endl;
						player1 = new JudgeDredd::Valkyrie(firstIsWhite, currBest);
						player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d));
						try
						{
							for (buffer=player1->makeMove(move), std::cout<<buffer.from.first<<" "<< buffer.from.second<<" " << buffer.to.first << " " << buffer.to.second<<std::endl; true;)
							{
								buffer=player2->makeMove(buffer);
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl;
								buffer = player1->makeMove(buffer);
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl;
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
							for (buffer = player2->makeMove(move); true;)
							{
								buffer = player1->makeMove(buffer);
								buffer = player2->makeMove(buffer);
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
	std::cout << std::endl << "RESULT:" << std::endl << a << "," << b << "," << c << "," << d << std::endl;
	std::cin >> a;
	return 0;
}