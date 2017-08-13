#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "Player2.h"

JudgeDredd::Player2::Player2(bool amIWhite)
{
    x=-1;

    library[0].whiteMove=false;
    library[0].special=false;
    library[0].from={1, 0};
    library[0].to={2, 2};

    library[1].whiteMove=false;
    library[1].special=false;
    library[1].from={3, 1};
    library[1].to={3, 3};

    library[2].whiteMove=false;
    library[2].special=false;
    library[2].from={2, 2};
    library[2].to={1, 4};

    library[3].whiteMove=false;
    library[3].special=true;
    library[3].from={2, 0};
    library[3].to={6, 4};
    library[3].type='C';

    library[4].whiteMove=false;
    library[4].special=false;
    library[4].from={4, 0};
    library[4].to={3, 1};

    library[5].whiteMove=false;
    library[5].special=false;
    library[5].from={6, 0};
    library[5].to={5, 2};

    library[6].whiteMove=false;
    library[6].special=true;
    library[6].from={1, 4};
    library[6].to={3, 3};
    library[6].type='B';

    library[7].whiteMove=false;
    library[7].special=true;
    library[7].from={3, 3};
    library[7].to={4, 5};
    library[7].type='M';
}

JudgeDredd::Player2::~Player2()
{

}

Move JudgeDredd::Player2::makeMove(struct Move lastMove)
{
    x++;
    return library[x];
}
