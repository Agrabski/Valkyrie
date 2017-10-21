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
	return stream << toWrite.friendlyValue << " " << toWrite.enemyValue << " " << toWrite.friendlyCheck << " " << toWrite.enemyCheck <<" "<<toWrite.pawnConstant<< std::endl;
}

void tester1()
{
	double averageTime[7-3];
	long int moveCount[7 - 3] = {0};
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
					for(double e=0;e<5;e+=.01f)
					{
						int firstWin = 0, secondWin = 0;
						for (int i = 3; i < 7; i++)
						{
							std::cout << "Game number:" << i << " Configuration:" << a << "," << b << "," << c << "," << d << std::endl << "Current Best:" << currBest << std::endl;
							stream << currBest << "----" << ChessEvaluator::ChessEvaluator(a, b, c, d,e) << "--\n";
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
							player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d,e), i);
							time_t t2 = time(0);
							try
							{
								for (moveCount[i-3]++,buffer = player1->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
								{
									buffer = player2->makeMove(buffer);
									moveCount[i - 3]++;
									if (displayMoves)
										std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
									buffer = player1->makeMove(buffer);
									moveCount[i - 3]++;
									if (displayMoves)
										std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								}
							}
							catch (GAME_ENDED err)
							{
								averageTime[i - 3] += time(0) - t2;
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
							player2 = new JudgeDredd::Valkyrie(firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d,e), i);
							t2 = time(0);
							try
							{
								for (moveCount[i - 3]++, buffer = player2->makeMove(move), std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << std::endl; true;)
								{
									buffer = player1->makeMove(buffer);
									moveCount[i - 3]++;
									if (displayMoves)
										std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
									buffer = player2->makeMove(buffer);
									moveCount[i - 3]++;
									if (displayMoves)
										std::cout << buffer.from.first << " " << buffer.from.second << " " << buffer.to.first << " " << buffer.to.second << "\n";
								}
							}
							catch (GAME_ENDED err)
							{
								averageTime[i - 3] += time(0) - t2;
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
							currBest = ChessEvaluator::ChessEvaluator(a, b, c, d,e);
					}
				}
			}
		}
	}
	for (int i = 7 - 3; i > 0; i--)
		averageTime[i] = averageTime[i] /(double)moveCount[i];
	std::cout << std::endl << "RESULT:" << std::endl << a << "," << b << "," << c << "," << d << std::endl;
	std::cout << "TIMES:" << std::endl;
	for (int i = 7 - 3; i > 0; i--)
	{
		std::cout << "Recursion Depth:" << i << averageTime[i] << std::endl;
	}
	system("pause");
	stream.close();
	std::cin >> a;
}

void tester2()
{
	Move move, buffer;
	std::fstream stream;
	JudgeDredd::Valkyrie *player1, *player2;
	stream.open("debug.txt", 'w');
	for (int i = 2; i < 10; i++)
	{
		int moveCount = 1;
		std::cout << i << std::endl;
		time_t t1 = time(0);   // get time now
		player1 = new JudgeDredd::Valkyrie(true, ChessEvaluator::ChessEvaluator(), i);
		player2 = new JudgeDredd::Valkyrie(false, ChessEvaluator::ChessEvaluator(), i);
		try
		{
			for (buffer = player1->makeMove(move); true;)
			{
				buffer = player2->makeMove(buffer);
				moveCount++;
				buffer = player1->makeMove(buffer);
				moveCount++;
			}
		}
		catch (GAME_ENDED err)
		{
		}
		stream <<i<<"	"<< (double)(time(0) - t1)/(double)moveCount <<std::endl;
		std::cout <<"RECURSION DEPTH:" << i << "-" << (double)(time(0) - t1) / (double)moveCount << std::endl;
		delete player1;
		delete player2;
	}
	std::cin.ignore();
	stream.close();
}

void tester3()
{
	double averageTime[7 - 3];
	int completion = 0;
	long int moveCount[7 - 3] = { 0 };
	bool displayMoves, displayTime;
	std::ofstream stream;
	stream.open("debug.txt", 'w');
	JudgeDredd::Valkyrie *player1, *player2;
	Move move, buffer;
	ChessEvaluator::ChessEvaluator currBest = ChessEvaluator::ChessEvaluator();
	float a, b, c, d,e;
	bool firstIsWhite = true;
	move.from = { 0,0 };
	move.to = { 0,0 };
	for (d = 0; d < 300; d += 10)
	{
		for (c = 0; c < 300; c += 10)
		{
			for (b = -10; b < 10; b += .1f)
			{
				for (a = -10; a <= 10; a += .1f)
				{
					for (e = 0; e<5; e += .1f)
					{
						int firstWin = 0, secondWin = 0;
						for (int i = 3; i < 7; i++)
						{
							player1 = new JudgeDredd::Valkyrie(firstIsWhite, currBest, i);
							player2 = new JudgeDredd::Valkyrie(!firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d, e), i);
							time_t t2 = time(0);
							try
							{
								for (moveCount[i - 3]++, buffer = player1->makeMove(move); true;)
								{
									buffer = player2->makeMove(buffer);
									moveCount[i - 3]++;
									buffer = player1->makeMove(buffer);
									moveCount[i - 3]++;
								}
							}
							catch (GAME_ENDED err)
							{
								averageTime[i - 3] += time(0) - t2;
								if (err.blackWon)
								{
									secondWin += 1;
								}
								if (err.whiteWon)
								{
									firstWin += 1;
								}

							}
							delete player1;
							delete player2;

							player1 = new JudgeDredd::Valkyrie(!firstIsWhite, currBest, i);
							player2 = new JudgeDredd::Valkyrie(firstIsWhite, ChessEvaluator::ChessEvaluator(a, b, c, d, e), i);
							t2 = time(0);
							try
							{
								for (moveCount[i - 3]++, buffer = player2->makeMove(move); true;)
								{
									buffer = player1->makeMove(buffer);
									moveCount[i - 3]++;
									buffer = player2->makeMove(buffer);
									moveCount[i - 3]++;
								}
							}
							catch (GAME_ENDED err)
							{
								averageTime[i - 3] += time(0) - t2;
								if (err.whiteWon)
								{
									secondWin += 1;
								}
								if (err.blackWon)
								{
									firstWin += 1;
								}
							}
							delete player1;
							delete player2;
						}
						completion++;
						if (secondWin > firstWin)
							currBest = ChessEvaluator::ChessEvaluator(a, b, c, d, e);
						std::cout << currBest << std::endl<<"Current Averages:"<<std::endl;
						for (int i = 7 - 3; i > 0; i--)
						{
							std::cout << "Recursion Depth:" << i+3 << " " << averageTime[i]/moveCount[i] << std::endl;
						}
						std::cout << "COMPLETION:" <<100.0 * (double)completion / (30.0 * 30.0 * 20.0 * 20.0 * 50.0)<<"%"<<std::endl;
					}
				}
			}
		}
	}
	for (int i = 7 - 3; i > 0; i--)
		averageTime[i] = averageTime[i] / (double)moveCount[i];
	std::cout << std::endl << "RESULT:" << std::endl << a << "," << b << "," << c << "," << d <<","<<e <<std::endl;
	std::cout << "TIMES:" << std::endl;
	for (int i = 7 - 3; i > 0; i--)
	{
		std::cout << "Recursion Depth:" << i <<" "<< averageTime[i] << std::endl;
	}
	system("pause");
	stream.close();
	std::cin >> a;
}

void tester4(int times, char* name)
{
	Move move, buffer;
	std::fstream stream;
	JudgeDredd::Valkyrie *player1, *player2;
	stream.open("queue.txt", 'w');
	time_t timeElapsed[6] = { 0 };
	int moveCount[6] = {1};
	while (times)
	{
		for (int i = 2; i < 8; i++)
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
					moveCount[i-2]++;
					buffer = player1->makeMove(buffer);
					moveCount[i-2]++;
				}
			}
			catch (GAME_ENDED err)
			{
			}
			timeElapsed[i - 2] += (time(0) - t1);
			std::cout << "RECURSION DEPTH:" << i << "-" << (double)timeElapsed[i - 2] / (double)moveCount[i-2] << std::endl;
			delete player1;
			delete player2;
		}
		times--;
	}
	for (int i = 0; i < 6; i++)
		stream << i + 2 << "	" << ((double)timeElapsed[i]) / ((double)moveCount[i]) << std::endl;
	stream.close();
}



int main(int argc, char * argv[])
{
	int selectedTester = 1;
	if (argc >= 2)
		selectedTester = atoi(argv[1]);
	switch (selectedTester)
	{
	case 1:
		tester1();
		return 0;
	case 2:
		tester2();
		return 0;
	case 4:
		tester4(atoi(argv[2]), argv[0]);
	}
	return 0;
}