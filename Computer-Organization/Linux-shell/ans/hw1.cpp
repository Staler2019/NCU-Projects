#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
    cout << "Which process do you want to do first?(p/c) ";
    char pro;
    cin >> pro;
    pid_t C_PID = fork();

    switch (C_PID)
    {
    case -1:
        perror("fork()");
        exit(-1);

    case 0:
        // cout << "Children process PID:" << getpid() << endl;
        cout << "Children process return value:" << C_PID << endl;
        break;

    default:
        if (pro == 'c' || pro == 'C')
            wait(nullptr);
        // cout << "Parent process PID:" << getpid() << endl;
        cout << "Parent process return value:" << C_PID << endl;
    }
    return 0;
}