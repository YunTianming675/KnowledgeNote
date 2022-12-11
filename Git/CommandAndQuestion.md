# 01. 指令

## add

添加新的文件

- 一般使用 `git add .` 添加当前目录下的所有文件

## branch



## status

## reset

- 回溯，`git reset [option] commitID` 

## commit

## log

## reset

## reflog



# 02. 问题

- windows 下中文乱码

    - 在 Gitbash 执行这条命令：`git config --global core.quotepath false` 
    - 在 `${git_home}/etc/bash.bashrc` 文件中最后加入下面两行

    ```bash
    export LANG="zh_CN.UTF-8"
    export LC_ALL="zh_CN.UTF-8"
    ```

    - `{git_home}` 是 git 的安装目录