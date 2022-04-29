#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
#define MAX_LENGTH 500
#define endl '\n'

int array1[MAX_LENGTH];
int array2[MAX_LENGTH];
int product[2 * MAX_LENGTH];


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

void save(int len, int *target, int position) {
    for (int i = 0; i < len; i++) target[i] = product[position + i];
    return;
}

int add(int len, int *target, int *come) {
    int carry = 0;
    for (int i = 0; i < len; i++) {
        target[i] = come[i] + come[i + len] + carry;
        carry = target[i] / 10;
        target[i] %= 10;
    }
    if (carry) {
        target[len] = carry;
        return len + 1;
    }
    return len;
}

void sub(int len, int *p, int position) {
    int carry = 0, w, ans;
    for (int i = 0; i < len; i++) {
        w = *(p + i);
        ans = product[position + i] = product[position + i] - w + carry;
        if (ans < 0)
            product[position + i] += 10, carry = -1;
        else
            carry = 0;
    }
    if (carry == -1) product[position + len] -= 1;
}

void reset(int *tmp, int len) {
    for (int i = 0; i < len; i++) {
        tmp[i] = 0;
    }
    return;
}

void mul(int length, int *part1, int *part2, int position) {
    int len = (length & 1 ? (length >> 1) + 1 : length >> 1);
    int ac_db[length + 5],
        temp[length + 5];  // (a + c) * (b + d) = ab + (ad + cb) + cd
                           //                   = ab + [(a+c)*(d+b)-ab-cd] + cd
                           // 以上須考慮位數
    if (length <= 80) {    // 短數字優化
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < length; j++) {
                if (part2[j] == 0) continue;
                product[position + i + j] += part1[i] * part2[j];
            }
        }
        for (int i = 0; i < 2 * length; i++) {
            product[position + i + 1] += product[position + i] / 10;
            product[position + i] %= 10;
        }

        return;
    }

    length += (int)(length & 1);
    mul(len, part1, part2, position);  // 長度，被乘數，乘數，儲存至product[position]
    mul(len, (part1 + len), (part2 + len), (position + length));

    save(length, temp, position);            // part1
    save(length, ac_db, (position + length));  // part2

    sub(length, temp, (position + len));
    sub(length, ac_db, (position + len));
    // end
    reset(temp, length);
    reset(ac_db, length);

    int LEN = add(len, temp, part1);
    LEN = add(len, ac_db, part2);
    mul(LEN, temp, ac_db, (position + len));

    return;
}

int main() {
    // optimization
    cin.tie(0), cout.tie(0), ios::sync_with_stdio(0);

    // output
    ofstream out("../results/N^(log2 3).txt");

    string s1, s2, ans;
    for (int digit = 10; digit <= 500; digit += 5)  // test 10 ~ 500位數
    {
        // 讀檔
        ifstream in("../data/" + to_string(digit) + ".in");  // read file
        getline(in, s1);
        getline(in, s2);

        // 資料前處理(string to int[ ] or string to char [ ] or ...)
        for (int i = 0; i < digit; i++)  // string to int array
        {
            array1[i] = s1[digit - i - 1] - '0';
            array2[i] = s2[digit - i - 1] - '0';
        }
        // end資料前處理

        // 算時間
        int testCnt;
        long long t = 0;
        for (testCnt = 0; testCnt < 200; testCnt++) {           // 200次乘法
            auto start = chrono::high_resolution_clock::now();  // start timing
            mul(digit, array1, array2, 0);  // large number multiplication
            auto end = chrono::high_resolution_clock::now();  // timing end
            tt += chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                      .count();
            reset(product, digit + digit);
        }
        t /= 200;  // average
        out << t << endl;
        // end算時間

        in.close();
    }
    return 0;
}
