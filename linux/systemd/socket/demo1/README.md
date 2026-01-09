# 安装依赖（Ubuntu/Debian）
```shell
  sudo apt install libsystemd-dev gcc
```

# 编译
```shell
gcc -o echo-activated echo-activated.c -lsystemd
```

# 安装到系统目录
```shell
sudo cp echo-activated /usr/local/bin/
cp echo-activated.service echo-activated.socket ~/.config/systemd/user/
```

# 开启

```shell
systemctl --user daemon-reload
systemctl --user enable --now echo-activated.socket
systemctl --user is-active echo-activated.service
```

# 查看log
```shell
journalctl --user -u echo-activated.service
```


