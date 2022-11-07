# 01.

QMainWindow 与 QDialog 继承自 QWeight，QWeight 仅提供一个空窗口，QMainWindow 与 QDialog 则再提供了一些控件

## 01.1 First Project

### 01.1.1 main 函数

- `QApplication a(argc, argv);` : 应用程序对象，有且只有一个
- `Widget w;` : 窗口对象，widget 的父类
- `w.show();` : 必须调用 show() 方法才会显示窗口
- `a.exec();` : 进入消息循环，阻塞在此

### 01.1.2 快捷键

- 正行移动：ctrl + shift + 上箭头或下箭头
- 帮助文档：F1
- 自动对齐：ctrl + i
- 同名之间的 `.h` 和 `.cpp` 切换：F4

### 01.1.3 按钮常用 API

- 创建: 

  - 头文件: 

  ```cpp
  QPushButton *btn = nullptr;
  QPushButton *btn1 = nullptr;
  ```
  
  - 源文件: 
  
  ```cpp
  btn = new QPushButton(this);
  btn1 = new QPushButton("Second Button", this);
  
  // 析构函数里记得删除
  delete btn;
  delete btn1;
  
  btn = nullptr;
  btn1 = nullptr;
  ```
  
- 显示文本: `btn->setText("First Button");` 

- 移动按钮: `btn1->move(100, 100);` 

- 按钮重设大小: `btn1->resize(150, 50);` 

> 窗口常用 API
>
> - 重置窗口大小: `resize(600, 480);` 
> - 固定窗口大小: `setFixedSize(600, 480);` 
> - 设置窗口标题: `setWindowTitle("My First Window");` 

## 01.2 对象树

在 Qt 中创建对象时会提供一个 Parent 对象指针，下面来解释这个 parent 到底是干什么的

- QObject 是**以对象树的形式组织起来的** 

  - 当创建一个 QObject 对象时，会看到 QObject 的构造函数接收一个 QObject 指针作为参数，这个参数就是 parent，也就是父对象指针

    这相当于，**在创建 QObject 对象时，可以提供一个其父对象，我们创建的这个 QObject 对象会自动添加到其父对象的 children() 列表** 

  - **当父对象析构的时候，这个列表中的所有对象也会被析构。（注意：这里的父对象不是继承意义上的父类）** 

    这种机制在 GUI 程序设计中相当有用。例如，一个按钮有一个 QShortcut（快捷键）对象作为其子对象。当我们删除按钮时，这个快捷键理应被删除，这是合理的

- QWidget 是能够在屏幕上显示的一切组件的父类

  - **QWidget继承自 QObject，因此也继承了这种对象树关系，一个子类自动成为父组件的一个子组件。**因此，它会显示在父组件的坐标系统中，被父组件的边界剪裁。例如，当用户关闭一个对话框的时候，应用程序将其删除，那么，我们希望属于这个对话框的按钮、图标等应被一起删除

- 析构顺序：从底向上，先析构 childre() 列表中的对象

> 为什么会先执行父类的析构函数：在父类析构函数中寻找子类对象并析构，因此能看到有些父类析构函数代码先执行，但应注意在所有子类被析构完成前，父类析构函数才执行完成

- 构造时指定 parent 对象，并且大胆的在堆上创建

## 01.3 Qt 坐标系

- 以左上角为 (0, 0) ，X 向右增加，Y 向下增加
- 对于嵌套窗口，其坐标是**相对于父窗口**来说的

## 01.4 信号和槽

- 信号：控件发送的消息，比如按钮被点击后发送一个信号
- 槽：信号的接收者，处理信号
- 信号和槽的优点：松散耦合，信号发送端和接收端没有直接连接，而是通过信号连接到一起
- 连接函数：connect()
  - 参数 1：信号的发送者
  - 参数 2：发送的信号（函数的地址）
  - 参数 3：信号的接收者
  - 参数 4：处理的槽函数

## 01.5 自定义信号和槽

- 自定义信号
  - 写到 signals 下
  - 返回 void
  - 需要声明，不需要实现
  - 可以有参数，可以重载
- 自定义槽函数
  - 返回 void
  - 需要声明，需要实现
  - 可以有参数，可以重载
  - 写到 public slot 下或者 public 或者全局函数
- 触发自定义信号
  - `emit` 自定义信号

### 01.5.1 当自定义信号和槽出现重载

- 需要利用函数指针明确指向函数的地址

```c++
void (Teacher:: *teacherSignal)(QString) = &Teacher::hungry;
void (Student:: *studentSlot)(QString) = &Student::treat;
connect(zt, teacherSignal, st, studentSlot);
```

- 将 QString 转为 `char *` : `.toUtf8().data()` 

### 01.5.2 信号连接信号

```c++
// 无参信号和槽连接
void (Teacher:: *teacherSignal2)(void) = &Teacher::hungry;
void (Student:: *studentSlot2)(void) = &Student::treat;
connect(zt, teacherSignal2, st, studentSlot2);
// 信号连接信号
connect(btn, &QPushButton::clicked, zt, teacherSignal2);
```

- 断开连接：`disconnect(zt, teacherSignal2, st, studentSlot2);` 

## 01.6 拓展

- 一个信号可以连接多个槽函数
- 多个信号可以连接一个槽函数
- 信号和槽函数的参数，必须类型一一对应
- 信号和槽的参数个数，是否需要一致？
  - 信号的参数个数可以多于槽函数的参数个数
  - 但需要保证剩余参数的类型一致

### 01.6.1 Qt4 版本以前的的信号和槽连接方式

`connect(zt, SIGNAL(hungry()), st, SLOT(treat()));` 

优点：参数直观

缺点：类型不做检测（编译器不会检查这个错误，仅在运行阶段有一个命令行提示）

> Qt4 没有类型检测的原因：
>
> SIGNAL(hungry()) 中函数名被转换为字符串格式：SIGNAL("hungry")，通过字符串匹配查找函数

## 01.7 lambda 表达式

C++11 中用于**定义创建匿名的函数对象**，以简化编程工作

- 基本构成：

```
[capture](parameters)mutable->return_type
{
	statement
}
```

- 函数对象参数：

  `[]` ，表示一个 lambda 的开始，这部分必须存在，**不能省略**，函数对象参数是传递给编译器自动生成的函数对象类的构造函数。函数对象参数只能使用那些到定义 lambda 为止时 lambda 所在作用范围内可见的局部变量（包括 lambda 所在类的 this）。函数对象参数有一下几种形式：

  - 空。没有使用任何函数对象参数
  - =。函数体可以使用 lambda 所在作用范围内的所有可见的局部变量（包括 this），并且是**值传递方式**
  - &。函数体可以使用 lambda 所在作用范围内的所有可见的局部变量（包括 this），并且是**引用传递方式**
  - this。函数体可以使用所在类中的成员变量
  - a。将 a 按值传递，函数体内不能修改传递进来的 a 的拷贝，因为默认情况下函数是 const 的，**要修改传递进来的 a 的拷贝，可以使用 mutable 修饰符**
  - &a。将 a 按引用传递
  - a, &b。将 a 按值传递，b 按引用传递
  - =, &a, &b。除 a 和 b 按引用传递外，其它的按值传递
  - &, a, b。除 a 和 b 按值传递外，其它的按引用传递

- 操作符重载函数参数：

  - 标识重载的 () 操作符的参数，没有参数时，这部分可以省略。参数可以通过按值（如(a, b)）和按引用（如(&a, &b)）两种方式进行传递

- 可修改操作符

  - mutable 声明，这部分可以省略。按值传递函数对象参数时，加上 mutable 修饰符后，可以修改按值传递进来的拷贝（不是值本身）

```c++
[=]()mutable {}
```

- 函数返回值：

  `->` 返回值类型，标识函数返回值的类型，当返回值为 void，或者函数体中只有一处 return 的地方（此时编译器可以自动推断出返回值类型）时，这部分可以省略

```c++
int ret = []()->int{return 1000;}();
```



- 一个示例

```c++
[=](){
	btn->setText("aaa");
}();
```

> 最后的那个括号表示调用，否则仅仅表示定义了一个匿名函数
>
> 一般建议值传递

- 一个示例

```c++
// 利用lambda表达式实现点击按钮关闭窗口
QPushButton *btn2 = new QPushButton(this);
btn2->setText("close");
btn2->move(0, 50);
connect(btn2, &QPushButton::clicked, this, [=](){this->close();});
```

# 02.

## 02.1 QMainWindow

QMainWindow 是一个为用户提供主窗口程序的类，包含一个菜单栏（menu bar）、多个工具栏（tool bar）、多个锚接部件（dock widgets）、一个状态栏（status bar）及一个中心部件（central widget）

## 02.2 菜单栏

- 菜单栏最多只有一个
- 代码示例

```c++
// 解决菜单栏不显示的问题
// 代码模式：自己的类->setNativeMenuBar(false);
// ui模式：
ui->menubar->setNativeMenuBar(false);

// 创建菜单栏，菜单栏最多只能有一个
QMenuBar *bar = menuBar();
// 将菜单栏放入到窗口中
setMenuBar(bar);

// 但由于菜单栏是空的，所以它依然不会显示
// 创建菜单
QMenu *fileMenu = bar->addMenu("File");
QMenu *editMenu = bar->addMenu("Edit");

// 创建菜单项
QAction *newAction = fileMenu->addAction("new");
// 添加分隔线
fileMenu->addSeparator();
QAction *openAction = fileMenu->addAction("open");
```

## 02.3 工具栏

- 工具栏可以有多个
- 代码示例

```c++
// 工具栏，可以有多个
QToolBar *toolBar = new QToolBar(this);
// 将工具栏放到窗口中，默认在上方
// addToolBar(toolBar);
// 将默认位置放到左边
addToolBar(Qt::LeftToolBarArea, toolBar);
// 设置只允许左右停靠
toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
// 设置不允许浮动
toolBar->setFloatable(false);
// 设置移动（总开关），此时工具栏不允许移动
toolBar->setMovable(false);

// 在工具栏中设置内容
toolBar->addAction(newAction);
// 添加分隔线
toolBar->addSeparator();
toolBar->addAction(openAction);

// 工具栏中添加控件
QPushButton *btn = new QPushButton("aa", this);
toolBar->addWidget(btn);
```

