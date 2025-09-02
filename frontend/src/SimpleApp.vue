<template>
  <div id="app">
    <header>
      <h1>智能家居温湿度监测系统</h1>
    </header>
    
    <main>
      <div class="dashboard">
        <div class="sensor-card temperature">
          <h2>温度</h2>
          <div class="sensor-value">{{ temperature }} °C</div>
          <div class="sensor-icon">🌡️</div>
        </div>
        
        <div class="sensor-card humidity">
          <h2>湿度</h2>
          <div class="sensor-value">{{ humidity }} %</div>
          <div class="sensor-icon">💧</div>
        </div>
      </div>
      
      <div class="status-panel">
        <div class="status-item">
          <span class="status-label">设备ID:</span>
          <span class="status-value">{{ deviceId }}</span>
        </div>
        <div class="status-item">
          <span class="status-label">最后更新:</span>
          <span class="status-value">{{ lastUpdate }}</span>
        </div>
        <div class="status-item">
          <span class="status-label">连接状态:</span>
          <span class="status-value" :class="{ 'status-online': isConnected }">
            {{ isConnected ? '在线' : '离线' }}
          </span>
        </div>
      </div>
    </main>
  </div>
</template>

<script>
export default {
  name: 'App',
  data() {
    return {
      temperature: '25.0',
      humidity: '60.0',
      deviceId: 'TempHum-001',
      lastUpdate: new Date().toLocaleString(),
      isConnected: true,
      pollingInterval: null
    };
  },
  mounted() {
    // 模拟数据更新
    this.pollingInterval = setInterval(() => {
      this.temperature = (Math.random() * 10 + 20).toFixed(1);
      this.humidity = (Math.random() * 20 + 50).toFixed(1);
      this.lastUpdate = new Date().toLocaleString();
    }, 5000);
  },
  beforeDestroy() {
    clearInterval(this.pollingInterval);
  }
};
</script>

<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Helvetica Neue', Arial, sans-serif;
  background-color: #f5f7fa;
  color: #333;
  line-height: 1.6;
}

#app {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

header {
  text-align: center;
  margin-bottom: 30px;
}

header h1 {
  color: #2c3e50;
  font-size: 2.5rem;
}

.dashboard {
  display: flex;
  justify-content: space-around;
  flex-wrap: wrap;
  margin-bottom: 30px;
}

.sensor-card {
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  padding: 20px;
  width: 45%;
  min-width: 250px;
  margin-bottom: 20px;
  position: relative;
  overflow: hidden;
}

.sensor-card h2 {
  font-size: 1.5rem;
  margin-bottom: 15px;
  color: #555;
}

.sensor-value {
  font-size: 3rem;
  font-weight: bold;
}

.sensor-icon {
  position: absolute;
  right: 20px;
  top: 20px;
  font-size: 3rem;
  opacity: 0.2;
}

.temperature {
  background: linear-gradient(135deg, #fff 0%, #ffe8e8 100%);
}

.temperature .sensor-value {
  color: #e74c3c;
}

.humidity {
  background: linear-gradient(135deg, #fff 0%, #e3f2fd 100%);
}

.humidity .sensor-value {
  color: #3498db;
}

.status-panel {
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  padding: 20px;
}

.status-item {
  display: flex;
  justify-content: space-between;
  padding: 10px 0;
  border-bottom: 1px solid #eee;
}

.status-item:last-child {
  border-bottom: none;
}

.status-label {
  font-weight: bold;
  color: #555;
}

.status-value {
  color: #777;
}

.status-online {
  color: #2ecc71;
  font-weight: bold;
}

@media (max-width: 768px) {
  .sensor-card {
    width: 100%;
  }
}
</style>