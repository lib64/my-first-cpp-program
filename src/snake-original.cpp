#include <ncurses.h>
#include <random>
#include <unistd.h>
#include <vector>

typedef std::vector<int> rows;
typedef std::vector<rows> matrix;

std::pair<int, int> initState(matrix &myState);
int printState(matrix myState);
std::pair<int, int> randXY();
int randDirection();
int randFood();
int randDiamond();
int move(int direction, matrix &myState, matrix &myTimeState,
         std::pair<int, int> &myPos);
int eMove(int direction, matrix &myState, matrix &myTimeState,
          std::pair<int, int> &myPos);
int checkExpired(matrix &myState, matrix &myTimeState);
int genDiamond(matrix &myState, matrix &myTimeState);
int genHeart(matrix &myState, matrix &myTimeState);
int genEnemy(matrix &myState, matrix &myTimeState, int &edirection,
             std::pair<int, int> &epos);
int randHeart();

matrix state;
matrix timeState;

std::pair<int, int> pos;
std::pair<int, int> epos;

int direction = 0;
int edirection = 0;
int snakeTime = 1;
int foodTotal = 0;
int heartTotal = 0;
int diamondTotal = 0;
int totalScore = 0;
int heart = randHeart();

int c1 = 0;
int c2 = 0;
int c3 = 0;
int over = 0;

int main() {
    pos = initState(state);

    int diamond = randDiamond();
    // int heart = randHeart();
    int enemy = randDiamond();

    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (over == 0) {
        if (snakeTime < 1) {
            over = 1;
        }

        if (diamond == 0) {
            genDiamond(state, timeState);
            diamond = randDiamond();
        }
        if (diamond > 0) {
            diamond--;
        }

        if (heart == 0) {
            genHeart(state, timeState);
            heart = randHeart();
        }
        if (heart > 0) {
            heart--;
        }

        if (enemy == 0) {
            genEnemy(state, timeState, edirection, epos);
            enemy = randDiamond();
        }
        if (enemy > 0) {
            enemy--;
        }

        int ch = 0;

        halfdelay(1);
        ch = getch();

        switch (ch) {
        case 258:
            if (direction == 3) {
                break;
            }
            direction = 2;
            break; // down
        case 259:
            if (direction == 2) {
                break;
            }
            direction = 3;
            break; // up
        case 260:
            if (direction == 1) {
                break;
            }
            direction = 0;
            break; // left
        case 261:
            if (direction == 0) {
                break;
            }
            direction = 1;
            break; // right
        }
        over = move(direction, state, timeState, pos);

        eMove(edirection, state, timeState, epos);
        int em = randFood();
        if (em < 18) {
            switch (edirection) {
            case 0:
                edirection = 2;
                break;
            case 1:
                edirection = 3;
                break;
            case 2:
                edirection = 0;
                break;
            case 3:
                edirection = 1;
                break;
            }
        } else if (em > 97) {
            switch (edirection) {
            case 0:
                edirection = 3;
                break;
            case 1:
                edirection = 2;
                break;
            case 2:
                edirection = 1;
                break;
            case 3:
                edirection = 0;
                break;
            }
        }

        checkExpired(state, timeState);
        printState(state);
    }
    endwin();
    return 0;
}

// initialize the game
std::pair<int, int> initState(matrix &myState) {
    for (int i = 0; i < 21; i++) {
        rows myRow;
        for (int j = 0; j < 80; j++) {
            myRow.push_back(0);
        }
        state.push_back(myRow);
        timeState.push_back(myRow);
    }

    std::pair<int, int> ret;
    ret.first = 10;
    ret.second = 50;
    return ret;
}

// print the game state
int printState(matrix myState) {
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);

    for (int i = 0; i < 20; i++) {
        for (size_t j = 0; j < 79; j++) {
            switch (myState[i][j]) {
            case 0:
                mvprintw(i, j, " ");
                break;
            case 1:
                attron(COLOR_PAIR(2));
                mvprintw(i, j, "#");
                attron(COLOR_PAIR(1));

                break;
            case 2:
                attron(COLOR_PAIR(3));
                mvprintw(i, j, "o");
                attron(COLOR_PAIR(1));
                break;
            case 3:
                attron(COLOR_PAIR(5));
                mvprintw(i, j, "*");
                attron(COLOR_PAIR(1));
                break;
            case 4:
                attron(COLOR_PAIR(4));
                mvprintw(i, j, "&");
                attron(COLOR_PAIR(1));
                break;
            case 5:
                attron(COLOR_PAIR(4));
                mvprintw(i, j, "@");
                attron(COLOR_PAIR(1));
                break;
            }
        }
        mvprintw(i, 0, "#");
        mvprintw(i, 79, "#");
    }
    for (int k = 0; k < 79; k++) {
        mvprintw(0, k, "#");
        mvprintw(20, k, "#");
    }

    std::string score = "Legnth:" + std::to_string(snakeTime);
    mvprintw(21, 1, score.c_str());

    std::string food = "Food:" + std::to_string(foodTotal);
    mvprintw(22, 1, food.c_str());

    std::string tm = "Diamonds:" + std::to_string(diamondTotal);
    mvprintw(23, 1, tm.c_str());

    std::string dh = "Demon Hearts:" + std::to_string(heartTotal);
    mvprintw(21, 30, dh.c_str());

    std::string ky = "Keys:" + std::to_string(heartTotal);
    mvprintw(22, 30, ky.c_str());

    std::string ts = "Total Score:" + std::to_string(totalScore);
    mvprintw(23, 30, ts.c_str());

    std::string mx = "c2:" + std::to_string(c2);
    mvprintw(21, 60, mx.c_str());

    std::string my = "over" + std::to_string(over);
    mvprintw(22, 60, my.c_str());

    refresh();
    return 0;
}

// generate random xy
std::pair<int, int> randXY() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> Y(1, 19);
    std::uniform_int_distribution<int> X(1, 77);
    auto y = Y(rng);
    auto x = X(rng);
    std::pair<int, int> retXY;
    retXY.first = y;
    retXY.second = x;
    return retXY;
}

int randDirection() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> D(0, 3);
    auto d = D(rng);
    return d;
}

int randFood() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> D(20, 100);
    auto d = D(rng);
    return d;
}

int randDiamond() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> D(10, 50);
    auto d = D(rng);
    return d;
}

int randHeart() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> D(1000, 2000);
    auto d = D(rng);
    return d;
}
// left:0 right:1 down:2 up:3
int move(int direction, matrix &myState, matrix &myTimeState,
         std::pair<int, int> &myPos) {
    // if food is gone, gen new food
    int gone = 1;
    for (int s = 0; s < 20; s++) {
        for (int d = 0; d < 78; d++) {
            if (state[s][d] == 2) {
                gone = 0;
            }
        }
    }

    if (gone == 1) {
        for (int t = 0; t < 4; t++) {
            int collide = 1;
            while (collide == 1) {
                std::pair<int, int> r;
                r = randXY();
                if (state[r.first][r.second] == 0) {
                    state[r.first][r.second] = 2;
                    timeState[r.first][r.second] = randFood();
                    collide = 0;
                } else {
                    collide = 1;
                }
                std::string ht = "h:" + std::to_string(collide);
                mvprintw(23, 60, ht.c_str());
                refresh();
            }
        }
    }

    // check if you are running into a wall
    if (myPos.first == 0 || myPos.first == 20) {
        return 1;
    }
    if (myPos.second == 0 || myPos.second == 79) {
        return 1;
    }

    // make a move. check if you have run into your tail or food
    switch (direction) {
    case 0:
        if (myState[myPos.first][myPos.second - 1] == 1) {
            return 1;
        } else if (myState[myPos.first][myPos.second - 1] == 5) {
            return 1;
        } else if (myState[myPos.first][myPos.second - 1] == 2) {
            totalScore += 1;
            foodTotal += 1;
            snakeTime += 1;
        } else if (myState[myPos.first][myPos.second - 1] == 3) {
            totalScore += 10;
            diamondTotal += 1;
            snakeTime += 10;
        } else if (myState[myPos.first][myPos.second - 1] == 4) {
            totalScore += 100;
            heartTotal += 1;
            snakeTime -= 20;
        }
        myState[myPos.first][myPos.second - 1] = 1;
        myTimeState[myPos.first][myPos.second - 1] = snakeTime;
        myPos.second -= 1;
        break;
    case 1:
        if (myState[myPos.first][myPos.second + 1] == 1) {
            return 1;
        } else if (myState[myPos.first][myPos.second + 1] == 5) {
            return 1;
        } else if (myState[myPos.first][myPos.second + 1] == 2) {
            totalScore += 1;
            foodTotal += 1;
            snakeTime += 1;
        } else if (myState[myPos.first][myPos.second + 1] == 3) {
            totalScore += 10;
            diamondTotal += 1;
            snakeTime += 10;
        } else if (myState[myPos.first][myPos.second + 1] == 4) {
            totalScore += 100;
            heartTotal += 1;
            snakeTime -= 20;
        }
        myState[myPos.first][myPos.second + 1] = 1;
        myTimeState[myPos.first][myPos.second + 1] = snakeTime;
        myPos.second += 1;
        break;
    case 2:
        if (myState[myPos.first + 1][myPos.second] == 1) {
            return 1;
        } else if (myState[myPos.first + 1][myPos.second] == 5) {
            return 1;
        } else if (myState[myPos.first + 1][myPos.second] == 2) {
            totalScore += 1;
            foodTotal += 1;
            snakeTime += 1;
        } else if (myState[myPos.first + 1][myPos.second] == 3) {
            totalScore += 10;
            diamondTotal += 1;
            snakeTime += 10;
        } else if (myState[myPos.first + 1][myPos.second] == 4) {
            totalScore += 100;
            heartTotal += 1;
            snakeTime -= 20;
        }
        myState[myPos.first + 1][myPos.second] = 1;
        myTimeState[myPos.first + 1][myPos.second] = snakeTime;
        myPos.first += 1;
        break;
    case 3:
        if (myState[myPos.first - 1][myPos.second] == 1) {
            return 1;
        } else if (myState[myPos.first - 1][myPos.second] == 1) {
            return 1;
        } else if (myState[myPos.first - 1][myPos.second] == 2) {
            totalScore += 1;
            foodTotal += 1;
            snakeTime += 1;
        } else if (myState[myPos.first - 1][myPos.second] == 3) {
            totalScore += 10;
            diamondTotal += 1;
            snakeTime += 10;
        } else if (myState[myPos.first - 1][myPos.second] == 4) {
            totalScore += 100;
            heartTotal += 1;
            snakeTime -= 20;
        }
        myState[myPos.first - 1][myPos.second] = 1;
        myTimeState[myPos.first - 1][myPos.second] = snakeTime;
        myPos.first -= 1;
        break;
    }
    return 0;
}

// left:0 right:1 down:2 up:3
int eMove(int direction, matrix &myState, matrix &myTimeState,
          std::pair<int, int> &myPos) {

    // check if you are running into a wall
    if (myPos.first == 0 || myPos.first == 20) {
        return 1;
    }
    if (myPos.second == 0 || myPos.second == 79) {
        return 1;
    }

    switch (direction) {
    case 0:
        if (myState[myPos.first][myPos.second - 1] == 5) {
            return 1;
        }
        if (myState[myPos.first][myPos.second - 1] == 1) {
            snakeTime -= myTimeState[myPos.first][myPos.second - 1];
        }
        myState[myPos.first][myPos.second - 1] = 5;
        myTimeState[myPos.first][myPos.second - 1] = 10;
        myPos.second -= 1;
        break;
    case 1:
        if (myState[myPos.first][myPos.second + 1] == 5) {
            return 1;
        }
        if (myState[myPos.first][myPos.second - 1] == 1) {
            snakeTime -= myTimeState[myPos.first][myPos.second + 1];
        }
        myState[myPos.first][myPos.second + 1] = 5;
        myTimeState[myPos.first][myPos.second + 1] = 10;
        myPos.second += 1;
        break;
    case 2:
        if (myState[myPos.first + 1][myPos.second] == 5) {
            return 1;
        }
        if (myState[myPos.first][myPos.second - 1] == 1) {
            snakeTime -= myTimeState[myPos.first + 1][myPos.second];
        }
        myState[myPos.first + 1][myPos.second] = 5;
        myTimeState[myPos.first + 1][myPos.second] = 10;
        myPos.first += 1;
        break;
    case 3:
        if (myState[myPos.first - 1][myPos.second] == 5) {
            return 1;
        }
        if (myState[myPos.first][myPos.second - 1] == 1) {
            snakeTime -= myTimeState[myPos.first - 1][myPos.second];
        }
        myState[myPos.first - 1][myPos.second] = 5;
        myTimeState[myPos.first - 1][myPos.second] = 10;
        myPos.first -= 1;
        break;
    }
    return 0;
}

// check if any cell times are expired.
int checkExpired(matrix &myState, matrix &myTimeState) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 79; j++) {
            if (myTimeState[i][j] == 0) {
                myState[i][j] = 0;
            } else {
                myTimeState[i][j] -= 1;
            }
        }
    }
    return 0;
}

int genDiamond(matrix &myState, matrix &myTimeState) {
    std::pair<int, int> d;
    d = randXY();
    c1 = 1;
    while (c1 == 1) {
        if (myState[d.first][d.second] == 0) {
            myState[d.first][d.second] = 3;
            myTimeState[d.first][d.second] = randFood();
            c1 = 0;
        } else {
            c1 = 1;
        }
    }
    return 0;
}

int genHeart(matrix &myState, matrix &myTimeState) {
    std::pair<int, int> d;
    d = randXY();
    c3 = 1;
    while (c3 == 1) {
        if (myState[d.first][d.second] == 0) {
            myState[d.first][d.second] = 4;
            myTimeState[d.first][d.second] = randFood() * 2;
            c3 = 0;
        } else {
            c3 = 1;
        }
    }
    return 0;
}

int genEnemy(matrix &myState, matrix &myTimeState, int &edirection,
             std::pair<int, int> &epos) {
    std::pair<int, int> d;
    d = randXY();
    int dir = randDirection();
    c2 = 1;
    while (c2 == 1) {
        if (myState[d.first][d.second] == 0) {
            switch (dir) {
            case 0:
                d.second = 78;
                break;
            case 1:
                d.second = 1;
                break;
            case 2:
                d.first = 1;
                break;
            case 3:
                d.first = 19;
                break;
            }

            myState[d.first][d.second] = 5;
            myTimeState[d.first][d.second] = 10;
            epos.first = d.first;
            epos.second = d.second;
            edirection = dir;
            c2 = 0;
        } else {
            c2 = 1;
        }
    }
    return 0;
}

