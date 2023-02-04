# 账户管理

创建一个名为 agetest 的账号，该账号第一次登陆后使用默认密码，但必须要更改密码后，使用新密码才能够登录系统使用 bash 环境

- `# useradd agetest` 

    增加名为 agetest 的用户

- `# echo "agetest" | passwd --stdin agetest` 

    修改 agetest 的密码，--stdin 指示 passwd 应该从标准输入或管道读取

- `# chage -d 0 agetest` 

    修改 agetest 的上次修改密码的时间，以 1970-01-01 开始算起。设为 0 表示上次修改密码的时间是 1970-01-01。

- `# chage -l agetest | head -n 3` 

    查看修改后的结果，需要有如下字样才表示达到了目的：

```
最近一次密码修改时间                                    ：密码必须更改
密码过期时间                                    ：密码必须更改
密码失效时间                                    ：密码必须更改
```

> agetest 第一次登陆时必须修改密码，且在修改后会被退出，可以用新密码再次登录

创建一个新群组 testgroup，且将群组交给 user01 来管理

- `# groupadd testgroup` 

    新建组

- `# gpasswd testgroup` 

    为组设置密码

- `# gpasswd -A user01 testgroup` 

    将 user01 设为管理员

- `# grep testgroup /etc/group /etc/gshadow` 

    查看是否设置成功

```
/etc/group:testgroup:x:1003:
/etc/gshadow:testgroup:$6$EUOGp/BGje$7Xb1VbRjtZxEFyib4CLAdIZBCOa0l0QL853cSFoS.d5F0EEAxo9ydUJW/O.erSgpdwhgt.tiKekYP1qTnwqDW/:user01:
```

> 一个组可以有多个组管理员
>
> 设置好后即可以通过组管理员来将其它用户加入组

- `$ gpasswd -a user01 testgroup` 

    以 user01 身份登录，先将 user01 自己加入 testgroup 组

- `gpasswd -a user02 testgroup` 

    将 user02 加入组

- `grep testgroup /etc/group` 

    查看组里有哪些用户

```
testgroup:x:1003:user01,user02
```

