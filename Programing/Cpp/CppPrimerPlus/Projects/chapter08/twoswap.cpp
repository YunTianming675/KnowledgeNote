/****************************************************************************
 * @file    twoswap.cpp
 * @brief   演示函数模板显式具体化的使用
 * @date    2023-10-05
 * @details 函数模板显式具体化：为特定类型提供具体的模板定义。
 *          这么做是为了弥补模板的不足：模板内的操作对泛型 T 可能是不适用的。比如假设 T 为数组，那么当比较大小时，直接 a > b 比较的是数组的地址。
 *          函数模板显式具体化标准：
 *          - 对于给定的函数名，可以有非模板函数、模板函数和显式具体化模板函数以及它们的重载版本
 *          - 显示具体化的原型和定义应以 template<> 开头，并通过名称来指出类型
 *          - 优先级：非模板函数 > 具体化函数 > 模板函数
*****************************************************************************/
#include <iostream>

template <typename T> void Swap(T &a, T &b);

struct job {
    char name[40];
    double salary;
    int floor;
};

/* 函数模板显式具体化 */
template <> void Swap<job>(job &j1, job &j2);

/* 一般函数声明 */
void show(job &j);

int main(int argc, char *argv[])
{
    using namespace std;

    int i = 10, j = 20;

    /* 设置浮点数的显示精度为 2，即小数点后保留 2 位
     * 无参重载 cout.precision() 返回当前的显示精度 */
    cout.precision(2);
    /* 设置输出格式标志
     * 第一个参数：哪个标志位要被设置
     *  ios::fixed 表示计数法
     * 第二个参数：标志位要被设置为什么
     *  ios::floatfield 表示使用浮点表示法，对应还有科学计数法 */
    cout.setf(ios::fixed, ios::floatfield);
    cout << "i, j = " << i << ", " << j << ".\n";
    cout << "使用编译器通过函数模板生成的 Swap 函数：\n";
    Swap(i, j);
    cout << "Now i, j = " << i << ", " << j << ".\n";

    job sue = {"Susan Yaffee", 73000.60, 7};
    job sidney = {"Sidney Taffee", 78060.72, 9};

    cout << "交换前：\n";
    show(sue);
    show(sidney);
    /* 使用函数模板显式具体化的 Swap 函数 */
    Swap(sue, sidney);
    cout << "交换后：\n";
    show(sue);
    show(sidney);

    return 0;
}

template <typename T> void Swap(T &a, T &b)
{
    T t;

    t = a;
    a = b;
    b = t;
}

template <> void Swap<job>(job &j1, job &j2)
{
    double t1;
    int t2;

    t1 = j1.salary;
    j1.salary = j2.salary;
    j2.salary = t1;
    t2 = j1.floor;
    j1.floor = j2.floor;
    j2.floor = t2;
}

void show(job &j)
{
    using namespace std;

    cout << j.name << ": $" << j.salary << " on floor " << j.floor << endl;
}
