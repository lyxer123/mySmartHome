<template>
  <div>
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
  </div>
</template>

<script>
export default {
  data() {
    return {
      temperature: '--',
      humidity: '--',
      deviceId: '--',
      lastUpdate: '--',
      isConnected: false
    }
  },
  methods: {
    async fetchData() {
      try {
        const response = await this.$axios.get('/sensor-data/latest')
        this.temperature = response.data.temperature.toFixed(1)
        this.humidity = response.data.humidity.toFixed(1)
        this.deviceId = response.data.deviceId
        this.lastUpdate = new Date(response.data.receivedAt).toLocaleString()
        this.isConnected = true
      } catch (error) {
        console.error('获取数据失败:', error)
        this.isConnected = false
      }
    }
  },
  created() {
    this.fetchData()
    this.interval = setInterval(this.fetchData, 10000)
  },
  beforeDestroy() {
    clearInterval(this.interval)
  }
}
</script>

<style scoped>
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