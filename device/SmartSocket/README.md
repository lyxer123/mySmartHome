# 智能插座(SmartSocket)项目

基于ESP32的多路智能插座控制器，支持远程控制和用电监测。

## 功能特性

- 支持最多16路输出控制
- 每路独立状态监测
- 电压、电流、功率和电量监测
- MQTT协议通信
- WiFi配置门户(通过手机或电脑配置)

## 硬件配置

1. ESP32开发板
2. 继电器模块(根据输出路数选择)
3. 电流传感器(如ACS712)
4. 电源模块

## 引脚配置

默认输出引脚配置(可在代码中修改):

```c
const int outputPins[NUM_OUTPUTS] = {12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
```

## 配置参数

在`platformio.ini`中可配置:

```ini
build_flags = 
    -D NUM_OUTPUTS=8 ; 设置输出路数(1-16)
```

## MQTT主题

- 控制主题: `mySmartHome/socket/control`
  - 格式: `{"output":0, "state":true}` (0为第一路)
- 状态主题: `mySmartHome/socket/status`
  - 包含所有路的状态和用电数据

## 编译上传

1. 安装PlatformIO插件
2. 连接ESP32开发板
3. 编译并上传程序

## 使用说明

1. 设备启动后创建WiFi热点 `SmartSocket_[设备ID]`
2. 连接到该热点，访问192.168.4.1
3. 配置设备连接到您的WiFi网络
4. 通过MQTT客户端发送控制命令