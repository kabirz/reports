# 编译
```shell
gcc -o echo2 echo2.c
```

# 安装到系统目录
```shell
sudo cp echo2 /usr/local/bin/
cp echo2.service echo2.socket ~/.config/systemd/user/
```

# 开启

```shell
systemctl --user daemon-reload
systemctl --user enable --now echo2.socket
systemctl --user is-active echo2.service
```

# 查看log
```shell
journalctl --user -u echo2.service
```

