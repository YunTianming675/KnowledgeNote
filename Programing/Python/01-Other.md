# 换源

- 临时换源，清华源

    `pip install -i https://pypi.tuna.tsinghua.edu.cn/simple some-package` 

- 永久换源，清华源

    `pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple` 

- 其它源

```
# 中科大
https://pypi.mirrors.ustc.edu.cn/simple/
```

# 虚拟环境

- 安装这两个包：`pip3 install virtualenv virtualenvwrapper` 
- 建立一个文件夹保存虚拟环境默认保存的位置：`mkdir ~/.virtualenvs` 
- 最后进行如下配置

```bash
# 设置virtualenvwrapper    
export WORKON_HOME="~/.virtualenvs"
# 指定默认环境为本机自带的环境
export VIRTUALENVWRAPPER_PYTHON="/usr/bin/python"    
# 打开终端自动启用    
source /usr/local/bin/virtualenvwrapper.sh # root 执行了pip安装
source ~/.local/bin/virtualenvwrapper.sh # 一般用户
```

## Arch

> 2022-09-23 尝试在一台新的 Arch 上安装虚拟环境，结果安装配置好后怎么都无法使用？

- 安装：`pacman -S python-virtualenvwrapper` 
- 之后的配置同上，除执行脚本位置外，`source /usr/bin/virtualenvwrapper.sh` 

## 虚拟环境的使用

- 安装额外的 Python 版本，[下载地址](https://www.python.org/downloads/) ，请下载源码的 tar 压缩包或其它压缩包
- 解压编译前确认下列包已安装：`gcc`、`zlib`、`libffi` 
- 解压源码，进入所在目录，指定安装目录：`./configure --prefix=/opt/python2.7 --with-ssl` 

> 更完整的配置：`./configure --prefix=/opt/python3.9 --enable-shared --enable-optimizations --with-ssl` 

- 编译：`make` 
- 安装：`make install` 
- 在指定目录即可找到 Python 可执行文件，随后可通过使用别名或添加路径的方法在终端中使用，但这里是为虚拟环境准备的，就不再进行其它操作

> 这里是为了创建其它版本的虚拟环境而做，因为 virtualenvwrapper 创建虚拟环境需要一个现有的环境，即想要创建一个 python3.7 的虚拟环境，得首先需要有一个 python3.7 环境

- 创建：`mkvirtualenv envname --python=python_path` 
- 查看所有环境：`lsvirtualenv` 
- 激活一个环境：`workon envname` 
- 删除一个环境：`rmvirtualenv envname` 
- 更多指令可输入 `virtualenv` 获得提示

# 其它问题

Linux 下安装 pip

- `python -m ensurepip --upgrade` 

> 注意有些 Linux 发行版同时存在 Python 和 Python3，请确定为谁安装，非 root 用户或未使用 sudo 则安装的 pip 或 pip3 位于 `~/.local/bin` 目录下，请将此路径加到 `.bachrc` 或 `.zshrc` 或其它终端配置文件中