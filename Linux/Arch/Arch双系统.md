# 01. 安装

## 01.01 联网

- `iwctl` 进入联网环境（针对无线网络连接，有线连接可以跳过）

```bash
# help 查看帮助
# device list 列出无线设备
# station <device> scan 扫描当前环境下的网络
# station <device> get-networks 显示扫描到的网络
# station <device> connect <net_name> 连接指定网络
password:输入密码
#exit 退出当前环境
```

> iwctl 专用于无线网卡的连接，如果使用的是有线网络可以不适用这个

- 验证网络的连接：`ping baidu.com` 

> 使用 ssh 连接：可以复制指令了 :)
>
> - 远程主机先设置密码：`passwd` 
> - 开启：`systemctl start sshd` 
> - 之后即普通的 ssh 连接

## 01.02 修改源

- 禁用 reflector

```bash
systemctl stop reflector
systemctl disable reflector
```

> Arch 用 reflector 来筛选镜像列表，但这里不使用自动筛选，自动筛选的方法在 [Arch WiKi]([Reflector (简体中文) - ArchWiki (archlinux.org)](https://wiki.archlinux.org/title/Reflector_(简体中文)) 中
>
> 如果你的网络在中国大陆，reflector 也不会筛选出国内的源，并且筛选后 `mirrorlist` 会仅保留筛选出的源，后面还得手动添加，不如直接修改

- 修改安装源：`vim /etc/pacman.d/mirrorlist`，这里只保留在 China 下的源，例如北外（bfsu）、上海交大（sjtug）、清华（tuna）、中科大（ustc），其它的全部删除或注释掉

- vim 中批量注释的方法：

    `起始行，结束行s/^/注释符/g`

- 取消注释

    `起始行，结束行s/^注释符//g` 

> 这里提供了修改后的 mirrorlist：[mirrorlist](./OtherFiles/mirrorlist) 

## 01.03 更新时间

`timedatectl set-ntp true`

## 01.04 磁盘分区

- 需要三个分区

| 分区   | 大小     |
| ------ | -------- |
| EFI    | 至少300M |
| swap   | 一般4G   |
| 根目录 | 剩余空间 |

- 这里使用 cfdisk 工具，具有简单的图形界面
    - 注意：cfdisk 默认对 sda 进行操作，但我的硬盘是 NVME 协议的，设备名是 nvme0n1，所以这里的指令为 `cfdisk /dev/nvme0n1` 
    - 如果不确定设备名，最好用 `lsblk` 看一下，下面是我的输出结果
    
    ```bash
    NAME        MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
    sda           8:0    0 931.5G  0 disk 
    └─sda1        8:1    0 931.5G  0 part
    sdb           8:16   1  28.8G  0 disk 
    ├─sdb1        8:17   1   779M  0 part 
    └─sdb2        8:18   1    15M  0 part 
    nvme0n1     259:0    0 476.9G  0 disk 
    ├─nvme0n1p1 259:1    0   100M  0 part 
    ├─nvme0n1p2 259:2    0    16M  0 part 
    └─nvme0n1p3 259:3    0 237.3G  0 part
    ```
    
    - 其中的 sda 为存数据用的机械硬盘，sdb 为引导 U盘，固态（nvme0n1）中已有的三个分区是 windows10 建立的分区
    
- 在建立好新分区的时候记得通过 type 选项选择分区类型

    > 这里忘记保存结果了，就这样吧  

- 建立好分区后 Write->Quit，最后用 lsblk 看一下分区是否成功

- nvme0n1p5-7是新出来的分区，下面是分区后结果

```bash
NAME        MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
sda           8:0    0 931.5G  0 disk 
└─sda1        8:1    0 931.5G  0 part
sdb           8:16   1  28.8G  0 disk 
├─sdb1        8:17   1   779M  0 part 
└─sdb2        8:18   1    15M  0 part 
nvme0n1     259:0    0 476.9G  0 disk 
├─nvme0n1p1 259:1    0   100M  0 part 
├─nvme0n1p2 259:2    0    16M  0 part 
├─nvme0n1p3 259:3    0 237.3G  0 part
├─nvme0n1p4 259:4    0   500M  0 part 
├─nvme0n1p5 259:5    0     4G  0 part
└─nvme0n1p6 259:6    0   235G  0 part
```



## 01.05 格式化分区

- 格式化 EFI 分区：`mkfs.vfat /dev/nvme0n1p5`
- 格式化 SWAP 分区：`mkswap /dev/nvme0n1p6`
- 格式化根分区：`mkfs.xfs -f /dev/nvme0n1p7`

## 01.06 挂载分区

- 挂载根分区：`mount /dev/nvme0n1p7 /mnt`
- 挂载 EFI 分区：`mkdir -p /mnt/boot/efi` 、`mount /dev/nvme0n1p5 /mnt/boot/efi`
- 挂载 SWAP 分区：`swapon /dev/nvme0n1p6`
- 查看挂载分区情况：`lsblk -f` 

## 01.07 安装系统基本包

`pacstrap /mnt linux linux-firmware linux-headers base base-devel vim git bash-completion` 

## 01.08 生成文件系统表

`genfstab -U /mnt >> /mnt/etc/fstab` 

## 01.09 进入新系统

`arch-chroot /mnt` 

## 01.10 设置时区

```bash
ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
hwclock --systohc
```

## 01.11 本地化配置

- 设置系统语言：`vim /etc/locale.gen` 
- 取消注释以下两行：

```bash
en_US.UTF-8 UTF-8
zh_CN.UTF-8 UTF-8
```

- 生成本地语言信息：`locale-gen` 
- 设置本地语言环境变量：`echo "LANG=en_US.UTF-8" > /etc/locale.conf` 

## 01.12 网络配置

- 主机配置：`echo arch > /etc/hostname` 
- 生成 hosts 文件：`vim /etc/hosts` 
- 在文件末尾添加：

```bash
127.0.0.1   localhost
::1         localhost
127.0.1.1   arch.localdomain arch   # 这里的archlinux是主机名
```

## 01.13 配置 grub

- 安装相关软件包：`pacman -S grub efibootmgr efivar networkmanager intel-ucode` 
- 生成 grub 配置文件：

```bash
grub-install /dev/nvme0n1
grub-mkconfig -o /boot/grub/grub.cfg
```

## 01.14 设置网络

`systemctl enable NetworkManager` 

## 01.15 配置 root 密码

`passwd` 

## 01.16 重启到新系统

```bash
exit
umount /mnt/boot/efi
umount /mnt
reboot
```

# 02. 安装桌面环境

​		到现在，你已经将 Arch 安装好了，但是你只有 shell 来与这个系统交互，如果你需要将它作为桌面使用就需要安装一个桌面环境；如果是作为服务器使用那么可以跳过下面的**桌面安装**部分。

## 02.01 再次联网

输入 `nmtui` 选择“Activate a connection”回车进入，选择你需要的网络，连接后back返回即可

> 同样，有线网络连接一般是自动连接好的

- 启用 ssh 连接（如果你想复制指令，而不是自己写），此时你只有一个 root 账户，但默认不允许 root 远程登陆

    - 安装 ssh ：`pacman -S openssh` 

    - 启动 ssh：`systemctl start sshd` 

    - 编辑 ssh 配置文件：`vim /etc/ssh/sshd_config` 

        找到 `#PermitRootLogin prohibit-password` 这一行，在下面添加：`PermitRootLogin yes` 

    - 重启 ssh：`systemctl restart sshd` 

    - 现在你应该能通过 root 远程登陆

## 02.02 添加用户

```bash
# 添加用户
useradd --create-home dlans
# 或者
useradd -m dlans
# 设置密码
passwd dlans
```

- 这里的 dlans 是想要添加的用户名
- 设置用户组：`usermod -aG wheel,users,storage,power,lp,adm,optical dlans` 
- 设置权限：`vim /etc/sudoers` ，取消注释 `%wheel ALL=(ALL) ALL` ，在 `root ALL=(ALL) ALL` 后面添加新行：`dlans ALL=(ALL) ALL` 
- 检查 dlans 是否具备 sudo 权限：`sudo -lU dlans` 

> KDE 不允许以 root 登录，必须创建一个标准用户

## 02.03 安装需要的包（KDE）

```bash
pacman -S xorg plasma plasma-wayland-session kde-applications
# 启用
systemctl enable sddm.service
systemctl enable NetworkManager.service
```

- 重启即可

> 我使用的电脑为笔记本，Intel 核显和 Nvidia 独显的混合模式，使用 plasma-wayland-session  框架时外接显示器掉帧严重，使用 X11 还是 wayland 需要自己考虑

- 重启之后将进入 KDE 的桌面环境

这里是安装 Arch 和 gnome 桌面的文档：[安装 Arch 和 gnome]([桌面应用|一步步教你如何安装 Arch Linux](https://linux.cn/article-9170-1.html) ) 

## 02.04 添加 ArchlinuxCN 存储库

- 编辑配置文件：`sudo vim /etc/pacman.conf` 
- 修改，在最后添加：

```bash
[archlinuxcn]
Server = https://mirrors.ustc.edu.cn/archlinuxcn/$arch
```

- 更新 GPG 秘钥

```bash
sudo pacman -Syy
sudo pacman -S archlinuxcn-keyring
```

# 03. 让 grub 找到 windows10

- 安装软件包 os-prober 和 ntfs-3g
- 挂载 windows 分区：

```bash
mkdir -p /mnt/windows
mount /dev/nvme0n1p3 /mnt/windows
```

- 更新 grub：`grub-mkconfig -o /boot/grub/grub.cfg` 

- 如果出现警告：`Warning: os-prober will not be executed to detect other bootable partitions` ，编辑 `/etc/default/grub` 并取消下面这一行的注释，如果没有相应注释的话就在文件末尾添加上

    `GRUB_DISABLE_OS_PROBER=false` 。详见：[Arch WiKi]([GRUB (简体中文) - ArchWiki (archlinux.org)](https://wiki.archlinux.org/title/GRUB_(简体中文)#探测其他操作系统) 
    
- 重启后 Grub 界面会出现 Windows Boot Manager 选项

# 04. 后续处理

​		此时你已经搞定了 Arch 的基本桌面环境，但它还不够好用，比如它的界面是英文的，没有中文输入法等

- 先开启 32 位存储库

    编辑配置文件：`sudo vim /etc/pacman.conf` ，将下面内容取消注释

    ```bash
    [multilib]
    Include = /etc/pacman.d/mirrorlist
    ```

    同步索引：`pacman -Syy` 

1. 系统切换到中文，首先需要一些中文字体，否则会乱码

```bash
pacman -S noto-fonts-cjk adobe-source-han-serif-cn-fonts adobe-source-han-sans-cn-fonts ttf-arphic-uming ttf-arphic-ukai wqy-zenhei wqy-bitmapfont wqy-microhei-lite wqy-microhei
```

> `adobe-source-han-sans-jp-fonts` 这是一个日语字体

- 设置 --> 语言和区域设置 --> 语言 --> 添加语言 --> 选择简体中文 --> 将简体中文移到最上面
- 设置 --> 语言和区域设置 --> 格式 --> 选择简体中文（中国）zh_cn.UTF-8
- 重新登陆

2. 中文输入法：fcitx5

```bash
sudo pacman -S fcitx5 # 框架
sudo pacman -S fcitx5-chinese-addons # 中文输入方式
sudo pacman -S fcitx5-pinyin-zhwiki # 适用于拼音输入法的词库
sudo pacman -S fcitx5-qt # 对QT5程序的支持
sudo pacman -S fcitx5-configtool # 图形配置界面

# 修改 /etc/environment，添加这些环境变量：
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
INPUT_METHOD=fcitx
SDL_IM_MODULE=fcitx
GLFW_IM_MODULE=ibus
```

3. 用户仓库助手（AUR）：`sudo pacman -S yay` ，添加了 ArchlinuxCN 存储库的前提下

    > AUR 不唯一，你可以选择其它的 AUR，这样下面遇到 yay 替换成你所使用的 AUR 即可

4. 浏览器（如果觉得 KDE 自带的 Falkon 和 Konqueror 不好用）：

    - Microsoft Edge：`yay -S microsoft-edge-stable-bin` 
    - chromium：
        - 稳定版：`pacman -S chromium` 
        - 开发版：`pacman -S chromium-dev` 
    - Google chrome
        - 稳定版：`yay -S google-chrome` 
        - 开发版：`yay -S google-chrome-dev` 
        - 测试版：`yay -S google-chrome-beta` 
    - firefox：`pacman -S firefox` ，中文支持：`pacman -S  firefox-i18n-zh-cn` 

5. 安装 packagekit-qt5 以使软件中心可用

## 04.1 各类驱动

1. Intel 核心显卡：`pacman -S mesa lib32-mesa vulkan-intel lib32-vulkan-intel` 

> `xf86-video-intel` 众多 wiki 中表示不需要安装它，因为它不如自带的 xorg 的 modesetting 驱动稳定
>
> `intel-gpu-tools` 提供了一些 intel gpu 的工具，视需要安装
>
> ` intel-media-driver` 硬件编解码支持，[仓库地址](https://github.com/intel/media-driver) 查看受支持的架构
>
> `intel-compute-runtime` Xe 架构运行时支持

2. Nvidia 显卡：

    - 较新的型号：`pacman -S nvidia nvidia-settings lib32-nvidia-utils` 

    - GeForce 630 至 GeForce 920：

        `yay -S nvidia-470xx-dkms nvidia-settings lib32-nvidia-470xx-utils` 

    - GeForce 630 以下至 400 系列：

        `yay -S nvidia-390xx-dkms nvidia-settings lib32-nvidia-390xx-utils` 

    - 更加古老：`pacman -S mesa lib32-mesa xf86-video-nouveau` 

        > 这是开源实现

3. 混合的笔记本：`yay -S optimus-manager optimus-manager-qt` 

    - 这两个包提供核显与独显的切换

    > 我的笔记本安装后不能外接屏幕，xrandr 甚至没有列出 HDMI 接口，你需要视情况安装

4. 声卡，没声音的情况下再安装：

    `pacman -S alsa-utils alsa-ucm-conf sof-firmware` 

    重启

5. 蓝牙：

    `pacman -S bluez-utils` 

    启用蓝牙服务：

    `systemctl start bluetooth.service` 

    蓝牙服务开机自启（如果你想）：

    `systemctl enable bluetooth.service` 

    蓝牙音频支持：

    `pacman -S pulseaudio-bluetooth` 

    重启音频：

    `pulseaudio -k` 

    > 如果报错，请重启系统
    
6. 硬件编解码：[wiki](https://wiki.archlinux.org/title/Hardware_video_acceleration#Verification) 

# 05. 其他软件

## 05.1 开发

- VSCode

    这两通过 `code` 启动：

    - 微软官方：`yay -S visual-studio-code-bin` 
    - 通过官方源码编译的开源版本：`pacman -S code` 

    > KDE 下全局菜单失效：
    >
    > 安装：`pacman -S libdbusmenu-glib` 
    >
    > ZSH 出现方块：
    >
    > 设置中搜索 `terminal` ，在 `Font Family` 中添加 `'NotoMono Nerd Font'` ，请确保安装了 Nerd Font 系列字体
    
- VMWare（收费，但许可证网络上到处都是 :satisfied: ）

    - 先决条件：在 bios 中开启 CPU 虚拟化
    - 各版本：

    ```bash
    aur/vmware-workstation11 11.1.4-21 (+2 0.00) 
    aur/vmware-workstation-tech-preview 16540321-2 (+2 0.00) 
    aur/vmware-workstation15 15.5.7-9 (+3 0.01) 
    aur/vmware-workstation-openrc 16.2.4-1 (+4 0.11) 
    aur/vmware-workstation14 14.1.7-16 (+5 0.00) 
    aur/vmware-workstation12 12.5.9-23 (+9 0.00) 
    aur/vmware-workstation 16.2.4-1 (+171 2.07)
    archlinuxcn/vmware-workstation-macos 16.2.4-1 (431.9 MiB 881.4 MiB) 
    archlinuxcn/vmware-workstation 16.2.4-1 (427.2 MiB 875.3 MiB)
    ```

    - 下面这些包请确认已安装：

    ```bash
    fuse2
    gtkmm
    linux-headers
    ncurses
    libcanberra
    pcsclite
    ```

    - 可能遇到的问题：

        模块 vmmon 未启动：	`sudo modprode -a vmmon` 

        模块 vmw_vmci 未启动：`sudo modprobe -a vmw_vmci` 

        网络服务未启动/虚拟机没有网络：`sudo systemctl start vmware-networks` 

        USB 设备不能连接到虚拟机：`sudo systemctl start vmware-usbarbitrator` 

        安装失败，找不到 `gcr` 包：

        - 在 arch 的 Package [gcr](https://archlinux.org/packages/extra/x86_64/gcr/) 界面下载，然后本地安装：

            `sudo pacman -U package_name` 

- markdown

    - typora（付费）：`yay -S typora` 

    > typora 中 emoji 表情显示为一个方块：缺少 Segoe 系列字体
    >
    > 从 windows 中将这些字体全部拿出来并安装 :grin: 即可

    - remarkable（免费）：`yay -S remarkable` 

    > 预览在编辑区发生改动后总是回到最开始的部分，不会跟随编辑区移动或由用户控制，不好用

    - haroopad（免费）：`yay -S haroopad` 

    > 预览区围栏代码块语法有 bug，不好用

    - retext（免费）：`pacman -S retext` 

    > 没有上面那两个的 bug，但是十分简洁

- dbeaver

    - 一个数据库图形管理界面软件

        `sudo pacman -S dbeaver` 

    - China 中的源可能没有这个软件，可以编辑 `/etc/mirrorlist` 添加其他源进行下载


## 05.2 Office

- Libreoffice

    稳定版：`pacman -S libreoffice-still` ，中文支持：`pacman -S libreoffice-still-zh-cn` 

- WPS

    中国大陆版：`yay -S wps-office-cn` 

    国际版：`yay -S wps-office` 

    中文使用者还需安装中文包：`yay -S wps-office-mui-zh-cn` 

    可选的符号字体：`yay -S ttf-wps-fonts` 
    
- 坚果云

    `yay -S nutstore` 

