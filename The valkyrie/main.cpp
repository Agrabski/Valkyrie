#include <stdio.h>
#include"Player1.h"
#include <stdlib.h>

int main()
{
	JudgeDredd::Valkyrie *player =new JudgeDredd::Valkyrie(true);
	Move move;
	move.from = { 0,0 };
	move.to = { 0,0 };

	player->makeMove(move);
	return 0;
}