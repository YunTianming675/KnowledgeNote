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

