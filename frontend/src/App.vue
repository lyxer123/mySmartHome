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
      
      <div class="chart-container">
        <h2>历史数据</h2>
        <div class="chart-tabs">
          <button 
            :class="{ active: activeChart === 'temperature' }" 
            @click="activeChart = 'temperature'"
          >
            温度图表
          </button>
          <button 
            :class="{ active: activeChart === 'humidity' }" 
            @click="activeChart = 'humidity'"
          >
            湿度图表
          </button>
        </div>
        <div class="chart" ref="chartContainer"></div>
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
import * as echarts from 'echarts/lib/echarts';
import 'echarts/lib/chart/line';
import 'echarts/lib/component/tooltip';
import 'echarts/lib/component/title';
import 'echarts/lib/component/grid';
import 'echarts/lib/component/legend';

export default {
  name: 'App',
  data() {
    return {
      temperature: '--',
      humidity: '--',
      deviceId: '--',
      lastUpdate: '--',
      isConnected: false,
      activeChart: 'temperature',
      chartInstance: null,
      historicalData: [],
      pollingInterval: null
    };
  },
  mounted() {
    this.fetchLatestData();
    this.fetchHistoricalData();
    
    // 设置轮询间隔
    this.pollingInterval = setInterval(() => {
      this.fetchLatestData();
    }, 10000); // 每10秒更新一次
    
    // 每分钟更新历史数据
    setInterval(() => {
      this.fetchHistoricalData();
    }, 60000);
    
    // 监听窗口大小变化，调整图表大小
    window.addEventListener('resize', this.resizeChart);
  },
  beforeDestroy() {
    clearInterval(this.pollingInterval);
    window.removeEventListener('resize', this.resizeChart);
    if (this.chartInstance) {
      this.chartInstance.dispose();
    }
  },
  watch: {
    activeChart() {
      this.updateChart();
    },
    historicalData() {
      this.updateChart();
    }
  },
  methods: {
    async fetchLatestData() {
      try {
        const response = await fetch('http://localhost:3000/api/sensor-data/latest');
        if (response.ok) {
          const data = await response.json();
          this.temperature = data.temperature.toFixed(1);
          this.humidity = data.humidity.toFixed(1);
          this.deviceId = data.deviceId;
          this.lastUpdate = new Date(data.receivedAt).toLocaleString();
          this.isConnected = true;
        } else {
          console.error('获取最新数据失败');
          this.isConnected = false;
        }
      } catch (error) {
        console.error('API请求错误:', error);
        this.isConnected = false;
      }
    },
    async fetchHistoricalData() {
      try {
        const response = await fetch('http://localhost:3000/api/sensor-data');
        if (response.ok) {
          const data = await response.json();
          this.historicalData = data.readings;
          this.initChart();
        } else {
          console.error('获取历史数据失败');
        }
      } catch (error) {
        console.error('API请求错误:', error);
      }
    },
    initChart() {
      if (!this.$refs.chartContainer) return;
      
      if (!this.chartInstance) {
        this.chartInstance = echarts.init(this.$refs.chartContainer);
      }
      
      this.updateChart();
    },
    updateChart() {
      if (!this.chartInstance || this.historicalData.length === 0) return;
      
      const dataType = this.activeChart;
      const title = dataType === 'temperature' ? '温度历史' : '湿度历史';
      const yAxisName = dataType === 'temperature' ? '温度 (°C)' : '湿度 (%)';
      const color = dataType === 'temperature' ? '#ff6b6b' : '#4dabf7';
      
      const times = this.historicalData.map(item => {
        return new Date(item.receivedAt).toLocaleTimeString();
      });
      
      const values = this.historicalData.map(item => {
        return dataType === 'temperature' ? item.temperature : item.humidity;
      });
      
      const option = {
        title: {
          text: title,
          left: 'center'
        },
        tooltip: {
          trigger: 'axis'
        },
        xAxis: {
          type: 'category',
          data: times,
          axisLabel: {
            rotate: 45
          }
        },
        yAxis: {
          type: 'value',
          name: yAxisName
        },
        series: [
          {
            name: yAxisName,
            type: 'line',
            data: values,
            smooth: true,
            itemStyle: {
              color: color
            },
            areaStyle: {
              color: {
                type: 'linear',
                x: 0,
                y: 0,
                x2: 0,
                y2: 1,
                colorStops: [
                  {
                    offset: 0,
                    color: color
                  },
                  {
                    offset: 1,
                    color: 'rgba(255, 255, 255, 0.3)'
                  }
                ]
              }
            }
          }
        ],
        grid: {
          left: '3%',
          right: '4%',
          bottom: '15%',
          containLabel: true
        }
      };
      
      this.chartInstance.setOption(option);
    },
    resizeChart() {
      if (this.chartInstance) {
        this.chartInstance.resize();
      }
    }
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

.chart-container {
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  padding: 20px;
  margin-bottom: 30px;
}

.chart-container h2 {
  text-align: center;
  margin-bottom: 20px;
  color: #555;
}

.chart-tabs {
  display: flex;
  justify-content: center;
  margin-bottom: 20px;
}

.chart-tabs button {
  background-color: #f1f1f1;
  border: none;
  padding: 10px 20px;
  margin: 0 5px;
  cursor: pointer;
  border-radius: 5px;
  font-weight: bold;
  transition: all 0.3s ease;
}

.chart-tabs button.active {
  background-color: #3498db;
  color: white;
}

.chart {
  height: 400px;
  width: 100%;
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