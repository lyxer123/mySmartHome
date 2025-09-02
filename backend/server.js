const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// 中间件
app.use(cors());
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