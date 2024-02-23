# 选择交叉编译器

- arm-linux-gnueabihf-gcc: 目标应用程序运行在 Linux 操作系统之上
- arm-none-eabi-gcc: 目标程序运行环境没有操作系统

编译器命名的一般规则：

​	`arch[-os][-(gnu)eabi(hf)]-gcc`

- arch: 目标芯片架构

- os: 操作系统

- gnu: C 标准库类型

- hf: 浮点模式

    以 arm-linux-gnueabihf-gcc 为例，目标芯片架构为 ARM，目标带操作系统为 Linux，使用 GNU 的标准库 glib，使用嵌入式应用二进制接口（eabi），编译器浮点模式为硬件浮点

## 应用二进制接口

​	描述应用程序和操作系统之间或其它应用程序的低级接口，主要有 abi 和 eabi 两种类型，abi 通常用于 x86 架构上，而 eabi 表示 embed abi，即嵌入式架构，如 ARM、MIPS 等

## 浮点模式

​	部分 ARM 处理器带浮点运算单元，代码需要进行浮点运算时可以交给 FPU 处理，可以加快运算速度。编译器针对浮点运算不同的处理情况提供以下几种模式：

- hard: 采用 FPU 参与浮点运算
- soft: 即使有 FPU 也不使用
- softfp: 允许使用浮点指令，但保持与软浮点 ABI 的兼容性

## 编译器对程序的影响

​	可以使用 `readelf` 读取可执行文件的架构，也可以读取库文件的类型。最好的做法是**编译器类型同库类型匹配**。

- 当使用软浮点编译器编译**非静态**程序时，由于目标板所运行的系统是使用硬浮点编译的，所以可能**不带软浮点相关的库**，导致运行出错。

# 01. 挂载 NFS 文件系统

​	NFS（网络文件系统）是一种非常便捷的在服务器和客户端通过网络共享文件的方式。对于嵌入式 Linux 开发来说，可以直接在开发板上执行主机上的程序，访问主机上的文件，便于开发。

## 01.1 主机端配置

- 以 Ubuntu18.04 为例，Ubuntu18.04 默认没有安装 NFS 服务，使用如下命令安装：

```bash
sudo apt install nfs-kernel-server
```

- 之后在会新增一个 `/etc/exports` 文件，NFS 服务根据它的配置来运行。在最后一行添加如下内容：

```
/home/dlans/Work 192.168.1.0/24(rw,sync,all_squash,anonuid=1000,anongid=1000,no_subtree_check)
```

​	其中具体的地址配置需要根据自己的环境进行配置。

​	一些配置如下：

- `/home/dlans/Work`: 要共享的主机目录。
- `192.168.1.0/24`: 配置谁可以访问，其中 `/24` 是掩码，表示 24 个 1，结合前面的 IP 地址表示所有 IP 为 `192.168.1.*` 的主机均可以访问该目录。这个配置如果写成主机名，如 `localhost` 则只有该主机可以访问目录。
- `anonuid=1000`: 将客户机上的用户映射成本地 ID=1000 的用户

- `anongid=1000`: 将客户机上的用户映射成属于指定的本地用户组 ID
- `no_subtree_check`: 不检查子目录权限

> 配置中的 anonuid 和 anongid 把客户机的用户映射成本地 uid=1000 及 gid=1000 的用户，那么在客户端上使用不同的用户访问 NFS 共享目录时，都会具有 uid=1000 及 gid=1000 的用户的权限。在客户端的 A 用户能读写的文件，其它用户如 B 也能读写。这不是一种安全的访问设置方式，不过在开发中非常方便。

- 更新 exports 配置

```bash
sudo exportfs -arv
```

- 查看 NFS 共享情况

```bash
showmount -e
```

## 01.2 客户端配置

- 安装 NFS 客户端

```
sudo apt install nfs-common
```

- 查看 NFS 服务器共享目录

```bash
showmount -e 192.168.1.12
```

> 在不同网络下，服务器地址可能不一样

- 挂载 NFS 文件系统

```bash
sudo mount -t nfs 192.168.1.12:/home/dlans/Work /mnt/Work
```

- 取消挂载

```
sudo umount /mnt/Work
```

# 02. 文件操作

## 02.1 C 标准库

​	标准库实际是对系统调用的再次封装。例如 Windows 系统打开文件操作的 API 为 `OpenFile`，Linux 为 `open`，C 标准库都把它们封装为 `fopen`。

- fopen

    - 打开一个文件

    原型：

    ```c
    #include <stdio.h>
    FILE *fopen(const char *pathname, const char *mode);
    ```

    参数：

    - pathname: 要打开或创建的文件名
    - mode: 文件的打开方式

    | mode | 含义                                                         |
    | ---- | ------------------------------------------------------------ |
    | "r"  | 只读，文件指针位于文件开头                                   |
    | "r+" | 读和写，文件指针位于文件开头                                 |
    | "w"  | 写，将原内容清除，文件指针位于文件开头，文件不存在则创报错   |
    | "w+" | 写，将原内容清除，文件指针位于文件开头，文件不存在则创建新文件 |
    | "a"  | 追加，文件指针位于文件末尾，文件不存在则创建新文件           |
    | "a+" | 读和追加，文件指针位于文件末尾，文件不存在则创建新文件       |

    - 返回值：不为 `NULL` 时表示正常

- fread

    原型：

    ```c
    #include <stdio.h>
    size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    ```

    参数：

    - ptr: 读取到的内容将通过该指针保存
    - size: 每项的大小
    - nmemb: 读取多少项数据
    - stream: 文件流

    返回值：成功读取的项数

- fwrite

    原型：

    ```c
    #include <stdio.h>
    size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    ```

    参数：

    - ptr: 要写入文件的内容
    - size: 每项的大小
    - nmemb: 写入多少项数据
    - stream: 文件流

    返回值：成功写入的项数

- fclose

    - 关闭指定的文件流，关闭时会把尚未写入到文件的内容都写出。

    原型：

    ```c
    #include <stdio.h>
    int fclose(FILE *stream);
    ```

    参数：

    - stream: 文件流

    返回值：0 = 成功，其它 = 失败

- fflush

    原型：

    ```c
    #include <stdio.h>
    int fflush(FILE *stream);
    ```

    参数：

    - stream: 文件流

- fseek

    - 设置文件指针的位置

    原型：

    ```c
    #include <stdio.h>
    int fseek(FILE *stream, long offset, int whence);
    ```

    参数：

    - stream: 文件流
    - offset: 偏移位置
    - whence: 定义 offset 的含义

    | whence   | 含义                      |
    | -------- | ------------------------- |
    | SEEK_SET | offset 是一个绝对位置     |
    | SEEK_END | offset 以文件尾为参考点   |
    | SEEK_CUR | offset 以当前位置为参考点 |

## 02.2 系统调用

​	系统调用是一组函数及指令，是 Linux 系统提供给用户的一种操作接口。

- open

    Linux 可以使用 open 函数来打开文件

    原型：

    ```c
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    int open(const char *pathname, int flags);
    int open(const char *pathname, int flags, mode_t mode);
    ```

    参数：

    - pathname: 文件路径
    - flags: 文件的打开方式

    | flags    | 含义                         |
    | -------- | ---------------------------- |
    | O_RDONLY | 只读                         |
    | O_WRONLY | 只写                         |
    | O_RDWR   | 读写                         |
    | O_CREAT  | 创建                         |
    | O_APPEND | 追加，数据写到结尾           |
    | O_TRUNC  | 如果文件存在，则清除文件内容 |

    > O_RDONLY、O_WRONLY、O_RDWR 只能三选一

    - mode: 当 flag = O_CREAT 时，必须使用 mode 参数来设置文件与用户相关的权限。各个参数可以使用 `|` 来组合

    |            | mode    | 含义                                   |
    | ---------- | ------- | -------------------------------------- |
    | 当前用户   | S_IRUSR | 用户有读权限                           |
    |            | S_IWUSR | 用户有写权限                           |
    |            | S_IXUSR | 用户有执行权限                         |
    |            | S_IRWXU | 用户有读、写、执行权限                 |
    | 当前用户组 | S_IRGRP | 当前用户组的其他用户有读权限           |
    |            | S_IWGRP | 当前用户组的其他用户有写权限           |
    |            | S_IXGRP | 当前用户组的其他用户有执行权限         |
    |            | S_IRWXG | 当前用户组的其他用户有读、写、执行权限 |
    | 其它用户   | S_IROTH | 其它用户有读权限                       |
    |            | S_IWOTH | 其它用户有写权限                       |
    |            | S_IXOTH | 其它用户有执行权限                     |
    |            | S_IROTH | 其它用户有读、写、执行权限             |

    返回值：文件描述符

    - 与 C 标准库的对应关系

    | fopen 的参数 | open 的参数                 |
    | ------------ | --------------------------- |
    | r            | O_RDONLY                    |
    | w            | O_WRONLY\|O_CREAT\|O_TRUNC  |
    | a            | O_WRONLY\|O_CREAT\|O_APPEND |
    | r+           | O_RDWR                      |
    | w+           | O_RDWR\|O_CREAT\|O_TRUNC    |
    | a+           | O_RDWR\|O_CREAT\|O_APPEND   |

- read

    从文件中读取若干字节的数据，保存到缓冲区中，并返回实际读取到的字节数。

    原型：

    ```c
    #include <unistd.h>
    ssize_t read(int fd, void *buf, size_t count);
    ```

    参数：

    - fd: 文件描述符
    - buf: 指向数据缓冲区的指针
    - count: 读取多少个字节的数据

    返回值：读取到的字节数

- write

    往文件写入内容，返回实际写入的字节长度。

    原型：

    ```c
    #include <unistd.h>
    ssize_t write(int fd, const void *buf, size_t count);
    ```

    参数：
    
    - fd: 文件描述符
    - buf: 指向数据缓冲区的指针
    - count: 写入多少个字节的数据
    
    返回值：实际写入的字节数。
    
- close

    关闭文件。

    原型：

    ```c
    #include <unistd.h>
    int close(int fd);
    ```

    参数：

    - fd: 文件描述符

    返回值：0 = 成功，其它表示失败。

- lseek

    设置文件指针的位置，并返回文件指针相对于文件头的位置。

    原型：

    ```c
    #include <sys/types.h>
    #include <unistd.h>
    off_t lseek(int fd, off_t offset, int whence);
    ```

    参数：

    - fd: 文件描述符
    - offset: 指示文件指针的位置
    - whence: 指示 offset 的含义

    | whence   | 含义                      |
    | -------- | ------------------------- |
    | SEEK_SET | offset 是一个绝对位置     |
    | SEEK_END | offset 以文件尾为参考点   |
    | SEEK_CUR | offset 以当前位置为参考点 |

    

    # 03. GPIO 子系统
    
    ​	Linux 提供了 GPIO 子系统驱动框架，使用该驱动框架可以把 CPU 的 GPIO 引脚导出到用户空间，用户通过访问 `/sys` 文件系统进行控制，GPIO 子系统支持把引脚用于基本的输入输出功能，其中输入功能还支持中断检测。通过 GPIO 子系统可以控制 LED、蜂鸣器这类硬件设备。

## 03.1 GPIO 设备目录

​	GPIO 驱动子系统导出到用户空间的目录是 `/sys/class/gpio` 

在 GPIO 导出前目录内容：

```bash
--w--w---- 1 root gpio 4.0K Sep 12 23:28 export
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip0 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip0
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip112 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip112
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip128 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip128
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip16 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip16
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip32 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip32
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip400 -> ../../devices/platform/soc/soc:pin-controller-z@54004000/gpio/gpiochip400
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip48 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip48
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip64 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip64
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip80 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip80
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip96 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip96
--w--w---- 1 root gpio 4.0K Sep 12 23:28 unexport
```

执行 GPIO 导入指令，将 45 号 IO 导入用户空间：

```bash
echo 45 > /sys/class/gpio/export
```

导入后目录内容：

```bash
--w--w---- 1 root gpio 4.0K Sep 12 23:33 export
lrwxrwxrwx 1 root root    0 Sep 12 23:33 gpio45 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpiochip2/gpio/gpio45
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip0 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip0
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip112 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip112
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip128 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip128
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip16 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip16
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip32 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip32
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip400 -> ../../devices/platform/soc/soc:pin-controller-z@54004000/gpio/gpiochip400
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip48 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip48
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip64 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip64
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip80 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip80
lrwxrwxrwx 1 root gpio    0 Sep 12 23:28 gpiochip96 -> ../../devices/platform/soc/soc:pin-controller@50002000/gpio/gpiochip96
--w--w---- 1 root gpio 4.0K Sep 12 23:28 unexport
```

如何取消导出：

```bash
echo 45 > /sys/class/gpio/unexport
```

两个主要文件：

- `/sys/class/gpio/export`: 导出 GPIO，该文件只能写不能读，向该文件写入 GPIO 的编号 N 可以向内核申请将编号为 N 的 GPIO 导出到用户空间，若内核本身没有把该 GPIO 用于其它功能，那么在 `/sys/class/gpio` 目录下会新增一个对应编号的 gpioN 目录。
- `/sys/class/gpio/unexport`: 取消导出 GPIO，只能写不能读。

## 03.2 GPIO 设备属性

可以通过 `ls` 命令查看：

```bash
ls -lh /sys/class/gpio/gpio45/
```

输出如下：

```bash
total 0
-rw-rw---- 1 root gpio 4.0K Sep 12 23:44 active_low
lrwxrwxrwx 1 root root    0 Sep 12 23:46 device -> ../../../gpiochip2
-rw-rw---- 1 root gpio 4.0K Sep 12 23:44 direction
-rw-rw---- 1 root gpio 4.0K Sep 12 23:44 edge
drwxr-xr-x 2 root root    0 Sep 12 23:46 power
lrwxrwxrwx 1 root gpio    0 Sep 12 23:44 subsystem -> ../../../../../../../class/gpio
-rw-r--r-- 1 root root 4.0K Sep 12 23:44 uevent
-rw-rw---- 1 root gpio 4.0K Sep 12 23:44 value
```

常用属性文件如下：

- direction: 表示 GPIO 引脚的方向，可取值如下
    - in: 引脚为输入模式。
    - out: 引脚输出模式，且默认输出电平为低。
    - low: 输出模式，输出电平为低。
    - high: 输出模式，输出电平为高。
- value: 表示 GPIO 的电平，1 表示高电平，0 表示低电平。如果 GPIO 被设置为输出模式，那么修改该文件的内容可以改变引脚的电平。
- edge: 配置 GPIO 的中断触发方式，当 GPIO 被配置为中断时，可以通过系统函数 `poll` 监听。edge 可取如下值：
    - none: 不使用中断
    - rising: 中断输入，上升沿触发
    - falling: 中断输入，下降沿触发
    - both: 中断输入，边沿触发

# 04. 输入子系统

​	输入子系统是 Linux 对输入设备提供的统一驱动框架。如按键、键盘、鼠标、触摸屏等输入设备的驱动方式是一样的，当出现按键、触摸操作时，硬件产生中断，然后 CPU 直接读取引脚电平，或通过 IIC、SPI 等通讯方式从设备的寄存器读取具体的按键值或触摸坐标，然后把这些信息提交给内核。使用输入子系统驱动的设备可以通过统一的数据结构提交给内核，该数据结构包括输入的时间、类型、代号以及具体的键值或坐标，内核通过 `/dev/input` 目录下的文件接口传递给用户空间。

## 04.1 input 事件目录

### evtest 工具

​	evtest 是一个用于测试输入设备的工具。它会列出系统当前可用的 `/dev/input/` 下的事件文件，并列出这些事件对应的设备名。

```bash
# 安装
sudo apt install evtest

# 运行
sudo evtest
```

### input 事件结构

​	evtest 通过读取 `/dev/input` 目录下文件的内容获取输入设备上报的信息，这些文件是驱动层设备暴露到用户空间的访问接口文件。该目录内容如下：

```bash
dlans@dlans-vm:/dev/input
$ ll
总用量 0
drwxr-xr-x 2 root root      80 9月  16 10:40 by-id/
drwxr-xr-x 2 root root     140 9月  16 10:40 by-path/
crw-rw---- 1 root input 13, 64 9月  16 10:40 event0
crw-rw---- 1 root input 13, 65 9月  16 10:40 event1
crw-rw---- 1 root input 13, 66 9月  16 10:40 event2
crw-rw---- 1 root input 13, 67 9月  16 10:40 event3
crw-rw---- 1 root input 13, 68 9月  16 10:40 event4
crw-rw---- 1 root input 13, 63 9月  16 10:40 mice
crw-rw---- 1 root input 13, 32 9月  16 10:40 mouse0
crw-rw---- 1 root input 13, 33 9月  16 10:40 mouse1
crw-rw---- 1 root input 13, 34 9月  16 10:40 mouse2
```

- input_event 结构体定义

    ```c
    struct input_event {
    #if (__BITS_PER_LONG != 32 || !defined(__USE_TIME_BITS64)) && !defined(__KERNEL__)
    	struct timeval time;
    #define input_event_sec time.tv_sec
    #define input_event_usec time.tv_usec
    #else
    	__kernel_ulong_t __sec;
    #if defined(__sparc__) && defined(__arch64__)
    	unsigned int __usec;
    #else
    	__kernel_ulong_t __usec;
    #endif
    #define input_event_sec  __sec
    #define input_event_usec __usec
    #endif
    	__u16 type;
    	__u16 code;
    	__s32 value;
    };
    ```

    - time: 记录事件产生的时间戳
    - type: 输入设备的事件类型。系统常用的默认类型有 `EV_KEY`、`EV_REL`、`EV_ABS`，分别用于表示按键状态改变事件、相对坐标改变事件及绝对坐标改变事件。`EV_SYN` 用于分割事件，无特别意义。
    - code: 事件代号，以更精确的方式表示事件。例如在 `EV_KEY` 事件类型中，code 的 值常用于表示键盘上具体的按键，其取值范围在 0~127 之间，例如按键 Q 对应的是 KEY_Q，枚举值为 16
    - value: 事件的值。对于 EV_KEY 事件类型，当按键按下时，该值为 1；按键松开时，该值为 0。对于鼠标，evtest 输出的内容里，ABS_X 事件类型中的 value 值表示 X 坐标，ABS_Y 类型中的 value 值表示 Y 坐标。

### input 事件设备名

​	`/dev/input/event*` 的事件编号与设备的联系不是固定的，它通常按系统检测到设备的先后顺序安排 event 文件的编号。可以通过 `/dev/input/by-id` 或 `/dev/input/by-path` 目录查看具体的硬件设备。例如：

```bash
dlans@dlans-vm:/dev/input/by-path
$ ll
总用量 0
lrwxrwxrwx 1 root root 9 9月  16 10:40 pci-0000:02:00.0-usb-0:1:1.0-event-mouse -> ../event4
lrwxrwxrwx 1 root root 9 9月  16 10:40 pci-0000:02:00.0-usb-0:1:1.0-mouse -> ../mouse2
lrwxrwxrwx 1 root root 9 9月  16 10:40 platform-i8042-serio-0-event-kbd -> ../event1
lrwxrwxrwx 1 root root 9 9月  16 10:40 platform-i8042-serio-1-event-mouse -> ../event3
lrwxrwxrwx 1 root root 9 9月  16 10:40 platform-i8042-serio-1-mouse -> ../mouse0
```

​	因为 `/dev` 下的设备都是通过 `/sys` 导出的，所以也可以通过 `/sys/class/input` 目录查看，例如：

```bash
dlans@dlans-vm:/sys/class/input
$ ll
总用量 0
lrwxrwxrwx 1 root root 0 9月  16 10:40 event0 -> ../../devices/LNXSYSTM:00/LNXPWRBN:00/input/input0/event0/
lrwxrwxrwx 1 root root 0 9月  16 10:40 event1 -> ../../devices/platform/i8042/serio0/input/input1/event1/
lrwxrwxrwx 1 root root 0 9月  16 10:40 event2 -> ../../devices/platform/i8042/serio1/input/input4/event2/
lrwxrwxrwx 1 root root 0 9月  16 10:40 event3 -> ../../devices/platform/i8042/serio1/input/input3/event3/
lrwxrwxrwx 1 root root 0 9月  16 10:40 event4 -> ../../devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-1/2-1:1.0/0003:0E0F:0003.0001/input/input5/event4/
lrwxrwxrwx 1 root root 0 9月  16 10:40 input0 -> ../../devices/LNXSYSTM:00/LNXPWRBN:00/input/input0/
lrwxrwxrwx 1 root root 0 9月  16 10:40 input1 -> ../../devices/platform/i8042/serio0/input/input1/
lrwxrwxrwx 1 root root 0 9月  16 10:40 input3 -> ../../devices/platform/i8042/serio1/input/input3/
lrwxrwxrwx 1 root root 0 9月  16 10:40 input4 -> ../../devices/platform/i8042/serio1/input/input4/
lrwxrwxrwx 1 root root 0 9月  16 10:40 input5 -> ../../devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-1/2-1:1.0/0003:0E0F:0003.0001/input/input5/
lrwxrwxrwx 1 root root 0 9月  16 10:40 mice -> ../../devices/virtual/input/mice/
lrwxrwxrwx 1 root root 0 9月  16 10:40 mouse0 -> ../../devices/platform/i8042/serio1/input/input4/mouse0/
lrwxrwxrwx 1 root root 0 9月  16 10:40 mouse1 -> ../../devices/platform/i8042/serio1/input/input3/mouse1/
lrwxrwxrwx 1 root root 0 9月  16 10:40 mouse2 -> ../../devices/pci0000:00/0000:00:11.0/0000:02:00.0/usb2/2-1/2-1:1.0/0003:0E0F:0003.0001/input/input5/mouse2/
```

# 05. 进程

## 05.1 查看进程

​	Linux 中每个进程都会被分配一个唯一的数字编号（ProcessID，PID）。PID 是一个 16 位的正整数，默认范围从 2-32768，在新进程启动时自动依次分配。当 PID 达到最大时，系统重新选择下一个未使用的数值。1 一般为特殊进程 init 保留。

### 父进程 ID

​	任何进程（除 init 进程）都是由另一个进程启动，该进程称为被启动进程的父进程，被启动的进程称为子进程，父进程号无法在用户层修改。用户可以通过调用 `getppid()` 函数来获得当前进程的父进程号。

​	为了直观展现系统中的父进程和子进程，可以使用 `pstree` 命令将进程以树状关系列出。如果提示没有该指令，使用 `apt install psmisc` 安装。

## 05.2 程序和进程

### 进程的概念

​	进程是程序执行的具体实例，比如一个可执行文件，在执行的时候，它就是一个进程，直到该进程执行完毕。在执行的过程中，它享有系统的资源，同样的一个程序，可以实例化为多个进程。程序不能单独执行，只有将程序加载到内存中，系统为它分配资源后才能够执行，这种执行的程序称为进程。也就是说，**进程是系统进行资源分配和调度的一个独立单位**，每个进程都有自己单独的地址空间。

### 程序变成进程

​	程序是静态的文件，而进程是动态的实体，其中的转换主要包含以下三个步骤：

1. 查找命令对应程序文件的位置
2. 使用 `fork()` 函数启动一个新的进程
3. 在新进程中使用 exec 族函数装载程序文件，并执行程序文件中的 main 函数

## 05.3 程序的状态

​	这里的状态指 `ps -ux` 中 STAT 列的信息

| 状态 | 含义                                                         |
| ---- | ------------------------------------------------------------ |
| R    | 可运行状态，表示进程在运行队列中，处于正在执行或即将执行的状态，只有在该状态下的进程才可能在 CPU 上运行。同一时刻可能有多个进程处于该状态 |
| S    | 可中断的睡眠状态，处于这个状态的进程因为等待某种事件的发生而被挂起，比如进程在等待信号 |
| D    | 不可中断的睡眠状态，通常是在等待输入或输出完成，处于这种状态的进程不能响应异步信号 |
| T    | 停止状态，通常是被 shell 的工作信号控制，或因为它被追踪，进程正处于调试器的控制之下 |
| Z    | 退出状态。进程成为僵尸进程                                   |
| X    | 推出状态。进程即将被回收                                     |
| s    | 进程是会话其首进程                                           |
| l    | 进程是多线程的                                               |
| +    | 进程属于前台进程组                                           |
| <    | 高优先级任务                                                 |

## 05.4 进程状态转换

![](./pic/01-BaseAndAppDevelop/Snipaste_2023-09-16_21-36-59.png)

1. 进程由父进程调用 `fork()` 开始，init 作为系统的第一个进程，所有进程都是 init 的子进程
2. 当进程收到 SIGSTOP 或者 SIGTSTP 中的其中一个时，进程状态会被置为暂停态，该状态下的进程不再参与调度，但系统资源不会释放，直到收到 SIGCONT 信号后被重新置为就绪态
3. 进程在完成任务后或遭遇异常会退出。在 main 函数内返回或调用 `exit()` 函数，或者线程调用 `pthread_exit()` 都是属于正常退出。进程收到 kill 信号会异常推出。不管怎么退出，**系统都会调用 `do_exit()` 函数来使进程进入僵尸态**，僵尸指将进程的进程控制块（Process Control Block，PCB）留下。留下该信息便于系统及用户知道进程结束的原因，进程返回的值。
4. 对僵尸进程的处理。父进程将僵尸进程状态设为 `EXIT_DEAD`，即退出态，然后回收其系统资源。
5. 如果一个子进程的父进程先结束的处理过程。子进程会被它的祖先进程收养，当子进程退出时，祖先进程回收子进程占用的资源。

## 05.5 启动新进程

### system()

​	该函数为 C 标准库提供，在 Linux 中，实际是启动一个 `/bin/sh` 进程来运行函数参数内容。

- 这种方法会阻塞，必须等该函数执行完代码才会继续向下执行。
- 可以不阻塞执行，方法是在函数参数内容加一个 `&`，实际是指 sh 在后台执行

```
system("ls -l &")
```

### fork()

​	fork() 启动一个新进程，在父进程中它返回子进程的 PID，在父进程中，它返回 0。

- fork() 本质上将父进程复制了一份，但仍有一些区别

    相同的地方

    - 进程的地址空间
    - 进程上下文、代码段
    - 进程堆空间、栈空间、内存信息
    - 进程的环境变量
    - 标准 IO 的缓冲区
    - 打开的文件描述符
    - 信号响应函数
    - 当前工作路径

    不同的地方

    - 进程号 ID
    - 记录锁，父进程对文件加的锁，子进程不会继承
    - 挂起的信号，这是已响应但未处理的信号，子进程不会继承这些信号

### execl()

​	替换进程的执行程序，将原进程的数据段、代码段和堆栈段替换掉

原型：`int execl(const char *pathname, const char *arg, ...)`

- 执行 pathname 所代表的文件路径，接下来的可变参数表示传递的参数，最后一个参数必须以 NULL 作为结束标志

exec 族还有其它函数

```c
int execl(const char *pathname, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *pathname, const char *arg, ...);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
```



可以通过后缀区分它们的作用：

- 包含 l：接受参数列表作为调用程序的参数
- 包含 p：接受程序名作为参数，在当前执行路径和 PATH 中搜索，不包含 p 的函数调用时必须指定绝对路径
- 包含 v：子程序参数通过数组装载
- 包含 e：比其它函数多接收一个指明环境变量列表的参数，通过参数 envp 传递字符串作为新程序的环境变量，每个字符串应表示为 `enviroment = viraables` 的形式
