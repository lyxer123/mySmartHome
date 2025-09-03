#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// DHT传感器配置
#define DHTPIN 4     // 连接DHT传感器的引脚 (ESP32标准开发板通常使用GPIO4)
#define LED_BUILTIN 2 // 内置LED引脚 (ESP32标准开发板通常使用GPIO2)
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// MQTT配置
/*
尝试连接MQTT服务器...连接失败, rc=-2 5秒后重试...
解决方案如下：
打开控制面板 > 系统和安全 > Windows Defender 防火墙
点击"启用或关闭Windows Defender防火墙"
对所有网络类型选择"关闭防火墙"
点击"确定"保存
*/
const char* mqtt_server = "192.168.124.1"; // 本地MQTT服务器
// const char* mqtt_server = "test.mosquitto.org"; // 本地MQTT服务器

const int mqtt_port = 1883;
const char* mqtt_topic = "data/pub"; // 与backend配置保持一致
const char* mqtt_client_id = "ESP32_TempHum_Client";
const char* mqtt_username = ""; // 如果需要认证，请设置用户名
const char* mqtt_password = ""; // 如果需要认证，请设置密码

// WiFi客户端和MQTT客户端
WiFiClient espClient;
PubSubClient client(espClient);

// 数据发送间隔(毫秒)
const unsigned long sendInterval = 30000; // 30秒
unsigned long lastSendTime = 0;

// 设备重置按钮引脚
const int resetButtonPin = 0; // ESP32上的BOOT按钮通常连接到GPIO0

// 模拟数据配置
bool useSimulatedData = false; // 设置为true使用模拟数据，false使用真实传感器数据
float baseTemperature = 25.0;  // 基础温度值
float baseHumidity = 50.0;     // 基础湿度值

// 函数声明
void setupWiFi();
void reconnectMQTT();
void publishSensorData(float temperature, float humidity);
void checkResetButton();
void blinkLED(int pin, int times, int delayMs);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n温湿度监测系统启动中...");
  
  // 设置内置LED
  pinMode(LED_BUILTIN, OUTPUT);
  
  // 设置重置按钮
  pinMode(resetButtonPin, INPUT);
  
  // 初始化DHT传感器
  dht.begin();
  
  // 设置WiFi
  setupWiFi();
  
  // 设置MQTT服务器
  client.setServer(mqtt_server, mqtt_port);
  
  Serial.println("系统初始化完成");
}

void loop() {
  // 检查WiFi连接
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi连接已断开，尝试重新连接...");
    setupWiFi();
  }
  
  // 检查MQTT连接
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  
  // 检查是否需要重置WiFi配置
  checkResetButton();
  
  unsigned long currentMillis = millis();
  
  // 检查是否到达发送间隔
  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;
    
    float humidity, temperature;
    
    if (useSimulatedData) {
      // 生成模拟数据
      temperature = baseTemperature + random(-20, 20) / 10.0; // 温度在基础值±2°C范围内波动
      humidity = baseHumidity + random(-100, 100) / 10.0;     // 湿度在基础值±10%范围内波动
      
      // 确保湿度在合理范围内
      humidity = constrain(humidity, 0, 100);
      
      Serial.println("使用模拟数据:");
    } else {
      // 读取真实传感器数据
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();
      
      // 检查读取是否成功
      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("无法从DHT传感器读取数据! 切换到模拟数据...");
        // 如果读取失败，使用模拟数据
        temperature = baseTemperature + random(-20, 20) / 10.0;
        humidity = baseHumidity + random(-100, 100) / 10.0;
        humidity = constrain(humidity, 0, 100);
      } else {
        Serial.println("使用传感器数据:");
      }
    }
    
    Serial.print("温度: ");
    Serial.print(temperature);
    Serial.print(" °C, 湿度: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    // 发布数据到MQTT
    publishSensorData(temperature, humidity);
  }
  
  // 短暂延时
  delay(100);
}

void setupWiFi() {
  // WiFiManager
  WiFiManager wifiManager;
  
  // 设置超时时间
  wifiManager.setConfigPortalTimeout(180); // 3分钟超时
  
  // 设置AP名称
  String apName = "TempHum_" + String(ESP.getEfuseMac(), HEX);
  
  // 闪烁LED指示WiFi配置模式
  blinkLED(LED_BUILTIN, 10, 100);
  
  Serial.println("启动WiFi配置门户...");
  Serial.println("请连接到AP: " + apName);
  Serial.println("然后访问: 192.168.4.1 进行WiFi配置");
  
  // 启动配置门户
  if (!wifiManager.autoConnect(apName.c_str())) {
    Serial.println("WiFi配置超时，重启设备...");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println("WiFi已连接");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  // 尝试连接MQTT服务器
  while (!client.connected()) {
    Serial.print("尝试连接MQTT服务器...");
    
    // 尝试连接
    bool connected = false;
    if (mqtt_username[0] != '\0') {
      connected = client.connect(mqtt_client_id, mqtt_username, mqtt_password);
    } else {
      connected = client.connect(mqtt_client_id);
    }
    
    if (connected) {
      Serial.println("已连接");
      // 连接成功后发布一条上线消息
      client.publish("mySmartHome/device/status", "TempHum device online");
    } else {
      Serial.print("连接失败, rc=");
      Serial.print(client.state());
      Serial.println(" 5秒后重试...");
      delay(5000);
    }
  }
}

void publishSensorData(float temperature, float humidity) {
  // 创建JSON数据
  StaticJsonDocument<200> doc;
  doc["deviceId"] = mqtt_client_id;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["timestamp"] = millis();
  
  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);
  
  // 发布到MQTT主题
  if (client.publish(mqtt_topic, jsonBuffer)) {
    Serial.println("数据发布成功");
    // 闪烁LED指示发送成功
    blinkLED(LED_BUILTIN, 2, 100);
  } else {
    Serial.println("数据发布失败");
  }
}

void checkResetButton() {
  // 检查重置按钮是否长按
  if (digitalRead(resetButtonPin) == LOW) {
    unsigned long pressStartTime = millis();
    
    // 等待按钮释放或超时
    while (digitalRead(resetButtonPin) == LOW) {
      // 如果按下超过5秒
      if (millis() - pressStartTime > 5000) {
        Serial.println("重置按钮长按，清除WiFi配置...");
        
        // 闪烁LED指示重置
        blinkLED(LED_BUILTIN, 20, 50);
        
        // 重置WiFi设置
        WiFiManager wifiManager;
        wifiManager.resetSettings();
        
        Serial.println("WiFi配置已清除，重启设备...");
        delay(1000);
        ESP.restart();
      }
      delay(100);
    }
  }
}

void printNetworkInfo() {
    Serial.println("--- 网络诊断信息 ---");
    Serial.print("WiFi状态: ");
    switch(WiFi.status()) {
        case WL_CONNECTED: Serial.println("已连接"); break;
        case WL_NO_SHIELD: Serial.println("无WiFi模块"); break;
        case WL_IDLE_STATUS: Serial.println("空闲状态"); break;
        case WL_NO_SSID_AVAIL: Serial.println("SSID不可用"); break;
        case WL_SCAN_COMPLETED: Serial.println("扫描完成"); break;
        case WL_CONNECT_FAILED: Serial.println("连接失败"); break;
        case WL_CONNECTION_LOST: Serial.println("连接丢失"); break;
        case WL_DISCONNECTED: Serial.println("未连接"); break;
        default: Serial.println("未知状态"); break;
    }
    
    Serial.print("本地IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("网关IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS IP: ");
    Serial.println(WiFi.dnsIP());
    
    Serial.print("MQTT服务器: ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);
    Serial.println("-------------------");
}

void blinkLED(int pin, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayMs);
    digitalWrite(pin, LOW);
    delay(delayMs);
  }
}