# 智能家居温湿度监测系统

这是一个基于ESP32-C3的温湿度监测系统，包含设备端、后端服务和前端界面。

## 项目结构

```
mySmartHome/
├── device/
│   └── TempHum/          # ESP32-C3温湿度传感器项目
│       ├── platformio.ini # PlatformIO配置
│       └── src/
│           └── main.cpp   # 设备端主程序
├── backend/              # 后端服务
│   ├── package.json      # 后端依赖
│   └── server.js         # 后端服务器代码
└── frontend/             # 前端界面
    ├── package.json      # 前端依赖
    ├── public/           # 静态资源
    └── src/              # 前端源代码
        ├── App.vue       # 主应用组件
        └── main.js       # 入口文件
```

## 设备端

基于ESP32-C3和DHT22传感器，使用PlatformIO框架开发。

### 功能

- 读取温湿度数据
- 通过WiFi连接网络
- 定期向后端发送数据

### 安装与配置

1. 安装PlatformIO IDE
2. 打开device/TempHum项目
3. 修改main.cpp中的WiFi凭据和服务器URL
4. 编译并上传到ESP32-C3设备

## 后端服务

基于Node.js和Express的RESTful API服务。

### 功能

- 接收并存储传感器数据
- 提供API获取最新和历史数据

### 安装与运行

```bash
cd backend
npm install
npm start
```

服务器将在http://localhost:3000运行。

## 前端界面

基于Vue.js的响应式Web应用。

### 功能

- 实时显示温湿度数据
- 历史数据图表展示
- 设备状态监控

### 安装与运行

```bash
cd frontend
npm install
npm run serve
```

应用将在http://localhost:8080运行。

## 接口说明

### 获取最新数据

```
GET /api/sensor-data/latest
```

### 获取所有历史数据

```
GET /api/sensor-data
```

### 发送传感器数据

```
POST /api/sensor-data
Content-Type: application/json

{
  "deviceId": "TempHum-001",
  "temperature": 25.5,
  "humidity": 60.2,
  "timestamp": 1630000000000
}