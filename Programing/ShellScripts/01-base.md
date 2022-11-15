# 第一个 shell scripts

```bash
#! /bin/bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
export PATH
echo -e "Hello World!\a\n"
exit 0
```

- 第一行：sheban，指示该文件由什么程序执行，它必须在第一行
- 第二到三行：指定环境变量，这样在后面可以不用写命令的绝对路径

> 这里指定的 PATH 都是一般系统已具有的，可以不再额外指定

- 第四行：输出一个 `Hello World` 
- 第五行：指示程序正常退出

# 读取输入

- 利用 `read` 指令读取用户输入的 firstname 和 lastname 并输出 fullname

```bash
#! /bin/bash

read -p "Please input your first name: " firstname
read -p "Please input your last name: " lastname
echo -e "\nYour full name is: ${firstname} ${lastname}"
```

# 随日期变化

- 利用 `date` 创建文件

```bash
#! /bin/bash

echo -e "I will use 'touch' command to create 3 files.\n"
read -p "Please input your filename: " fileuser

# To avoid the user randomly pressing enter
filename=${fileuser:-"filename"}

# get three diffirent date
date1=$(date --date='2 days ago' +%Y%m%d)
date2=$(date --date='1 days ago' +%Y%m%d)
date3=$(date +%Y%m%d)

file1=${filename}${date1}
file2=${filename}${date2}
file3=${filename}${date3}

touch "${file1}"
touch "${file2}"
touch "${file3}"
```

# 数值运算

- 两个数相乘

```bash
#! /bin/bash

echo -e "You should input 2 numbers, I will multiplying them.\n"
read -p "first number: " firstnu
read -p "second number: " secnu
total=$((${firstnu}*${secnu}))
echo -e "\nThe result of ${firstnu} x ${secnu} is ${total}"
```

- bash 仅支持整数，但可以通过 `bc` 来协助计算小数：`echo "123.123*55.9" | bc` 

# 计算 pi

```bash
#! /bin/bash

echo -e "This program will calculate pi value.\n"
echo -e "You should input a float number to calculate pi value.\n"
read -p "The scale number(10~10000)?" checking
num=${checking:-"10"}
echo -e "Starting calculate pi value. Be patient."
time echo "scale=${num}; 4*a(1)" | bc -lq
```

- `scale` 表示计算的精度，`4*a(1)` 是 bc 计算 pi 的函数
- `scale` 很高的时候将十分占用 CPU

# 判断文件的类型和权限

- 先判断文件名是否存在，若不存在则提示后终止
- 若存在则判断它是文件还是目录
- 判断执行者对文件或目录的权限并输出

```bash
#! /bin/bash

echo -e "Please input a filename, I will check the filename's type and permission.\n"
read -p "Input a filename: " filename
# Determines if user really entered a string
test -z ${filename} && echo "You must input a filename." && exit 0
# Check whether the file exists.If no, a message is displayed and the script exits
test ! -e ${filename} && echo "The filename '${filename}' do not exist" && exit 0
# Deteimine the file type and attributes
test -f ${filename} && filetype="regulare file"
test -d ${filename} && filetype="directory"
test -r ${filename} && perm="readable"
test -w ${filename} && perm="${perm} writable"
test -x ${filename} && perm="${perm} executable"

echo "The filename: ${filename} is a ${filetype}"
echo "And the permissions for you are: ${perm}"
```

> && 表示左边的命令执行结果为真，才执行右边的语句

# 选择是或否

```bash
#! /bin/bash

read -p "Please input (Y/N)" yn
[ "${yn}" == "Y" -o "${yn}" == "y" ] && echo "OK, continue" && exit 0
[ "${yn}" == "N" -o "${yn}" == "n" ] && echo "Oh, interrupt!" && exit 0
echo "I don't kone what your choice is" && exit 0
```

# 向 scripts 传递参数

执行后将显示：

- 程序的文件名
- 参数总数
- 参数数量小于 2 则报告数量太少
- 输出全部参数
- 输出第一个和第二个参数

```bash
#! /bin/bash

echo "The scripts name is: ${0}"
echo "Total parameter number is: $#"
[ "$#" -lt 2 ] && echo "The number of parameter is less than 2. Stop here." && exit 0
echo "Your whole parameter is: '$@'"
echo "The 1st parameter is: ${1}"
echo "The 2st parameter is: ${2}"
exit 0
```

# shift 移动变量

- shift 表示拿掉前面几个变量的意思

```bash
#! /bin/bash

echo "Total parameter number is: $#"
echo "Your whole parameter is: '$@'"
shift
echo "Total parameter number is: $#"
echo "Your whole parameter is: '$@'"
shift 3
echo "Total parameter number is: $#"
echo "Your whole parameter is: '$@'"
```

- 运行结果：

```
$ bash ./shift_paras.sh one two three four five six
Total parameter number is: 6
Your whole parameter is: 'one two three four five six'
Total parameter number is: 5
Your whole parameter is: 'two three four five six'
Total parameter number is: 2
Your whole parameter is: 'five six'
```

# 使用 if

```bash
#! /bin/bash

read -p "Please input (Y/N)" yn
if [ "${yn}" == "Y" ] || [ "${yn}" == "y" ]; then
    echo "OK, continue"
    exit 0
fi
if [ "${yn}" == "N"] || [ "${yn}" == "n" ]; then
    echo "Oh, interrupt"
    exit 0
fi
echo "I don't kone what your choice is" && exit 0
```

# 多重 if

```bash
#! /bin/bash

if [ "${1}" == "hello" ]; then
    echo "Hello, how are you"
elif [ "${1}" == "" ]; then
    echo "You must input parameters, example: '${0} hello'"
else
    echo "The only parameter is 'hello'"
fi
exit 0
```

# 分支判断

```bash
#! /bin/bash

case ${1} in
    "hello")
        echo "Hello, how are you"
        ;;  
    "") 
        echo "You must input parameters, example: '${0} hello'"
        ;;  
    *)  
        echo "Usage '${0} hello'"
        ;;  
esac

```

- `;;` ：表示程序段的结束

# 定义函数

```bash
#! /bin/bash

function printit() {
    echo -n "Your choise is: "
}

echo "This program will print your selection."
case ${1} in
    "one")
        printit
        echo ${1} | tr 'a-z' 'A-Z'
        ;;  
    "two")
        printit
        echo ${1} | tr 'a-z' 'A-Z'
        ;;  
    "three")    
        printit
        echo ${1} | tr 'a-z' 'A-Z'
        ;;  
    *)  
        echo "Usage ${0} {one;two;three}"
        ;;  
esac

```

- 注意：函数也拥有内置变量 `${0} ${1} ...` ，但这些和 scripts 的 `${0} ${1} ...` 不同。比如上面的函数调用如果使用 `printit 1` 则在函数内部 `${1}` 的值为 `1` 

# 循环

## 不定循环

- 条件成立时进行循环

```bash
while [condition]
do 
	statement...
done
```

- 条件成立时终止循环

```bash
until [condition]
do 
	statement...
done
```

直到用户输入 yes 或 YES 才结束循环

```bash
#! /bin/bash

while [ "${yn}" != "yes" -a "${yn}" != "YES" ]
do
    read -p "Please input yes/YES to top this program: " yn
done
echo "OK"
```

## 固定循环

```bash
for var in con1 con2 con3
do
	statement...
done
```

- 在工作时，var 的内容分别是 con1 con2 con3

显示所有用户的 id

```bash
#! /bin/bash

users=$(cut -d ':' -f1 /etc/passwd)
for username in ${users}
do
    id ${username}
done
```

- `for var in $(seq 1 100)` 让 var 在 1 到 100 循环，等效于 `for var in ${1..100}` 

# shell scripts Debug

sh

| 选项 | 含义                                   |
| ---- | -------------------------------------- |
| -n   | 不执行，仅检查语法，无返回结果即无错误 |
| -x   | 输出使用到的 scripts 内容              |
| -v   | 执行前，先将内容输出到屏幕上           |

