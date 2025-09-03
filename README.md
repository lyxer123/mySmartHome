# 智能家居系统

一个基于Web的智能家居监控与控制系统，支持用户认证、数据可视化和设备控制。

## 系统架构

### 后端 (Python + Flask)
- RESTful API服务
- MQTT客户端与智能设备通信
- SQLite数据库存储用户信息和传感器数据
- 用户认证与授权

### 前端 (Vue.js)
- 响应式用户界面，支持移动端
- 用户注册与登录系统
- 实时数据可视化图表
- 设备控制面板

## 快速开始

### 后端设置
```bash
# 进入后端目录
cd backend

# 安装依赖
pip install -r requirements.txt

# 启动服务器
python app.py
```

### 前端设置
```bash
# 进入前端目录
cd frontend

# 安装依赖
npm install

# 开发模式启动
npm run serve

# 构建生产版本
npm run build
```

## 主要功能

- **用户管理**：注册、登录和身份验证
- **数据可视化**：温度变化曲线和实时数据展示
- **设备控制**：通过MQTT协议发送控制命令
- **移动端支持**：响应式设计，适配各种屏幕尺寸

## 技术栈

- **后端**：Python, Flask, SQLite, MQTT
- **前端**：Vue.js, Vue Router, ECharts, HTML5/CSS3

## 访问方式

- 前端开发服务器：http://localhost:8080
- 后端API服务器：http://localhost:5000