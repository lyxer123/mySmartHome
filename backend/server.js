const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const fs = require('fs');
const path = require('path');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');

const app = express();

// 用户数据存储
const usersFilePath = path.join(__dirname, 'users.json');

// 确保用户文件存在
if (!fs.existsSync(usersFilePath)) {
  fs.writeFileSync(usersFilePath, JSON.stringify([]));
}

// 读取用户数据
function readUsers() {
  try {
    return JSON.parse(fs.readFileSync(usersFilePath, 'utf8'));
  } catch (error) {
    return [];
  }
}

// 保存用户数据
function saveUsers(users) {
  fs.writeFileSync(usersFilePath, JSON.stringify(users, null, 2));
}

// JWT密钥
const JWT_SECRET = 'your_jwt_secret_key_here';
const PORT = process.env.PORT || 3000;

// 中间件
app.use(cors({
  origin: 'http://localhost:8080',
  credentials: true
}));
app.use(bodyParser.json());

// 数据存储路径
const dataFilePath = path.join(__dirname, 'sensor-data.json');

// 确保数据文件存在
if (!fs.existsSync(dataFilePath)) {
  fs.writeFileSync(dataFilePath, JSON.stringify({
    readings: []
  }));
}

// 读取传感器数据
function readSensorData() {
  try {
    const data = fs.readFileSync(dataFilePath, 'utf8');
    return JSON.parse(data);
  } catch (error) {
    console.error('读取数据文件错误:', error);
    return { readings: [] };
  }
}

// 保存传感器数据
function saveSensorData(data) {
  try {
    fs.writeFileSync(dataFilePath, JSON.stringify(data, null, 2));
    return true;
  } catch (error) {
    console.error('保存数据文件错误:', error);
    return false;
  }
}

// 认证中间件
function authenticateToken(req, res, next) {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];
  
  if (!token) return res.sendStatus(401);
  
  jwt.verify(token, JWT_SECRET, (err, user) => {
    if (err) return res.sendStatus(403);
    req.user = user;
    next();
  });
}

// 认证路由
app.post('/api/auth/register', async (req, res) => {
  const { username, password } = req.body;
  
  if (!username || !password) {
    return res.status(400).json({ message: '用户名和密码不能为空' });
  }
  
  const users = readUsers();
  
  if (users.some(user => user.username === username)) {
    return res.status(400).json({ message: '用户名已存在' });
  }
  
  const hashedPassword = await bcrypt.hash(password, 10);
  const newUser = { username, password: hashedPassword };
  
  users.push(newUser);
  saveUsers(users);
  
  res.status(201).json({ message: '注册成功' });
});

app.post('/api/auth/login', async (req, res) => {
  const { username, password } = req.body;
  
  const users = readUsers();
  const user = users.find(u => u.username === username);
  
  if (!user || !(await bcrypt.compare(password, user.password))) {
    return res.status(401).json({ message: '用户名或密码错误' });
  }
  
  const token = jwt.sign({ username }, JWT_SECRET, { expiresIn: '1h' });
  res.json({ token });
});

// API路由
// 获取最新传感器数据
app.get('/api/sensor-data/latest', (req, res) => {
  const data = readSensorData();
  if (data.readings.length > 0) {
    res.json(data.readings[data.readings.length - 1]);
  } else {
    res.status(404).json({ message: '没有可用的传感器数据' });
  }
});

// 获取所有传感器数据
app.get('/api/sensor-data', (req, res) => {
  const data = readSensorData();
  res.json(data);
});

// 接收传感器数据
app.post('/api/sensor-data', (req, res) => {
  const { deviceId, temperature, humidity, timestamp } = req.body;
  
  if (!deviceId || temperature === undefined || humidity === undefined) {
    return res.status(400).json({ message: '缺少必要的数据字段' });
  }
  
  const data = readSensorData();
  
  // 添加新的读数
  const newReading = {
    deviceId,
    temperature,
    humidity,
    timestamp: timestamp || Date.now(),
    receivedAt: Date.now()
  };
  
  data.readings.push(newReading);
  
  // 限制存储的数据量（保留最新的100条记录）
  if (data.readings.length > 100) {
    data.readings = data.readings.slice(-100);
  }
  
  if (saveSensorData(data)) {
    res.status(201).json({ message: '数据已保存', reading: newReading });
  } else {
    res.status(500).json({ message: '保存数据时出错' });
  }
});

// 启动服务器
app.listen(PORT, () => {
  console.log(`服务器运行在端口 ${PORT}`);
});