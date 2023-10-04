/****************************************************************************
 * @file    sceref.cpp
 * @brief   演示不能通过赋值来设置引用，这是无效的
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
    cout << "rats address = " << &rats;
    cout << ", rodents address = " << &rodents << endl;

    int bunnies = 50;
    rodents = bunnies;

    cout << "bunnies = " << bunnies;
    cout << ", rats = " << rats;
    cout << ", rodents = " << rodents << endl;

    cout << "bunnies address = " << &bunnies;
    cout << ", rodents address = " << &rodents << endl;

    return 0;
}
