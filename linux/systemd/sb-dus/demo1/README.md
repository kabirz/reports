# sd-bus 示例

## ubuntu 依赖安装
```shell
  sudo apt install libsystemd-dev
```

## 编译
```shell
gcc main.c -o main -lsystemd
```

## 开启任务
```shell
./main
```

## 查看调用接口
1. 使用busctl
```shell
busctl --user introspect com.example.Calculator /com/example/Calculator
```
Log:
```log
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
com.example.Calculator              interface -         -            -
.Add                                method    xx        x            -
.Echo                               method    s         s            -
org.freedesktop.DBus.Introspectable interface -         -            -
.Introspect                         method    -         s            -
org.freedesktop.DBus.Peer           interface -         -            -
.GetMachineId                       method    -         s            -
.Ping                               method    -         -            -
org.freedesktop.DBus.Properties     interface -         -            -
.Get                                method    ss        v            -
.GetAll                             method    s         a{sv}        -
.Set                                method    ssv       -            -
.PropertiesChanged                  signal    sa{sv}as  -            -
```

2. 使用gdbus
```shell
gdbus introspect -e -d com.example.Calculator -o /com/example/Calculator
```

Log:
```log
node /com/example/Calculator {
  interface org.freedesktop.DBus.Peer {
    methods:
      Ping();
      GetMachineId(out s machine_uuid);
    signals:
    properties:
  };
  interface org.freedesktop.DBus.Introspectable {
    methods:
      Introspect(out s xml_data);
    signals:
    properties:
  };
  interface org.freedesktop.DBus.Properties {
    methods:
      Get(in  s interface_name,
          in  s property_name,
          out v value);
      GetAll(in  s interface_name,
             out a{sv} props);
      Set(in  s interface_name,
          in  s property_name,
          in  v value);
    signals:
      PropertiesChanged(s interface_name,
                        a{sv} changed_properties,
                        as invalidated_properties);
    properties:
  };
  interface com.example.Calculator {
    methods:
      Echo(in  s arg_0,
           out s arg_1);
      Add(in  x arg_0,
          in  x arg_1,
          out x arg_2);
    signals:
    properties:
  };
};
```

## busctl/gdbus 交互
```shell
busctl --user call com.example.Calculator /com/example/Calculator com.example.Calculator Add 'xx' 10 2023
```

```shell
gdbus call -e -d com.example.Calculator -o /com/example/Calculator -m com.example.Calculator.Add  123 456
```


