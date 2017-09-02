#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\The valkyrie\chessboard.h"
#include "..\The valkyrie\Evaluator.cpp"
#include "..\The valkyrie\Player1.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MAKE_MOVE(x1,y1,x2,y2)board.ChangeState( ChessBoard::InternalMove({x1-1,y1-1},{x2-1,y2-1},ChessBoard::Standard));
#define RANK(x,y) archetype.fields[x][y].rank
#define MOVE(x1,y1,x2,y2) board.ChangeState(ChessBoard::InternalMove({ x1,y1 }, { x2,y2 }, ChessBoard::Standard));


namespace Valkyrietests
{		
	TEST_CLASS(BorderlineCases)
	{
	public:

		TEST_METHOD(DoubleMoveWhite)
		{
			// TODO: Your test code here
			ChessBoard::Board board;
			try
			{
				board.ChangeState(ChessBoard::InternalMove({ 0,1 }, { 0,2 }, ChessBoard::Standard));
			}
			catch (std::exception exc)
			{
				Assert::Fail();
			}
			Assert::IsTrue(board.fields[0][1].rank.type == ChessBoard::Empty&&board.fields[0][2].rank.type == ChessBoard::Pawn);
		}

		TEST_METHOD(PawnBeatingEmptyField)
		{
			try
			{
				ChessBoard::Board board;
				board.ChangeState({ { 0,1 },{ 1,2 },ChessBoard::Standard });
			}
			catch (ChessBoard::INVALID_MOVE m)
			{
				Assert::IsTrue(true);
				return;
			}
		}

		TEST_METHOD(BlackMovingFirst)
		{

			// TODO: Your test code here
			ChessBoard::Board board;
			try
			{
				board.ChangeState(ChessBoard::InternalMove({ 0,6 }, { 0,5 }, ChessBoard::Standard));
			}
			catch (ChessBoard::WRONG_COLOR e)
			{
				Assert::IsTrue(true);
				return;
			}
			Assert::Fail();
		}

		TEST_METHOD(MoveAfterCheckmate)
		{
			ChessBoard::Board board;
			MOVE(4, 1, 4, 2);
			MOVE(0, 6, 0, 5);
			MOVE(5, 0, 2, 3);
			MOVE(0, 5, 0, 4);
			MOVE(3, 0, 7, 4);
			MOVE(0, 4, 0, 3);
			MOVE(7, 4, 5, 6);
			try
			{
				MOVE(1, 5, 1, 4);
			}
			catch (ChessBoard::KING_IN_DANGER exception)
			{
				Assert::IsFalse(exception.isWhite && !exception.isBlack);
			}
		}


		TEST_METHOD(InvalidTowerMove)
		{

			ChessBoard::Board board;
			try
			{
				MOVE(7, 0, 7, 5);
			}
			catch (ChessBoard::MOVE_BLOCKED)
			{
				Assert::IsTrue(true);
			}
			board = ChessBoard::Board();
			MOVE(6, 1, 6, 3);
			MOVE(6, 6, 6, 5);
			try
			{
				MOVE(7, 0, 4, 3);
			}
			catch (ChessBoard::INVALID_MOVE)
			{
				Assert::IsTrue(true);
				return;
			}
			Assert::Fail();
		}

		TEST_METHOD(InvalidKnightMove)
		{
			ChessBoard::Board board;
			try
			{
				board.ChangeState(ChessBoard::InternalMove({ 1,0 }, { 1,3 }, ChessBoard::Standard));
				Assert::Fail();
			}
			catch (ChessBoard::INVALID_MOVE)
			{
				Assert::IsTrue(true);
			}
			try
			{
				board.ChangeState(ChessBoard::InternalMove({ 1,0 }, { 2,5 }, ChessBoard::Standard));
				Assert::Fail();
			}
			catch (ChessBoard::INVALID_MOVE)
			{
				Assert::IsTrue(true);
			}
		}

		TEST_METHOD(InvalidBishopMove)
		{
			ChessBoard::Board board;
			ChessBoard::Board archetype;
			board.ChangeState(ChessBoard::InternalMove({ 2,1 }, { 2,3 }, ChessBoard::Standard));
			archetype.ChangeState(ChessBoard::InternalMove({ 2,1 }, { 2,3 }, ChessBoard::Standard));
			board.ChangeState(ChessBoard::InternalMove({ 0,6 }, { 0,5 }, ChessBoard::Standard));
			archetype.ChangeState(ChessBoard::InternalMove({ 0,6 }, { 0,5 }, ChessBoard::Standard));
			try
			{
				board.ChangeState(ChessBoard::InternalMove({ 2,0 }, { 2,2 }, ChessBoard::Standard));
			}
			catch (ChessBoard::INVALID_MOVE)
			{
				Assert::IsTrue(true);
			}
			Assert::IsTrue(archetype == board);
		}

		TEST_METHOD(iteratorTest)
		{
			int count = 0;
			ChessBoard::Board board;
			ChessBoard::Board::Moves iterator(&board);
			iterator.Reset(true);
			for (++iterator; *iterator != nullptr; ++iterator)
			{
				try
				{
					board.ChangeState(**iterator);
					count++;
					board.Revert();
				}
				catch(ChessBoard::INVALID_MOVE)
				{ }
				catch(ChessBoard::WRONG_COLOR)
				{ }
			}

			Assert::IsTrue(count == 20);
		}
	};

	TEST_CLASS(RealGame)
	{
		TEST_METHOD(RochadeBothSides)
		{
			ChessBoard::Board board;
			ChessBoard::Board archetype;
			for (int x = 0; x < 8; x++)
				for (int y = 0; y < 8; y++)
					archetype.fields[x][y].rank.type = ChessBoard::Empty;
			RANK(0, 7) = RANK(5, 7) = { ChessBoard::Tower,false };
			RANK(0, 6) = RANK(1, 6) = RANK(2, 4) = RANK(3, 5) = RANK(4, 4) = RANK(5, 6) = RANK(6, 5) = RANK(7, 6) = { ChessBoard::Pawn,false };
			RANK(2, 7) = RANK(6, 6) = { ChessBoard::Bishop,false };
			RANK(3, 3) = RANK(4, 6) = { ChessBoard::Knight,false };
			RANK(3, 7) = { ChessBoard::Queen,false };
			RANK(6, 7) = { ChessBoard::King,false };
			RANK(0, 1) = RANK(1, 1) = RANK(2, 1) = RANK(3, 2) = RANK(4, 3) = RANK(5, 3) = RANK(6, 2) = RANK(7, 1) = { ChessBoard::Pawn,true };
			RANK(0, 0) = RANK(5, 0) = { ChessBoard::Tower,true };
			RANK(2, 2) = RANK(5, 2) = { ChessBoard::Knight,true };
			RANK(4, 2) = RANK(6, 1) = { ChessBoard::Bishop,true };
			RANK(6, 0) = { ChessBoard::King,true };
			RANK(3, 1) = { ChessBoard::Queen,true };
			MAKE_MOVE(5, 2, 5, 4);
			MAKE_MOVE(3, 7, 3, 5);
			MAKE_MOVE(2, 1, 3, 3);
			MAKE_MOVE(2, 8, 3, 6);
			MAKE_MOVE(7, 2, 7, 3);
			MAKE_MOVE(7, 7, 7, 6);
			MAKE_MOVE(4, 2, 4, 3);
			MAKE_MOVE(6, 8, 7, 7);
			MAKE_MOVE(3, 1, 5, 3);
			MAKE_MOVE(4, 7, 4, 6);
			MAKE_MOVE(4, 1, 4, 2);
			MAKE_MOVE(5, 7, 5, 5);
			MAKE_MOVE(6, 1, 7, 2);
			MAKE_MOVE(7, 8, 5, 7);
			MAKE_MOVE(6, 2, 6, 4);
			MAKE_MOVE(3, 6, 4, 4);
			MAKE_MOVE(7, 1, 6, 3);
			board.ChangeState(ChessBoard::InternalMove({ -1,-1 }, { 0,0 }, ChessBoard::RochadeRight));
			board.ChangeState(ChessBoard::InternalMove({ -1,-1 }, { 0,0 }, ChessBoard::RochadeRight));
			Assert::IsTrue(archetype == board);
		}

	};

	TEST_CLASS(OptymalisationTests)
	{
		ChessEvaluator::ChessEvaluator tmp;
		TEST_METHOD(RecursionLevel1)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp,1);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel2)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 2);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel3)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 3);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel4)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 4);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel5)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 5);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel6)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 6);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel7)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 7);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel8)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 8);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel9)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 9);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel10)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 10);
			Move tmp;
			player->makeMove(tmp);
		}

		TEST_METHOD(RecursionLevel11)
		{
			JudgeDredd::Valkyrie *player = new JudgeDredd::Valkyrie(true,tmp, 11);
			Move tmp;
			player->makeMove(tmp);
		}
	};
}