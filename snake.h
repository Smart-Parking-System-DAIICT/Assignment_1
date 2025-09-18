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
#include <atomic>
using namespace std;
using namespace std::this_thread;

char direction = 'r';
atomic<bool> paused(false);
vector<int> high_scores;

void input_handler() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}
    };

    while (true) {
        char input = getchar();

        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
            if (direction == 'q') exit(0);
        }
        else if (input == ' ') {  
            paused = !paused;  // toggle pause
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void update_high_scores(int score) {
    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());
    if (high_scores.size() > 10) {
        high_scores.resize(10);
    }
}

void show_high_scores() {
    cout << "\n Top 10 High Scores" << endl;
    for (size_t i = 0; i < high_scores.size(); i++) {
        cout << i + 1 << ". " << high_scores[i] << endl;
    }
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

void game_over(string reason, int score) {
    system("clear");
    cout << "Game Over: " << reason << endl;
    cout << "Final Score: " << score << endl;

    update_high_scores(score);
    show_high_scores();

    exit(0);
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

        if (paused) {
            cout << "Game Paused - Press Space to Resume" << endl;
            sleep_for(200ms);
            continue;
        }

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            game_over("Game Over (You hit yourself)", score);
        }

        if (head == poison) {
            game_over("Game Over (Ate poison)", score);
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
        cout << "    Length: " << snake.size()
             << "    Score: " << score
             << "    Speed: " << delay_ms << "ms"
             << "    [Space: Pause/Resume]" << endl;

        sleep_for(milliseconds(delay_ms));
    }
}
