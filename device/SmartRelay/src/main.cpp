#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// 从编译参数获取继电器路数 (1-4路)
#ifndef NUM_OUTPUTS
#define NUM_OUTPUTS 4
#endif

// MQTT配置
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_client_id = "SmartRelay_Controller";
const char* mqtt_username = "";
const char* mqtt_password = "";

// WiFi和MQTT客户端
WiFiClient espClient;
PubSubClient client(espClient);

// 继电器控制引脚 (ESP8266 NodeMCU引脚映射)
// D1=GPIO5, D2=GPIO4, D3=GPIO0, D4=GPIO2, D5=GPIO14, D6=GPIO12, D7=GPIO13, D8=GPIO15
const int relayPins[NUM_OUTPUTS] = {D1, D2, D5, D6}; // 使用D1,D2,D5,D6作为继电器控制

// 状态上报间隔(毫秒)
const unsigned long reportInterval = 5000;
unsigned long lastReportTime = 0;

// 继电器状态
struct RelayState {
  bool enabled;
  unsigned long lastChangeTime;
  unsigned long onDuration; // 累计开启时间(毫秒)
};

RelayState relays[NUM_OUTPUTS];

void setupWiFi();
void reconnectMQTT();
void publishStatus();
void callback(char* topic, byte* payload, unsigned int length);
void controlRelay(int index, bool state);
String getMacAddress();

void updateRelayTimers() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    if (relays[i].enabled) {
      relays[i].onDuration += (currentTime - lastReportTime);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n智能继电器系统启动中...");
  Serial.print("配置为 ");
  Serial.print(NUM_OUTPUTS);
  Serial.println(" 路继电器输出");

  // 初始化继电器引脚
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); // 继电器默认关闭
    relays[i] = {false, 0, 0};
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
  
  // 更新继电器状态计时
  updateRelayTimers();
  
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
  
  String apName = "SmartRelay_" + getMacAddress();
  
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
      String controlTopic = "mySmartHome/relay/" + getMacAddress() + "/control";
      client.subscribe(controlTopic.c_str());
      
      // 发布上线状态
      String onlineMsg = "SmartRelay online - " + getMacAddress();
      client.publish("mySmartHome/device/status", onlineMsg.c_str());
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
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.print("JSON解析错误: ");
    Serial.println(error.c_str());
    return;
  }
  
  // 检查控制命令格式
  if (doc.containsKey("relay") && doc.containsKey("state")) {
    int relayIndex = doc["relay"];
    bool state = doc["state"];
    
    if (relayIndex >= 0 && relayIndex < NUM_OUTPUTS) {
      controlRelay(relayIndex, state);
      publishStatus(); // 立即反馈状态变化
    } else {
      Serial.print("无效的继电器索引: ");
      Serial.println(relayIndex);
    }
  }
}

void controlRelay(int index, bool state) {
  if (index < 0 || index >= NUM_OUTPUTS) return;
  
  digitalWrite(relayPins[index], state ? HIGH : LOW);
  
  // 更新状态和时间统计
  if (relays[index].enabled != state) {
    relays[index].enabled = state;
    relays[index].lastChangeTime = millis();
    
    Serial.print("继电器 ");
    Serial.print(index);
    Serial.print(" 设置为 ");
    Serial.println(state ? "ON" : "OFF");
  }
}



void publishStatus() {
  StaticJsonDocument<512> doc;
  
  // 添加设备信息
  doc["deviceId"] = getMacAddress();
  doc["deviceType"] = "SmartRelay";
  doc["numOutputs"] = NUM_OUTPUTS;
  doc["timestamp"] = millis();
  doc["rssi"] = WiFi.RSSI();
  
  // 添加每路继电器状态
  JsonArray relayStates = doc.createNestedArray("relays");
  
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    JsonObject relay = relayStates.createNestedObject();
    relay["index"] = i;
    relay["state"] = relays[i].enabled;
    relay["lastChange"] = relays[i].lastChangeTime;
    relay["onDuration"] = relays[i].onDuration;
    relay["pin"] = relayPins[i];
  }
  
  // 发布状态
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  
  String statusTopic = "mySmartHome/relay/" + getMacAddress() + "/status";
  if (client.publish(statusTopic.c_str(), jsonBuffer)) {
    Serial.println("状态发布成功");
  } else {
    Serial.println("状态发布失败");
  }
}

String getMacAddress() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}