#include <iostream>
using namespace std;
#define elif else if

/*ppt link: https://drive.google.com/file/d/1y2Hhf_MhPQo73JVKz37zTCM6odS6kkGm/view?usp=sharing*/

bool x0, x1, x2, x20, x21, x3, x4, x41, x42;
int dddistance, angle; //距離, 角度(sumDegree->angle(max:36))
int arr[37]{};

void preSets() // fin
{
    x0 = 1;
    x1 = x2 = x20 = x21 = x3 = x4 = x41 = x42 = 0;
    dddistance = 0;
    angle = 1;
    for (int &i : arr)
        i = 0;
}

void grafcet1() //感測距離
{
    cout << "Please input dddistance:";
    cin >> dddistance;
}

void grafcet2() //距離參數傳入 SQL
{
    // if (x2 == 1 && x20 == 1)
    arr[angle] = dddistance;
    cout << "Angle/dddistance : " << angle << "/" << arr[angle] << endl;
}

/*
void sub20(int dddistance, int angle, int arr[]) // 距離參數回傳 SQL(進入)/傳入 array
{
    arr[angle] = dddistance;
}

void sub21(int dddistance, int angle) // SQL 儲存資料
{
    // INSERT INTO table_name(dddistance_column, angle_column) VALUES (dddistance, angle);
}
*/

void grafcet3() //平面旋轉一個角度
{
    angle += 1;
}

void grafcet4() // Python 畫圖 //angle = i*10, dddistance = arr[angle]
{
    for (int i = 1; i < 11; i++) {
        cout << "angle : " << i * 10 << ", distance : " << arr[i] << endl;
    }
}

/*
void sub40() // SQL 查詢資料(進入)
{
    // SELECT * FROM TABLE;
}

int sub41() //回傳資料
{
    return dddistance;
}

void sub42() // Python 構圖
{
    call draw.py;
}
*/

void grafcet0() // fin
{
    if (x0 == 1) {
        cout << "Is opened? (y/n)";
        char c;
        cin >> c;
        if (c == 'y') {
            x0 = 0;
            x1 = 1;
        }
    }
    elif (x0 == 0 && x1 == 1)
    {
        grafcet1(); // enter dddistance by yourself
        if (dddistance >= 0 && dddistance <= 50) {
            x1 = 0;
            x2 = x20 = 1;
        }
    }
    elif (dddistance >= 0 && dddistance <= 50 && x1 == 0 && x2 == 1 && x20 == 1)
    {
        grafcet2(); // output angle and dddistance (for debugging)
        x21 = x2 = 0;
        x3 = 1;
    }
    elif (x21 == 0 && x2 == 0 && x3 == 1)
    {
        grafcet3(); // angle++
        x3 = 0;
    }
    elif (x3 == 0 && x4 == 0) //
    {
        if (angle <= 10)
            x0 = 1;
        elif (angle > 10) x4 = 1;
    }
    elif (x4 == 1)
    {
        grafcet4(); // output all the detail in array
        x42 = x4 = 0;
        x0 == 1;
        exit(0);
    }
    // cerr << "one loop" << endl;
}

int main()
{
    preSets();
    while (true) {
        grafcet0();
    }
    return 0;
}
