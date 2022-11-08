# 12.

## 12.1 shell scripts 执行方式差异

`bash xxx.sh` 或 `sh xxx.sh` 或 `./xxx.sh` 

- 相当于使用子程序 bash 或 sh 去执行，子程序内的各种变量不会回传到父程序中

`source xxx.sh` 

- 不使用子程序方式，scripts 里的各种变量将出现在当前 shell 中

## 12.2 bash scripts 的判断式

`test` 指令

`[]` 

- 中括号用于判断时，两端都要有空白字符，如判断 `HOME` 这个变量是否为空：

  `[ -z "${HOME}" ] ; echo $?` 

- 在中括号内的变量或常数，最好以单引号或双引号括起来