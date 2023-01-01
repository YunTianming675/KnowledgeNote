# Makefile 小试验

示例：

```makefile
# targeta 依赖于 targetc 和 targetb
targeta: targetc targetb
	ls -lh

targetb:
	touch test.txt

targetc:
	pwd

# targetb 不被任何目标依赖，所以默认不会执行
# 可以通过 make targetb 来执行
targetd:
	rm -f test.txt
```

执行结果：

```
pwd
/mnt/d/Projects/VSCode/C/example/mkfile_step/step0
touch test.txt
ls -lh
total 0
-rwxrwxrwx 1 dlans dlans 100 Dec 29 16:19 Makefile
-rwxrwxrwx 1 dlans dlans   0 Dec 31 13:33 test.txt
```

注意：

- 命令前必须是 Tab，不能替换成空格
- 第一个目标，是 make 的最终目标及默认目标

# 使用 Makefile 编译程序

三个文件：

- hello_func.c

```c
#include "stdio.h"
#include "hello_func.h"

void hello_func(void)
{
	printf("hello, world! This is a C program.\n");
	for (int i = 0; i < 10; i++) {
		printf("output i = %d\n", i);
	}
}
```

- hello_func.h

```c
#ifndef __HELLO_FUNC_H
#define __HELLO_FUNC_H

void hello_func(void);

#endif
```

- hello_main.c

```c
#include "hello_func.h"

int main()
{
	hello_func();
	return 0;
}
```

Makefile:

```makefile
hello_main: hello_main.c hello_func.c
	gcc -o hello_main hello_main.c hello_func.c -I .

clean:
	rm -f *.o hello_main
```

- hello_main 为默认目标，它依赖于 hello_main.c 和 hello_func.c
- make 的目标名和 gcc 生成的文件名一样表示目标 hello_main 已经是一个目标文件 hello_main。这样的好处是 make 执行时会检查 hello_main 和依赖文件的日期，如果依赖文件的修改日期比 hello_main 新，则执行，否则不执行

# 目标与依赖

语法：

```
目标1: 依赖
	命令1
	命令2
目标2: 依赖
	命令1
	命令2
```

- 目标：make 要做的事，需要定格开始书写，前面不能有空白字符。可以有多个目标，第一个目标被当作默认目标
- 依赖：要达成目标需要依赖的文件或其它目标
- 命令：达成目标所需要执行的命令

## 伪目标

一个目标什么时候会被执行：

- 目标在 make 看来，都是目标文件，make 在执行后希望得到与目标同名的文件。所以在没有与目标同名的文件或者依赖文件比与目标同名的文件新时，才会执行目标
- 当存在与目标同名的文件且是最新的时，目标不会被执行，但有时这会引起误会，所以需要将一个目标标记为**伪目标，伪目标不期待生成目标文件** 
- 伪目标关键字：`.PHONY` 
- 示例：

```makefile
hello_main: hello_main.o hello_func.o
	gcc -o hello_main hello_main.o hello_func.o

.PHONY:clean
clean:
	rm -f *.o hello_main
```

- 这里的 clean 目标即为伪目标

# 变量

定义变量的方式有以下四种：

| 方式 | 含义                                             |
| ---- | ------------------------------------------------ |
| =    | 延时赋值，该变量只有在调用时，才会被赋值         |
| :=   | 直接赋值，与延时赋值相反，变量的值在定义时就确定 |
| ?=   | 若变量的值为空，则进行赋值，通常用于设置默认值   |
| +=   | 追加赋值，往变量后面增加新的内容                 |

- 使用一个变量：`$(变量名)` 

## 自动化变量

| 符号 | 含义                                           |
| ---- | ---------------------------------------------- |
| $@   | 匹配目标文件                                   |
| $%   | 与 $@ 类似，但 $% 仅匹配库类型的目标文件       |
| $<   | 依赖中的第一个目标文件                         |
| $^   | 所有的依赖目标，如果依赖中有重复的，只保留一份 |
| $+   | 所有的依赖目标，即使依赖中有重复的也原样保留   |
| $?   | 所有比目标要新的依赖目标                       |



## 通过变量修改默认规则

```makefile
# 定义变量
CC = gcc
CFLAGS = -I .
DEPS = hello_func.h

# 目标文件
hello_main: hello_main.o hello_func.o
	$(CC) -o hello_main hello_main.o hello_func.o

# *.o 文件的生成规则
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# 伪目标
.PHONY: clean
clean:
	rm -f *.o hello_main
```

- 11 行表示所有的 `.o` 文件除了依赖默认的 `.c` 文件外还依赖 `$(DEPS)` 代表的头文件。`%` 表示通配符，`%.o` 将匹配所有的 `.o` 文件
- 12 行在 `%` 匹配到 `hello_func` 时将等价于：`gcc -c -o hello_func.o hello_func.c -I .` 

> 最终生成的可执行文件本质上依赖的是 `.o` 文件而非 `.c` 文件

## 通过变量修改链接规则

```makefile
# 定义变量
TARGET = hello_main
CC = gcc
CFLAGES = -I .
DEPS = hello_func.h
OBJS = hello_main.o hello_func.o

# 目标文件
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGES)

# *.o 文件的生成规则
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGES)

# 伪目标
.PHONY: clean
clean:
	rm -rf *.o hello_main
```

# 分支

语法：

```makefile
ifeq(arg1, arg2)
分支 1
else
分支 2
endif
```

- arg1 和 arg2 相同时执行分支 1 的内容，否则执行分支 2 内容。arg1、arg2 可以是变量或者常量

示例：

```makefile
# 定义变量
# ARCH 默认为 x86，使用 gcc 编译器，否则使用 arm 编译器
ARCH ?= x86
TARGET = hello_main
CC = gcc
CFLAGES = -I .
DEPS = hello_func.h
OBJS = hello_main.o hello_func.o

# 根据 ARCH 选择编译器
ifeq ($(ARCH), x86)
CC = gcc
else
CC = arm-linux-gnueabihf-gcc
endif

# 目标文件
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGES)

# *.o 文件的生成规则
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGES)

# 伪目标
.PHONY: clean
clean:
	rm -rf *.o hello_main
```

- 使用 ARM 平台：`make ARCH=arm` 
- 使用 x86 平台：`make` 

# 函数

- 调用函数：`$(函数名 参数)` 或 `${函数名 参数}` 

## 通过函数支持多级文件结构

```makefile
# 定义变量
# ARCH 默认为 x86，使用 gcc 编译器，否则使用 arm 编译器
ARCH ?= x86
TARGET = hello_main

# 存放中间文件的路径
BUILD_DIR = build_$(ARCH)
# 存放源文件的文件夹
SRC_DIR = sources
# 存放头文件的文件夹
INC_DIR = includes .
# 源文件
SRCS = ${wildcard $(SRC_DIR)/*.c}
# 目标文件
OBJS = ${patsubst %.c, $(BUILD_DIR)/%.o, ${notdir $(SRCS)}}
# 头文件
DEPS = ${wildcard $(INC_DIR)/*.h}

# 指定头文件的路径
CFLAGS = ${patsubst %, -I%, $(INC_DIR)}

# 根据 ARCH 变量来选择编译器
ifeq ($(ARCH), x86)
	CC = gcc
else
	CC = arm-linux-gnueabihf-gcc
endif

# 目标文件
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

# *.o 文件的生成规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
# 创建一个编译目录，存放过程文件
# 命令前带 @，表示不在终端上输出
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# 伪目标
.PHONY: clean cleanall
# 按架构删除
clean:
	rm -rf $(BUILD_DIR)

# 全部删除
cleanall:
	rm -rf build_x86 build_arm
```



# 附 A 函数

## notdir

- 去除文件路径中的目录部分
- 格式：`${notdir 文件名}` 
- 示例：`${notdir ./sources/hello_func.c}` ，输出：`hello_func.c` 

## wildcard

- 获取文件列表，用空格分隔开
- 格式：`${wildcard 匹配规则}` 
- 示例：`${wildcard sources/*.c}` ，将 sources 文件夹下所有的 `.c` 文件列出

## patsubst

- 模式字符串替换
- 格式：`${patsubst 匹配规则，替换规则，输入的字符串}` 
- 示例：`${patsubst %.c, build_dir/%.o, hello_main.c}` ，输出：`build_dir/hello_main.o` 