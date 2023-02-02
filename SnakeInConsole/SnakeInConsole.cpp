// SnakeInConsole.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

int width, height;
const string tailSymbols = "v<^>";
const string headSymbols = "OOUC";
const int maxspeed = 1;
bool runThread = true;
bool isDead = false;

float getRandom() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void setchar(HANDLE hConsole, char value, int x, int y) {
    COORD cursorPos;
    cursorPos.X = x;
    cursorPos.Y = y;
    SetConsoleCursorPosition(hConsole, cursorPos);
    std::cout << value;
}

void setinconsole(HANDLE hConsole, string value, int x, int y) {
    COORD cursorPos;
    cursorPos.X = x;
    cursorPos.Y = y;
    SetConsoleCursorPosition(hConsole, cursorPos);
    std::cout << value;
}

class Input {
public:
    Input(string direction) {
        this->direction = direction;
    }

    string getDirection() {
        return direction;
    }

    void setDirection(string newDirection) {
        direction = newDirection;
    }
private:
    string direction;
};

class GameObject {
private:
    int x;
    int y;
public:
    GameObject(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int getX() {
        return x;
    }

    int getY() {
        return y;
    }

    void setX(int newX) {
        x = newX;
    }

    void setY(int newY) {
        y = newY;
    }

    int* getXY() {
        int array[] = { x, y };
        return array;
    }

    void setXY(int* array) {
        x = array[0];
        y = array[1];
    }
};

class SnakePart : public GameObject{
public:
    SnakePart(int x, int y, string direction) : GameObject(x, y) {
        this->direction = direction;
    }

    string getDirection() {
        return direction;
    }

    void setDirection(string newDirection) {
        direction = newDirection;
    }
private:
    string direction;
};

class Snake {
public:
    Snake(int startX, int startY, string direction) {
        this->points.push_back(SnakePart(startX, startY, direction));
    }

    void step(string inputDirection) {
        SnakePart head = points[0];
        string direction = head.getDirection();
        int x = head.getX();
        int y = head.getY();

        if (direction == "up") {
            points.insert(points.begin(), SnakePart(x, y - 1, inputDirection));
        }
        else if (direction == "right") {
            points.insert(points.begin(), SnakePart(x + 1, y, inputDirection));
        }
        else if (direction == "down") {
            points.insert(points.begin(), SnakePart(x, y + 1, inputDirection));
        }
        else if (direction == "left") {
            points.insert(points.begin(), SnakePart(x - 1, y, inputDirection));
        }
    }

    void deleteTail() {
        if (!points.empty()){
            points.pop_back();
        }
    }

    SnakePart getHead() {
        return points[0];
    }

    vector<SnakePart> getPoints() {
        return points;
    }

    void print(HANDLE console) {
        int length = points.size();
        for (int i = 0; i < length; i++) {
            SnakePart part = points[i];
            int x = part.getX();
            int y = part.getY();
            string direction = part.getDirection();

            if (i == 0) {
                if (direction == "up") {
                    setchar(console, headSymbols[0], x, y);
                }
                else if (direction == "right") {
                    setchar(console, headSymbols[1], x, y);
                }
                else if (direction == "down") {
                    setchar(console, headSymbols[2], x, y);
                }
                else if (direction == "left") {
                    setchar(console, headSymbols[3], x, y);
                }
            }
            else if (i == length - 1) {
                if (direction == "up") {
                    setchar(console, tailSymbols[0], x, y);
                }
                else if (direction == "right") {
                    setchar(console, tailSymbols[1], x, y);
                }
                else if (direction == "down") {
                    setchar(console, tailSymbols[2], x, y);
                }
                else if (direction == "left") {
                    setchar(console, tailSymbols[3], x, y);
                }
            }
            else {
                if (direction == "down" || direction == "up") {
                     setchar(console, '|', x, y);
                }
                else if (direction == "right" || direction == "left") {
                    setchar(console, '=', x, y);
                }
                
            }
        }
    }
private:
    vector<SnakePart> points;
};

Input input = Input("right");


int main() {
    std::cout << "Your operation system is 1 - Windows, 2 - linux" << endl;
    string osystem;
    std::cin >> osystem;

    thread inputThread([] {
        while (runThread) {    
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                input.setDirection("up");
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                input.setDirection("down");
            }
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
                input.setDirection("left");
            }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
                input.setDirection("right");
            }
        }
    });

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    Snake snake = Snake((int)(width/2), (int)(height/2), "right");
    GameObject apple = GameObject((int)(getRandom() * width), (int)(getRandom() * (height - 1)));

    thread mainThread;
    int timer = 0;

    if (osystem == "1" || osystem == "windows" || osystem == "Windows" || osystem == "win" || osystem == "w") {
        while (isDead == false) {
            timer++;
            if (timer == 50) {
                snake.step(input.getDirection());

                SnakePart head = snake.getHead();
                int x = head.getX();
                int y = head.getY();

                if ((x >= width - 1) || (x < 0) || (y >= height) || (y < 0)) {
                    isDead = true;
                }
                if (x == apple.getX() && y == apple.getY()) {

                    bool a = true;
                    while (a) {
                        apple = GameObject((int)(getRandom() * width), (int)(getRandom() * (height)));
                        vector<SnakePart> points = snake.getPoints();
                        for (int i = 0; i < points.size(); i++) {
                            if (apple.getX() == points[i].getX() && apple.getY() == points[i].getY()) {
                                a = true;
                            }
                            else {
                                a = false;
                            }
                        }
                    }
                }
                else {
                    snake.deleteTail();
                }



                vector<SnakePart> points = snake.getPoints();
                for (int i = 1; i < points.size(); i++) {
                    if (head.getX() == points[i].getX() && head.getY() == points[i].getY()) {
                        isDead = true;
                    }
                }

                system("cls");
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

                setinconsole(hConsole, "score: " + to_string(snake.getPoints().size() - 1), 0, 0);

                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
                snake.print(hConsole);

                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                setchar(hConsole, '@', apple.getX(), apple.getY());
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                timer = 0;
            }
        }
    }
    else if (osystem == "2" || osystem == "linux" || osystem == "Linux" || osystem == "l") {
    }

    delete[] hConsole;
    runThread = false;
    mainThread.join();
    inputThread.join();
    std::cout << "You died!";
    string text;
    std::cin >> text;

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
