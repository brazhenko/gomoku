//
// Created by Lommy Reznak on 1/24/21.
//


#include "gtest/gtest.h"
#include "Board.h"
#include <fstream>
#include "Board.h"
// Тесты пишутся с помощью вот таких макросов:
// TEST(группа_тестов, название теста)
// {
// 		код теста
// }

// EXPECT_EQ - проверяет, что первый и второй параметры совпадают
// ASSERT_EQ - то же самое, только если проверка не выполняется, тесты прекращаются

// Есть куча других макросов EXPECT_*, ASSERT_*

// Выдержка

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros for testing equalities and inequalities.
//
//    * {ASSERT|EXPECT}_EQ(v1, v2): Tests that v1 == v2
//    * {ASSERT|EXPECT}_NE(v1, v2): Tests that v1 != v2
//    * {ASSERT|EXPECT}_LT(v1, v2): Tests that v1 < v2
//    * {ASSERT|EXPECT}_LE(v1, v2): Tests that v1 <= v2
//    * {ASSERT|EXPECT}_GT(v1, v2): Tests that v1 > v2
//    * {ASSERT|EXPECT}_GE(v1, v2): Tests that v1 >= v2
//
// When they are not, Google Test prints both the tested expressions and
// their actual values.  The values must be compatible built-in types,
// or you will get a compiler error.  By "compatible" we mean that the
// values can be compared by the respective operator.
//
// Note:
//
//   1. It is possible to make a user-defined type work with
//   {ASSERT|EXPECT}_??(), but that requires overloading the
//   comparison operators and is thus discouraged by the Google C++
//   Usage Guide.  Therefore, you are advised to use the
//   {ASSERT|EXPECT}_TRUE() macro to assert that two objects are
//   equal.
//
//   2. The {ASSERT|EXPECT}_??() macros do pointer comparisons on
//   pointers (in particular, C strings).  Therefore, if you use it
//   with two C strings, you are testing how their locations in memory
//   are related, not how their content is related.  To compare two C
//   strings by content, use {ASSERT|EXPECT}_STR*().
//
//   3. {ASSERT|EXPECT}_EQ(v1, v2) is preferred to
//   {ASSERT|EXPECT}_TRUE(v1 == v2), as the former tells you
//   what the actual value is when it fails, and similarly for the
//   other comparisons.
//
//   4. Do not depend on the order in which {ASSERT|EXPECT}_??()
//   evaluate their arguments, which is undefined.
//
//   5. These macros evaluate their arguments exactly once.
//
// Examples:
//
//   EXPECT_NE(Foo(), 5);
//   EXPECT_EQ(a_pointer, NULL);
//   ASSERT_LT(i, array_size);
//   ASSERT_GT(records.size(), 0) << "There is no record left.";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////
// Чтоб провелись тесты, надо собирать в терминале : cmake -DCMAKE_BUILD_TYPE=Debug && make
// Нужно протестировать класс Gomoku::Board
// Методы, кототорые считают определенное количество фигур на доске - особенно
// Вдохновиться тестами можно здесь: https://ru.wikipedia.org/wiki/%D0%A0%D1%8D%D0%BD%D0%B4%D0%B7%D1%8E

// Пример теста:
TEST(board, test1)
{
	std::ifstream is("test_files/board1.gg");
	// Если файл не открылся, тесты надо прекращать, что логично
    assert(is.is_open());

	Gomoku::Board b1;
	is >> b1;
	// Если один тест не прошел, можно тестировать дальше, поэтому EXPECT_EQ, а не ASSERT_EQ
	EXPECT_EQ(b1.GetStoneCount(), 7);
}


TEST(board, locality)
{
    std::ifstream is("test_files/board1.gg");
    assert(is.is_open());

    Gomoku::Board b1;
    is >> b1;
    // Если один тест не прошел, можно тестировать дальше, поэтому EXPECT

    EXPECT_EQ(b1.IsCellHasStoneNearby({7, 7}), true);
    EXPECT_EQ(b1.IsCellHasStoneNearby({7, 8}), true);
}


TEST(board, count_free_threes)
{
	{
		std::ifstream is("test_files/board3.gg");
		assert(is.is_open());
		Gomoku::Board b1;
		is >> b1;
		// Если один тест не прошел, можно тестировать дальше, поэтому EXPECT

		EXPECT_EQ(b1.CountFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b1.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/board4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 7	);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/board5.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 5);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 2);
	}
}

TEST(board, count_free_threes_row)
{
	{
		std::ifstream is("test_files/free_threes/r1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/r2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/r3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/r4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/r0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/free_threes/r0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/free_threes/r0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/free_threes/r0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_free_threes_column)
{
	{
		std::ifstream is("test_files/free_threes/c1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/c2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/c3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/c4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/c0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/free_threes/c0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/free_threes/c0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/free_threes/c0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_free_threes_diagonal)
{
	{
		std::ifstream is("test_files/free_threes/d1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/d2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/d3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/d4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/free_threes/d0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/free_threes/d0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/free_threes/d0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/free_threes/d0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_free_threes_random)
{
	{
		std::ifstream is("test_files/free_threes/random1_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/free_threes/random2_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/free_threes/random3_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/free_threes/random4_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_fours_row)
{
	{
		std::ifstream is("test_files/half_fours/r1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/r2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/r3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/r4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/r0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_fours/r0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_fours/r0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_fours/r0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_fours_column)
{
	{
		std::ifstream is("test_files/half_fours/c1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/c2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/c3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/c4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/c0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_fours/c0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_fours/c0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_fours/c0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_fours_diagonal)
{
	{
		std::ifstream is("test_files/half_fours/d1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/d2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/d3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/d4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_fours/d0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_fours/d0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_fours/d0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_fours/d0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_fours_random)
{
	{
		std::ifstream is("test_files/half_fours/random1_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_fours/random4_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeFours(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_threes_row)
{
	{
		std::ifstream is("test_files/half_threes/r1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/r2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/r3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/r4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/r0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_threes/r0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_threes/r0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_threes/r0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_threes_column)
{
	{
		std::ifstream is("test_files/half_threes/c1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/c2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/c3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/c4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/c0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_threes/c0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_threes/c0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_threes/c0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_threes_diagonal)
{
	{
		std::ifstream is("test_files/half_threes/d1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/d2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/d3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/d4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 0);
	}
	{
		std::ifstream is("test_files/half_threes/d0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_threes/d0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_threes/d0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_threes/d0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 0);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 4);
	}
}

TEST(board, count_half_threes_random)
{
	{
		std::ifstream is("test_files/half_threes/random1_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 1);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 1);
	}
	{
		std::ifstream is("test_files/half_threes/random2_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 2);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 2);
	}
	{
		std::ifstream is("test_files/half_threes/random3_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 3);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 3);
	}
	{
		std::ifstream is("test_files/half_threes/random4_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 4);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 4);
	}
	{
		std::ifstream is("test_files/half_threes/random5_5.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::White), 5);
		EXPECT_EQ(b.CountHalfFreeThrees(Gomoku::Board::Side::Black), 5);
	}
}

TEST(board, count_free_fours_row)
{
	{
		std::ifstream is("test_files/free_fours/r1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/r2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/r3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/r4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/r0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/r0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/r0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/r0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
}

TEST(board, count_free_fours_column)
{
	{
		std::ifstream is("test_files/free_fours/c1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/c2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/c3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/c4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/c0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/c0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/c0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/c0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
}

TEST(board, count_free_fours_diagonal)
{
	{
		std::ifstream is("test_files/free_fours/d1_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/d2_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/d3_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/d4_0.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), false);
	}
	{
		std::ifstream is("test_files/free_fours/d0_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/d0_2.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/d0_3.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/d0_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), false);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
}

TEST(board, count_free_fours_random)
{
	{
		std::ifstream is("test_files/free_fours/random1_1.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
	{
		std::ifstream is("test_files/free_fours/random4_4.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;

		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_w), true);
		EXPECT_EQ(b.IsThereFigureOnBoard(Gomoku::Board::figure_free_four_b), true);
	}
}

#include "Players/AI2.h"
#include "Players/AI1.h"
#include "Engine.h"
TEST(datastructure, 1)
{
//    auto bot = Gomoku::PlayerFactory("TestAI", Gomoku::Board::Side::White, Gomoku::MakeMove_t{}, Gomoku::Board{}, false);
//	Gomoku::AI1 ai1(Gomoku::Board::Side::White, Gomoku::MakeMove_t{}, Gomoku::Board{}, false, true);
//	while (true)
//		;


	{
		std::ifstream is("/Users/lreznak-/Desktop/gomoku_04-03-202118-26-22.gg");
		assert(is.is_open());
		Gomoku::Board b;
		is >> b;
		std::cout << "Eval: " << Gomoku::Engine::StaticPositionAnalize(b);
	}

}