# 智能家居系统前端

本文档提供了智能家居系统前端的架构说明、安装指南以及操作方法。

## 技术架构

智能家居系统前端采用现代化的组件化设计，主要包含以下部分：

- **用户界面**：响应式设计，适配多种设备尺寸
- **状态管理**：集中管理应用状态
- **路由系统**：处理页面导航
- **API 集成**：与后端服务通信
- **实时更新**：设备状态实时监控和显示

## 技术栈

- 框架：Vue.js 3
- 状态管理：Vuex/Pinia
- 路由：Vue Router
- UI 组件库：Element Plus
- HTTP 客户端：Axios
- 构建工具：Vite
- CSS 预处理器：SCSS

## 安装依赖

### 前提条件

- Node.js (v16.0.0 或更高版本)
- npm (v7.0.0 或更高版本) 或 yarn (v1.22.0 或更高版本)

### 安装步骤

1. 进入前端目录：

```bash
cd frontend
```

2. 安装依赖包：

```bash
# 使用 npm
npm install

# 或使用 yarn
yarn install
```

## 配置方法

1. 创建环境配置文件：

```bash
cp .env.example .env.local
```

2. 根据需要修改 `.env.local` 文件中的配置项：

```
VITE_API_BASE_URL=http://localhost:3000/api
VITE_MQTT_BROKER=ws://localhost:9001
VITE_APP_TITLE=智能家居系统
```

## 编译与运行

### 开发环境

启动开发服务器（支持热重载）：

```bash
# 使用 npm
npm run dev

# 或使用 yarn
yarn dev
```

开发服务器默认运行在 `http://localhost:5173`

### 生产环境

1. 构建项目：

```bash
# 使用 npm
npm run build

# 或使用 yarn
yarn build
```

2. 预览构建结果：

```bash
# 使用 npm
npm run preview

# 或使用 yarn
yarn preview
```

## 项目结构

```
frontend/
├── public/             # 静态资源
├── src/
│   ├── assets/         # 资源文件（图片、字体等）
│   ├── components/     # 可复用组件
│   ├── composables/    # 组合式函数
│   ├── layouts/        # 布局组件
│   ├── router/         # 路由配置
│   ├── services/       # API 服务
│   ├── stores/         # 状态管理
│   ├── styles/         # 全局样式
│   ├── utils/          # 工具函数
│   ├── views/          # 页面视图
│   ├── App.vue         # 根组件
│   └── main.js         # 应用入口
├── .env.example        # 环境变量示例
├── index.html          # HTML 模板
├── vite.config.js      # Vite 配置
├── package.json        # 项目依赖
└── README.md           # 本文档
```

## 功能模块

前端应用包含以下主要功能模块：

- **仪表盘**：显示设备状态概览和重要数据
- **设备控制**：管理和控制各种智能设备
- **场景管理**：创建和管理自动化场景
- **用户设置**：个人信息和偏好设置
- **系统配置**：系统级别的配置选项

## 开发指南

### 组件开发

- 遵循单一职责原则
- 使用 Props 和 Events 进行组件通信
- 使用组合式 API (Composition API) 组织逻辑

### 状态管理

- 全局状态存储在 Vuex/Pinia 中
- 组件本地状态使用 reactive 或 ref
- 使用 actions 处理异步操作

### 样式指南

- 使用 SCSS 预处理器
- 遵循 BEM 命名规范
- 优先使用 CSS 变量实现主题定制

## API 文档

前端与后端 API 的交互文档可在开发环境中访问：

```
http://localhost:5173/api-docs
```

## 测试

运行单元测试：

```bash
# 使用 npm
npm run test:unit

# 或使用 yarn
yarn test:unit
```

运行端到端测试：

```bash
# 使用 npm
npm run test:e2e

# 或使用 yarn
yarn test:e2e
```

## 部署

### 静态托管

构建后的文件位于 `dist` 目录，可以部署到任何静态文件服务器：

```bash
# 例如使用 nginx
cp -r dist/* /var/www/html/
```

### Docker 部署

```bash
# 构建 Docker 镜像
docker build -t smart-home-frontend .

# 运行容器
docker run -p 80:80 -d smart-home-frontend
```

## 常见问题

### 开发环境问题

- 如果遇到依赖问题，尝试删除 `node_modules` 目录并重新安装
- 确保 Node.js 和 npm/yarn 版本符合要求
- 检查环境变量配置是否正确

### 构建问题

- 检查 `vite.config.js` 中的配置
- 确保所有导入的模块都存在
- 查看构建日志获取详细错误信息