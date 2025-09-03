# TempHum 温湿度监测设备

这是一个基于ESP32-C3的温湿度监测设备项目，使用DHT传感器采集环境温湿度数据，并通过MQTT协议将数据发送到智能家居系统。

## 硬件要求

- ESP32-C3-DevKitM-1开发板
- DHT温湿度传感器(DHT11/DHT22/AM2302)
- 连接线和面包板(可选)

## 软件依赖

项目使用PlatformIO进行开发和编译，依赖以下库：

- DHT sensor library (v1.4.4+)
- Adafruit Unified Sensor (v1.1.9+)
- ArduinoJson (v6.21.3+)
- PubSubClient (v2.8+)

## 编译说明

### 安装PlatformIO

1. 安装[Visual Studio Code](https://code.visualstudio.com/)
2. 在VSCode扩展市场中安装PlatformIO IDE扩展

### 编译和上传项目

#### 使用VSCode图形界面

1. 使用VSCode打开TempHum文件夹
2. 连接ESP32-C3开发板到电脑
3. 在PlatformIO侧边栏中点击"Build"按钮编译项目
4. 编译成功后，点击"Upload"按钮将固件上传到开发板

#### 使用命令行编译

如果您更喜欢使用命令行，可以使用以下命令：

```bash
# 进入项目目录
cd path/to/mySmartHome/device/TempHum

# 编译项目
pio run

# 编译并上传到开发板
pio run --target upload

# 清理编译文件
pio run --target clean
```

#### 高级编译选项

```bash
# 指定环境编译（如果有多个环境配置）
pio run -e esp32-c3-devkitm-1

# 显示详细编译信息
pio run -v

# 编译特定文件
pio run --source-dir=src/specific_folder
```

### 监控串口输出

项目配置了115200的波特率用于串口监控。上传完成后，可以通过以下步骤查看设备输出：

1. 在PlatformIO侧边栏中点击"Monitor"按钮
2. 或者使用命令：`pio device monitor --baud 115200`

## 项目配置

项目使用platformio.ini文件进行配置：

```ini
[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
    adafruit/Adafruit Unified Sensor@^1.1.9
    bblanchon/ArduinoJson@^6.21.3
    knolleary/PubSubClient@^2.8
monitor_speed = 115200
```

## 使用说明

1. 将DHT传感器连接到ESP32-C3开发板
2. 在源代码中配置WiFi和MQTT服务器信息
3. 编译并上传代码
4. 设备将自动连接WiFi并开始发送温湿度数据到MQTT服务器

## 故障排除

- 确保所有依赖库已正确安装
- 检查WiFi和MQTT配置是否正确
- 通过串口监视器查看设备日志以诊断问题