# SmartRelay - 智能继电器控制器

基于ESP8266的智能继电器控制器，支持1-4路继电器控制，通过MQTT协议与后台通信，支持WiFi智能配网。

## 功能特性

- ✅ 支持1-4路继电器控制（通过编译参数配置）
- ✅ MQTT协议通信
- ✅ WiFi智能配网（Web配置界面）
- ✅ 状态实时上报
- ✅ 设备唯一标识（MAC地址）
- ✅ 累计运行时间统计

## 硬件要求

- ESP8266 NodeMCU开发板
- 1-4路继电器模块
- 杜邦线若干
- 5V/3.3V电源

## 引脚连接

| 继电器路数 | ESP8266引脚 | NodeMCU标记 |
|----------|------------|------------|
| 第1路     | GPIO5      | D1         |
| 第2路     | GPIO4      | D2         |
| 第3路     | GPIO14     | D5         |
| 第4路     | GPIO12     | D6         |

## 软件配置

### 1. 安装PlatformIO

建议使用VS Code + PlatformIO插件进行开发。

### 2. 配置继电器路数

在 `platformio.ini` 中修改 `NUM_OUTPUTS` 参数：

```ini
build_flags = 
    -D NUM_OUTPUTS=4  ; 改为1,2,3或4
```

### 3. MQTT服务器配置

修改 `src/main.cpp` 中的MQTT配置：

```cpp
const char* mqtt_server = "your.mqtt.server";
const int mqtt_port = 1883;
const char* mqtt_username = "username";
const char* mqtt_password = "password";
```

## WiFi配置

1. 首次启动时，设备会创建AP热点：`SmartRelay_XXXXXX`
2. 连接到此热点
3. 浏览器访问 `192.168.4.1`
4. 选择要连接的WiFi并输入密码
5. 设备将自动连接并保存配置

## MQTT主题

### 控制主题
```
mySmartHome/relay/{MAC地址}/control
```

控制消息格式：
```json
{
  "relay": 0,     // 继电器索引 (0-3)
  "state": true   // 开关状态 (true/false)
}
```

### 状态主题
```
mySmartHome/relay/{MAC地址}/status
```

状态消息格式：
```json
{
  "deviceId": "AABBCCDDEEFF",
  "deviceType": "SmartRelay",
  "numOutputs": 4,
  "timestamp": 1234567890,
  "rssi": -55,
  "relays": [
    {
      "index": 0,
      "state": true,
      "lastChange": 1234567800,
      "onDuration": 3600000,
      "pin": 5
    }
  ]
}
```

## 编译和上传

```bash
# 使用PlatformIO编译
pio run

# 上传到设备
pio run --target upload

# 监视串口输出
pio device monitor
```

## 故障排除

1. **无法连接WiFi**: 长按复位键重置WiFi配置
2. **MQTT连接失败**: 检查服务器地址和端口
3. **继电器不工作**: 检查引脚连接和电源电压

## 许可证

MIT License