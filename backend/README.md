# 智能家居系统后端

本文档提供了智能家居系统后端的架构说明、安装指南以及操作方法。

## 项目架构

智能家居系统后端采用Python开发，主要包含以下组件：

- **API 服务**：基于Flask的RESTful API接口，处理前端请求
- **MQTT 客户端**：负责与智能设备通信，接收和发送数据
- **数据存储**：使用SQLite数据库存储传感器数据和设备状态
- **配置管理**：集中管理服务器、MQTT和数据库配置

## 技术栈

- 编程语言：Python
- Web框架：Flask
- 数据库：SQLite
- 消息队列：MQTT (使用paho-mqtt客户端)
- 跨域支持：Flask-CORS

## 安装依赖

### 前提条件

- Python 3.8+
- pip (Python包管理器)
- MQTT代理服务器 (如Mosquitto)

### 安装步骤

1. 进入后端目录：

```bash
cd backend
```

2. 安装依赖包：

```bash
pip install -r requirements.txt
```

## 配置

配置信息位于`config.py`文件中，包括：

1. Web服务器配置：
```python
WEB_SERVER_CONFIG = {
    'host': '0.0.0.0',  # 允许所有IP访问
    'port': 5000,       # Flask Web服务端口
    'debug': True       # 调试模式
}
```

2. MQTT配置：
```python
MQTT_CONFIG = {
    'broker_url': 'localhost',  # MQTT代理地址
    'port': 1883,               # MQTT标准端口
    'keepalive': 60,            # 心跳间隔
    'topic_subscribe': 'data/pub',  # 订阅主题
    'topic_publish': 'data/sub',    # 发布主题
    'username': None,           # 认证用户名(如有)
    'password': None            # 认证密码(如有)
}
```

3. 数据库配置：
```python
DATABASE_CONFIG = {
    'database_path': 'app.db',  # SQLite数据库文件路径
    'table_name': 'sensor_data' # 数据表名
}
```

## 运行

启动后端服务器：

```bash
python app.py
```

服务器默认运行在 `http://localhost:5000`

## API 接口

### 1. 获取数据

- **URL**: `/api/data`
- **方法**: GET
- **参数**: 
  - `limit`: 返回的数据条数 (默认: 10)
- **响应**: 
  ```json
  {
    "data": [
      {
        "id": 1,
        "timestamp": "2023-09-01 12:34:56",
        "data": {"temperature": 25.5, "humidity": 60}
      },
      ...
    ]
  }
  ```

### 2. 发布数据

- **URL**: `/api/publish`
- **方法**: POST
- **请求体**: 
  ```json
  {
    "temperature": 25.5,
    "humidity": 60
  }
  ```
- **响应**: 
  ```json
  {
    "status": "success",
    "message": "Data published"
  }
  ```

## MQTT 通信

系统使用MQTT协议与智能设备通信：

- **订阅主题**: `data/pub` - 接收来自设备的数据
- **发布主题**: `data/sub` - 向设备发送控制命令

接收到的数据会自动存储到SQLite数据库中。

## 数据库

系统使用SQLite数据库存储传感器数据：

- **数据表**: `sensor_data`
- **字段**:
  - `id`: 自增主键
  - `timestamp`: 时间戳
  - `data`: JSON格式的传感器数据

## 故障排除

如果遇到问题，请检查：

1. MQTT代理服务器是否正常运行
2. 数据库文件权限是否正确
3. 检查应用日志获取详细错误信息

## 项目结构

```
backend/
├── app.py             # 应用入口和API定义
├── config.py          # 配置文件
├── database.py        # 数据库操作
├── mqtt_handler.py    # MQTT客户端处理
├── app.db             # SQLite数据库文件
├── requirements.txt   # 依赖包列表
└── README.md          # 本文档