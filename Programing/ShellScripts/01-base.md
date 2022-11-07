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