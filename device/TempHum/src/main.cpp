#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// 网络配置
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// 后端服务器配置
const char* serverUrl = "http://your-backend-server/api/sensor-data";

// DHT传感器配置
#define DHTPIN 10     // 连接DHT传感器的引脚
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// 数据发送间隔(毫秒)
const unsigned long sendInterval = 30000;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("温湿度监测系统启动中...");
  
  // 初始化DHT传感器
  dht.begin();
  
  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi已连接");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  
  // 检查是否到达发送间隔
  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;
    
    // 读取温湿度数据
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // 检查读取是否成功
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("无法从DHT传感器读取数据!");
      return;
    }
    
    Serial.print("温度: ");
    Serial.print(temperature);
    Serial.print(" °C, 湿度: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    // 检查WiFi连接
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      
      // 配置请求
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      
      // 创建JSON数据
      StaticJsonDocument<200> doc;
      doc["deviceId"] = "TempHum-001";
      doc["temperature"] = temperature;
      doc["humidity"] = humidity;
      doc["timestamp"] = millis();
      
      String requestBody;
      serializeJson(doc, requestBody);
      
      // 发送POST请求
      int httpResponseCode = http.POST(requestBody);
      
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP响应码: " + String(httpResponseCode));
        Serial.println("响应内容: " + response);
      } else {
        Serial.print("错误码: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
    } else {
      Serial.println("WiFi连接已断开");
      // 尝试重新连接
      WiFi.begin(ssid, password);
    }
  }
  
  // 短暂延时
  delay(2000);
}