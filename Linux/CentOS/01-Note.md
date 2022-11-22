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

> 比如设置 `name="VBird Tsai"` ，然后这样判定 `[ ${name} == "VBird"]` 则会报错：`too many arguments` 。因为 `${name}` 如果没有使用双引号括起来，则上面的判定会变成 `[ VBird Tsai == "VBird" ]` 

## 12.3 shell scripts 的默认变量

- 用处：在脚本文件后面带参数
- scripts 的默认变量

```bash
./scriptsname opt1 opt2 opt3 opt4
      $0       $1   $2   $3   $4
```

- 其它的
  - `$#` ：后接的参数个数，以上面为例是 4
  - `$@` ：代表 `"$1" "$2" "$3" "$4"` 
  - `$*` ：代表 `"$1<u>c</u>$2<u>c</u>$3<u>c</u>$4"` ，这个 `<u>c</u>` 表示分隔字符，默认是空白键