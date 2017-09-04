#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main()
{
	JudgeDredd::Valkyrie *player1, *player2;
	Move move, buffer;
	ChessEvaluator::ChessEvaluator currBest=ChessEvaluator::ChessEvaluator();
	float a, b, c, d;
	bool firstIsWhite=true;
	move.from = { 0,0 };
	move.to = { 0,0 };
	for(d=300;d>0;d-=10)
		for(c=300;c>0;c-=10)
			for(b=10;b>0;b-=.1f)
				for (a = 0; a < 10; a += .1f)
				{
					int firstWin = 0, secondWin = 0;
					for (int i = 0; i < 2; i++)
					{
						ChessBoard::Board board;
						std::cout << "Game number:" << i << " Configuration:" << a << "," << b << "," << c << "," << d << std::endl;
						player1 = new JudgeDredd::Valkyrie(firstIsWhite, currBest);
						player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d));
						try
						{
							for (buffer=player1->makeMove(move), std::cout<<buffer.from.first<<" "<< buffer.from.second<<" " << buffer.to.first << " " << buffer.to.second<<std::endl,board.ChangeState(ChessBoard::InternalMove(buffer)); true;)
							{
								buffer=player2->makeMove(buffer);
								board.ChangeState(ChessBoard::InternalMove(buffer));
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second <<(board.IsBlackChecked()?"+\n": "\n");
								buffer = player1->makeMove(buffer);
								board.ChangeState(ChessBoard::InternalMove(buffer));
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << (board.IsWhiteChecked() ? "+\n" : "\n");
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
						board = ChessBoard::Board();
						try
						{
							for (buffer = player2->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl, board.ChangeState(ChessBoard::InternalMove(buffer)); true;)
							{
								buffer = player1->makeMove(buffer);
								board.ChangeState(ChessBoard::InternalMove(buffer));
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << (board.IsBlackChecked() ? "+\n" : "\n");
								buffer = player2->makeMove(buffer);
								board.ChangeState(ChessBoard::InternalMove(buffer));
								std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << (board.IsWhiteChecked() ? "+\n" : "\n");
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