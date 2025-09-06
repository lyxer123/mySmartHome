# 库文件安装指南

## SmartSocket 项目

### WiFiManager 库安装
1. 将下载的 WiFiManager 库文件解压
2. 将整个库文件夹复制到：`device/SmartSocket/lib/WiFiManager/`
3. 确保库文件结构如下：
   ```
   device/SmartSocket/lib/WiFiManager/
   ├── WiFiManager.h
   ├── WiFiManager.cpp
   ├── LICENSE
   └── README.md
   ```

## TempHum 项目

### DHT传感器库安装
1. 将下载的 DHT sensor library 库文件解压
2. 将整个库文件夹复制到：`device/TempHum/lib/DHT-sensor-library/`
3. 确保库文件结构如下：
   ```
   device/TempHum/lib/DHT-sensor-library/
   ├── DHT.h
   ├── DHT.cpp
   ├── DHT_U.h
   ├── DHT_U.cpp
   ├── LICENSE
   └── README.md
   ```

## PlatformIO 配置说明

两个项目的 `platformio.ini` 文件已经配置为：
- 优先使用本地库文件（如果存在）
- 如果本地库不存在，则从在线仓库下载

## 验证安装

1. 打开 PlatformIO IDE
2. 打开对应的项目（SmartSocket 或 TempHum）
3. 执行编译命令，检查是否能够正确找到库文件
4. 如果编译成功，说明库文件放置正确

## 注意事项

- 确保库文件的版本与代码兼容
- 如果遇到编译错误，检查库文件路径是否正确
- 本地库优先于在线库，方便离线开发和版本控制


## XiaoZhi 项目


1. 项目在esp-idf 5.4.2下可以编译成功
