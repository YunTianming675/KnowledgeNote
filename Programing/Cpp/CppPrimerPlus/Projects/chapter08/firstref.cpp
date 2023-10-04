/****************************************************************************
 * @file    firstref.cpp
 * @brief   演示如何声明引用变量并演示引用变量和原变量执行相同的值和内存单元
 * @date    2023-10-05
*****************************************************************************/
#include "iostream"

int main(int argc, char *argv[])
{
    using namespace std;

    int rats = 101;
    int &rodents = rats;

    cout << "rats = " << rats;
    cout << ", rodents = " << rodents << endl;
    rodents++;
    cout << "rats = " << rats;
    cout << ", rodents = " << rodents << endl;

    cout << "rats address = " << &rats;
    cout << ", rodents address = " << &rodents << endl;

    return 0;
}
