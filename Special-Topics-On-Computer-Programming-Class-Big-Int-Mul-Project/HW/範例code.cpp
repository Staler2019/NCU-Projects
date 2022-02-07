#include <iostream>
#include <chrono>
#include <string>
#include <fstream>

using namespace std;
#define MAXLENGTH 505

int arr1[MAXLENGTH];
int arr2[MAXLENGTH];
int product[2 * MAXLENGTH];

/*
    目標 : 大數乘法 N^(log2 3) 盡量在200位內超車N^2，最好的情況是能在100位內超車

    O 使用宣告好的陣列，重複使用。
    X 禁止使用malloc
    ! 宣告Array的總長度需 < 5000

    input:
    10. 15. 20. 25. ...500位數

    步驟:
    1. 拿到兩數字，做前處理
     + 開始算時間
    2. for loop 200次(不斷重複call某一個計算func，回傳ans為string)
     + 結束時間
    3. 取平均

*/

string mul(/*參數*/)
{
    string ans;
    //運算
    return ans;
}

int main()
{
    string s1, s2, ans;
    int length;
    long long t;

    for (int digit = 10; digit <= 500; digit += 5) // test 10 ~ 500位數
    {

        // 讀檔
        ifstream in(to_string(digit) + ".in"); // read file
        getline(in, s1);
        getline(in, s2);

        /*
            資料前處理(string to int[ ] or string to char [ ] or ...)
        */

        int testCnt;
        auto start = chrono::high_resolution_clock::now(); // start timing
        for (testCnt = 0; testCnt < 200; testCnt++)        //200次乘法
        {
            ans = mul(/*參數*/); // large number multiplication
        }
        auto end = chrono::high_resolution_clock::now();                                    // timing end
        t = chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / testCnt; //average
        cout << t << endl;
        
        in.close();
    }

    return 0;
}