#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main()
{
	JudgeDredd::Valkyrie *player =new JudgeDredd::Valkyrie(true);
	Move move;
	move.from = { 0,0 };
	move.to = { 0,0 };
	int a;
	player->makeMove(move);
	std::cin >> a;
	return 0;
}