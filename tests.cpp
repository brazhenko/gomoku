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

// Чтоб провелись тесты, надо собирать в терминале : cmake -DCMAKE_BUILD_TYPE=Debug && make
// Нужно протестировать класс Gomoku::Board

// Пример теста:
TEST(board, test1)
{
	std::ifstream is("../test_files/board1.gg");
	// Если файл не открылся, тесты надо прекращать, что логично
	ASSERT_TRUE(is.is_open());

	Gomoku::Board b1;
	is >> b1;
	// Если один тест не прошел, можно тестировать дальше, поэтому EXPECT
	EXPECT_EQ(b1.StoneCount(), 7);
}