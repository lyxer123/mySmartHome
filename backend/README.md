# 智能家居系统后端

本文档提供了智能家居系统后端的架构说明、安装指南以及操作方法。

## 项目架构

智能家居系统后端采用模块化设计，主要包含以下组件：

- **API 服务**：处理前端请求，提供 RESTful API 接口
- **设备管理**：负责设备连接、状态监控和控制
- **数据存储**：管理用户数据、设备配置和历史记录
- **认证授权**：处理用户登录、权限验证
- **事件处理**：管理设备触发的事件和自动化规则

## 技术栈

- 编程语言：Node.js
- 框架：Express.js
- 数据库：MongoDB
- 消息队列：MQTT (用于设备通信)
- 认证：JWT (JSON Web Token)

## 安装依赖

### 前提条件

- Node.js (v14.0.0 或更高版本)
- npm (v6.0.0 或更高版本)
- MongoDB (v4.0 或更高版本)

### 安装步骤

1. 进入后端目录：

```bash
cd backend
```

2. 安装依赖包：

```bash
npm install
```

## 配置

1. 创建环境配置文件：

```bash
cp .env.example .env
```

2. 根据需要修改 `.env` 文件中的配置项：

```
PORT=3000
MONGODB_URI=mongodb://localhost:27017/smart_home
JWT_SECRET=your_jwt_secret
MQTT_BROKER=mqtt://localhost:1883
```

## 编译与运行

### 开发环境

启动开发服务器（支持热重载）：

```bash
npm run dev
```

### 生产环境

1. 构建项目：

```bash
npm run build
```

2. 启动服务：

```bash
npm start
```

## API 文档

启动服务后，可以通过以下地址访问 API 文档：

```
http://localhost:3000/api-docs
```

## 常见操作

### 数据库管理

- 初始化数据库：

```bash
npm run db:init
```

- 数据迁移：

```bash
npm run db:migrate
```

- 数据备份：

```bash
npm run db:backup
```

### 日志管理

日志文件存储在 `logs` 目录下，可以通过以下命令查看：

```bash
# 查看错误日志
cat logs/error.log

# 查看访问日志
cat logs/access.log
```

## 测试

运行单元测试：

```bash
npm test
```

运行集成测试：

```bash
npm run test:integration
```

## 故障排除

如果遇到问题，请检查：

1. MongoDB 服务是否正常运行
2. 环境变量是否正确配置
3. 查看日志文件获取详细错误信息

## 部署

推荐使用 Docker 进行部署：

```bash
# 构建 Docker 镜像
docker build -t smart-home-backend .

# 运行容器
docker run -p 3000:3000 -d smart-home-backend
```

## 项目结构

```
backend/
├── src/
│   ├── config/         # 配置文件
│   ├── controllers/    # 控制器
│   ├── middlewares/    # 中间件
│   ├── models/         # 数据模型
│   ├── routes/         # 路由定义
│   ├── services/       # 业务逻辑
│   ├── utils/          # 工具函数
│   └── app.js          # 应用入口
├── tests/              # 测试文件
├── logs/               # 日志文件
├── .env                # 环境变量
├── .env.example        # 环境变量示例
├── package.json        # 项目依赖
└── README.md           # 本文档