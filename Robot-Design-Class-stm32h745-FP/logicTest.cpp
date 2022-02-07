#include <bits/stdc++.h>
using namespace std;

int x0, start, mv, action, getBall, throwInit, throwBall;

char command;

void init()
{
    x0 = 1;
    start = 0;
    mv = 0;
    action = 0;
    getBall = 0;
    throwInit = 0;
    throwBall = 0;
}

void grafcet()
{
    if (x0 == 1) { // 0
        cout << "start or not(y/n): ";
        char c;
        cin >> c;
        if (c == 'y') {
            cout << "program started" << endl;
            start = 1, x0 = 0;
        }
    }
    else if (start == 1) { // 1
        cout << "夾子移動成功" << endl;
        mv = 1, start = 0;
    }
    else if (mv == 1) { // 2
        cout << "接收從電腦指令: ";
        cin >> command;
        cout << "接收電腦指令成功" << endl;
        action = 1, mv = 0;
    }
    else if (action == 1) { // 3
        switch (command) {
        case '.': // space
            cout << "detect space pressed" << endl;
            getBall = 1, action = 0;
            break;
        case 'w':
            cout << "complete forward" << endl;
            mv = 1, action = 0;
            break;
        case 's':
            cout << "complete backward" << endl;
            mv = 1, action = 0;
            break;
        case 'a':
            cout << "complete left" << endl;
            mv = 1, action = 0;
            break;
        case 'd':
            cout << "complete right" << endl;
            mv = 1, action = 0;
            break;
        default:
            cout << "nothing done" << endl;
            mv = 1, action = 0;
            break;
        }
    }
    else if (getBall == 1) { // 4
        cout << "完成take up the ball" << endl;
        throwInit = 1, getBall = 0;
    }
    else if (throwInit == 1) { // 5
        cout << "完成move the ball to 投球預備位置" << endl;
        throwBall = 1, throwInit = 0;
    }
    else if (throwBall == 1) { // 6
        cout << "完成throw the ball" << endl;
        x0 = 1, throwBall = 0;
    }
}

int main()
{
    init();
    while (1) {
        grafcet();
    }
}