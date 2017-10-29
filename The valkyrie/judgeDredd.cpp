#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "stdio.h"
#include <cstdlib>

#include "judgeDredd.h"
#include "Player1.h"
#include "Player2.h"

JudgeDredd::JudgeDredd(bool isWhite, int givenTime)
{
    int x, y;

    for( y=2 ; y<6 ; y++ )
    {
        for( x=0 ; x<6 ; x++ )
            board[x][y]=empty;
    }
    for( x=0; x<8 ; x++ )
    {
        board[x][1]=pownW;
        board[x][6]=pownB;
    }
    board[0][0]=board[7][0]=towerW;
    board[0][7]=board[7][7]=towerB;
    board[1][0]=board[6][0]=knightW;
    board[1][7]=board[6][7]=knightB;
    board[2][0]=board[5][0]=bishopW;
    board[2][7]=board[5][7]=bishopB;
    board[3][0]=kingW;
    board[3][7]=kingB;
    board[4][0]=queenW;
    board[4][7]=queenB;

    fiftyMoves=0;
    castleKW=true;
    castleQW=true;
    castleKB=true;
    castleQB=true;
    isWhiteP1=isWhite;
    enPassan={-1, -1};
    score={0, 0};
    time={0, 0};

    lastMove.special=false;
    lastMove.from={-1, -1};
    lastMove.to={-1, -1};
    lastMove.type={'?'};
//    player1=new JudgeDredd::Valkyrie(isWhiteP1);
  //  player2=new JudgeDredd::Player2(!isWhiteP1);


}

JudgeDredd::~JudgeDredd()
{

}

void JudgeDredd::resetBoard()
{
    int x, y;

    for( y=2 ; y<6 ; y++ )
    {
        for( x=0 ; x<6 ; x++ )
            board[x][y]=empty;
    }
    for( x=0; x<8 ; x++ )
    {
        board[x][1]=pownW;
        board[x][6]=pownB;
    }
    board[0][0]=board[7][0]=towerW;
    board[0][7]=board[7][7]=towerB;
    board[1][0]=board[6][0]=knightW;
    board[1][7]=board[6][7]=knightB;
    board[2][0]=board[5][0]=bishopW;
    board[2][7]=board[5][7]=bishopB;
    board[3][0]=kingW;
    board[3][7]=kingB;
    board[4][0]=queenW;
    board[4][7]=queenB;

    fiftyMoves=0;
    castleKW=true;
    castleQW=true;
    castleKB=true;
    castleQB=true;
    isWhiteP1=!isWhiteP1;
    enPassan={-1, -1};
    time={0, 0};

    lastMove.special=false;
    lastMove.from={-1, -1};
    lastMove.to={-1, -1};
    lastMove.type={'?'};

    delete player1;
    delete player2;
//    player1=new Valkyrie(isWhiteP1);
  //  player2=new Player2(!isWhiteP1);
}

bool JudgeDredd::move()
{
    if(isWhiteP1)
    {
        lastMove=player1->makeMove(lastMove);


       if(!validMove())
       {
           std::cout << "P1 fucked up!";
           return false;
       }
       getchar();
       system("CLS");
       printBoard();
    }
    while(!isOver())
    {
        lastMove=player2->makeMove(lastMove);

        if(!validMove())
       {
           std::cout << "P2 fucked up!";
           return false;
       }
        getchar();
        system("CLS");
        printBoard();

       if(isOver())
        break;

        lastMove=player1->makeMove(lastMove);

       if(!validMove())
       {
           std::cout << "P1 fucked up!";
           return false;
       }
       getchar();
       system("CLS");
       printBoard();

    }
    return true;
}

bool JudgeDredd::isOver()
{
    return false;
}

bool JudgeDredd::validMove()
{

    board[lastMove.to.first][lastMove.to.second]=board[lastMove.from.first][lastMove.from.second];
    board[lastMove.from.first][lastMove.from.second]=empty;
    return true;
}

void JudgeDredd::printScore()
{
    std::cout << "Player 1: " << score.first << "Player 2: " << score.second;
}

void JudgeDredd::printBoard()
{
    int x, y;

    for( y=7 ; y>=0 ; y--)
    {
        for( x=0 ; x<8 ;x++)
        {

            switch(board[x][y])
            {
                case rank::empty    : std::cout << "_______";   break;
                case rank::pownW    : std::cout << " pownW ";   break;
                case rank::pownB    : std::cout << " pownB ";   break;
                case rank::knightW  : std::cout << "knightW";   break;
                case rank::knightB  : std::cout << "knightB";   break;
                case rank::bishopW  : std::cout << "bishopW";   break;
                case rank::bishopB  : std::cout << "bishopB";   break;
                case rank::towerW   : std::cout << "towerW ";   break;
                case rank::towerB   : std::cout << "towerB ";   break;
                case rank::queenW   : std::cout << "queenW ";   break;
                case rank::queenB   : std::cout << "queenB ";   break;
                case rank::kingW    : std::cout << " kingW ";   break;
                case rank::kingB    : std::cout << " kingB ";   break;
            }
            std::cout << "\t";
        }
        std::cout << "\n\n\n";
    }
}
