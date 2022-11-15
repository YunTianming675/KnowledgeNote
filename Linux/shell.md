# shell 中的变量

- 查看一个变量：`echo $var` 或 `echo ${var}` 
- 修改一个变量：`var=new_content` 
- 删除一个变量：`unset $var` 
- 扩充一个变量的内容：`PATH="$PATH":/home/user/bin` 或 `PATH=$PATH:/home/user/bin` 
- 在子程序中使用父程序的一个变量：`export var` ，如：`export PATH` 
- 将指令执行的结果赋给一个变量：`var=$(command)` 

## 特殊变量：${RANDOM}

每次读取都会返回一个随机值

# 一些 shell scripts

[shell scripts](../Programing/ShellScripts/01-base.md) 