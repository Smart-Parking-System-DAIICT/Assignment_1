#include <gtest/gtest.h>
#include "snake.h"
#include <deque>
#include <vector>

using namespace std;

TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'r'), make_pair(current.first, (current.second + 1) % 10));
}

TEST(SnakeBehaviour, NextHeadLeft) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'l'), make_pair(current.first, current.second == 0 ? 9 : current.second - 1));
}

TEST(SnakeBehaviour, NextHeadUp) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'u'), make_pair(current.first == 0 ? 9 : current.first - 1, current.second));
}

TEST(SnakeBehaviour, NextHeadDown) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'd'), make_pair((current.first + 1) % 10, current.second));
}

TEST(SnakeBehaviour, WrapAroundRightEdge) {
    pair<int, int> current = {4, 9};
    EXPECT_EQ(get_next_head(current, 'r', 10), make_pair(4, 0));
}

TEST(SnakeBehaviour, WrapAroundLeftEdge) {
    pair<int, int> current = {4, 0};
    EXPECT_EQ(get_next_head(current, 'l', 10), make_pair(4, 9));
}

TEST(SnakeBehaviour, WrapAroundTopEdge) {
    pair<int, int> current = {0, 7};
    EXPECT_EQ(get_next_head(current, 'u', 10), make_pair(9, 7));
}

TEST(SnakeBehaviour, WrapAroundBottomEdge) {
    pair<int, int> current = {9, 7};
    EXPECT_EQ(get_next_head(current, 'd', 10), make_pair(0, 7));
}

TEST(SnakeUtils, UpdateHighScoresSorted) {
    vector<int> scores = {50, 40, 30};
    update_high_scores(scores, 60);
    EXPECT_EQ(scores[0], 60);
    EXPECT_EQ(scores.size(), 4);
}

TEST(SnakeUtils, HighScoresLimitedTo10) {
    vector<int> scores = {100,90,80,70,60,50,40,30,20,10};
    update_high_scores(scores, 95);
    EXPECT_EQ(scores.size(), 10);
    EXPECT_EQ(scores[1], 95);
}

TEST(SnakeUtils, GenerateItemNotOnSnakeOrOther) {
    deque<pair<int,int>> snake = {{0,0}, {0,1}, {0,2}};
    pair<int,int> other = {1,1};
    pair<int,int> item = generate_item(10, snake, other);

    EXPECT_TRUE(find(snake.begin(), snake.end(), item) == snake.end());
    EXPECT_NE(item, other);
}

/** 
 * g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
 * This command is a two-part shell command. Let's break it down.

  The first part is the compilation:
  g++ -o my_tests hello_gtest.cpp -lgtest -lgtest_main -pthread


   * g++: This invokes the GNU C++ compiler.
   * -o my_tests: This tells the compiler to create an executable file named
     my_tests.
   * hello_gtest.cpp: This is the C++ source file containing your tests.
   * -lgtest: This links the Google Test library, which provides the core testing
     framework.
   * -lgtest_main: This links a pre-compiled main function provided by Google
     Test, which saves you from writing your own main() to run the tests.
   * -pthread: This links the POSIX threads library, which is required by Google
     Test for its operation.
 * 
*/