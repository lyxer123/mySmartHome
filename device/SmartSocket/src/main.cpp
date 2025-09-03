#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// 从编译参数获取输出路数
#ifndef NUM_OUTPUTS
#define NUM_OUTPUTS 8
#endif

// MQTT配置
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_client_id = "SmartSocket_Controller";
const char* mqtt_username = "";
const char* mqtt_password = "";

// WiFi和MQTT客户端
WiFiClient espClient;
PubSubClient client(espClient);

// 输出控制引脚数组 (根据实际硬件连接修改)
const int outputPins[NUM_OUTPUTS] = {12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};

// 状态上报间隔(毫秒)
const unsigned long reportInterval = 10000;
unsigned long lastReportTime = 0;

// 设备状态
struct SocketState {
  bool enabled;
  float voltage;
  float current;
  float power;
  float energy;
  unsigned long lastChangeTime;
};

SocketState sockets[NUM_OUTPUTS];

void setupWiFi();
void reconnectMQTT();
void publishStatus();
void callback(char* topic, byte* payload, unsigned int length);
void updateEnergyConsumption();
void controlOutput(int index, bool state);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n智能插座系统启动中...");
  Serial.print("配置为 ");
  Serial.print(NUM_OUTPUTS);
  Serial.println(" 路输出");

  // 初始化输出引脚
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);
    sockets[i] = {false, 220.0, 0.0, 0.0, 0.0, 0};
  }

  // 设置WiFi
  setupWiFi();
  
  // 设置MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
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
  
  // 更新能耗计算
  updateEnergyConsumption();
  
  // 定期上报状态
  unsigned long currentMillis = millis();
  if (currentMillis - lastReportTime >= reportInterval) {
    lastReportTime = currentMillis;
    publishStatus();
  }
  
  delay(100);
}

void setupWiFi() {
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  
  String apName = "SmartSocket_" + String(ESP.getEfuseMac(), HEX);
  
  Serial.println("启动WiFi配置门户...");
  Serial.println("请连接到AP: " + apName);
  Serial.println("然后访问: 192.168.4.1 进行WiFi配置");
  
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
  while (!client.connected()) {
    Serial.print("尝试连接MQTT服务器...");
    
    bool connected = false;
    if (mqtt_username[0] != '\0') {
      connected = client.connect(mqtt_client_id, mqtt_username, mqtt_password);
    } else {
      connected = client.connect(mqtt_client_id);
    }
    
    if (connected) {
      Serial.println("已连接");
      // 订阅控制主题
      client.subscribe("mySmartHome/socket/control");
      client.publish("mySmartHome/device/status", "SmartSocket online");
    } else {
      Serial.print("连接失败, rc=");
      Serial.print(client.state());
      Serial.println(" 5秒后重试...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到消息 [");
  Serial.print(topic);
  Serial.print("] ");
  
  // 解析JSON
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);
  
  // 检查是否是控制命令
  if (doc.containsKey("output") && doc.containsKey("state")) {
    int outputIndex = doc["output"];
    bool state = doc["state"];
    
    if (outputIndex >= 0 && outputIndex < NUM_OUTPUTS) {
      controlOutput(outputIndex, state);
      publishStatus(); // 立即反馈状态变化
    }
  }
}

void controlOutput(int index, bool state) {
  if (index < 0 || index >= NUM_OUTPUTS) return;
  
  digitalWrite(outputPins[index], state ? HIGH : LOW);
  sockets[index].enabled = state;
  sockets[index].lastChangeTime = millis();
  
  Serial.print("输出 ");
  Serial.print(index);
  Serial.print(" 设置为 ");
  Serial.println(state ? "ON" : "OFF");
}

void updateEnergyConsumption() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastReportTime) / 3600000.0; // 转换为小时
  
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    if (sockets[i].enabled) {
      // 模拟用电数据 - 实际应用中应从传感器读取
      sockets[i].current = random(100, 1000) / 1000.0; // 0.1-1.0A
      sockets[i].power = sockets[i].voltage * sockets[i].current; // P=U*I
      sockets[i].energy += sockets[i].power * deltaTime; // 累计电量
    } else {
      sockets[i].current = 0.0;
      sockets[i].power = 0.0;
    }
  }
}

void publishStatus() {
  StaticJsonDocument<1024> doc;
  
  // 添加设备信息
  doc["deviceId"] = mqtt_client_id;
  doc["timestamp"] = millis();
  
  // 添加每路状态
  JsonArray outputs = doc.createNestedArray("outputs");
  
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    JsonObject output = outputs.createNestedObject();
    output["index"] = i;
    output["state"] = sockets[i].enabled;
    output["voltage"] = sockets[i].voltage;
    output["current"] = sockets[i].current;
    output["power"] = sockets[i].power;
    output["energy"] = sockets[i].energy;
    output["lastChange"] = sockets[i].lastChangeTime;
  }
  
  // 发布状态
  char jsonBuffer[1024];
  serializeJson(doc, jsonBuffer);
  
  if (client.publish("mySmartHome/socket/status", jsonBuffer)) {
    Serial.println("状态发布成功");
  } else {
    Serial.println("状态发布失败");
  }
}