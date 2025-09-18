#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

char direction = 'r';

void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q') {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️";
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
}

pair<int, int> get_next_head(pair<int, int> current, char direction) {
    pair<int, int> next;
    if (direction == 'r') {
        next = make_pair(current.first, (current.second + 1) % 10);
    } else if (direction == 'l') {
        next = make_pair(current.first, current.second == 0 ? 9 : current.second - 1);
    } else if (direction == 'd') {
        next = make_pair((current.first + 1) % 10, current.second);
    } else if (direction == 'u') {
        next = make_pair(current.first == 0 ? 9 : current.first - 1, current.second);
    }
    return next;
}

pair<int,int> generate_item(int size, deque<pair<int,int>> &snake, pair<int,int> other) {
    pair<int,int> pos;
    do {
        pos = make_pair(rand() % size, rand() % size);
    } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == other);
    return pos;
}

void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    int gridSize = 10;
    pair<int, int> food = generate_item(gridSize, snake, {-1,-1});
    pair<int, int> poison = generate_item(gridSize, snake, food);

    int foods_eaten = 0;
    int delay_ms = 500;
    const int min_delay = 100;
    int score = 0;

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        cout << "\033[H";

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over (hit yourself)" << endl;
            cout << "Final Score: " << score << endl;
            exit(0);
        }

        if (head == poison) {
            system("clear");
            cout << "Game Over (ate poison)" << endl;
            cout << "Final Score: " << score << endl;
            exit(0);
        }

        if (head == food) {
            snake.push_back(head);
            food = generate_item(gridSize, snake, poison);
            foods_eaten++;
            score += 10;

            if (foods_eaten % 10 == 0 && delay_ms > min_delay) {
                delay_ms -= 50;
            }
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        poison = generate_item(gridSize, snake, food);

        render_game(gridSize, snake, food, poison);
        cout << "Length of snake: " << snake.size()
             << "    Score: " << score
             << "    Speed: " << delay_ms << "ms" << endl;

        sleep_for(milliseconds(delay_ms));
    }
}