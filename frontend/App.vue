<template>
  <div id="app">
    <h1>IoT数据监控系统</h1>
    
    <div class="chart-container">
      <div ref="chart" style="width: 100%; height: 400px;"></div>
    </div>
    
    <div class="data-container">
      <h2>最新传感器数据</h2>
      <div v-for="item in sensorData" :key="item.id" class="data-item">
        <p>时间: {{ item.timestamp }}</p>
        <pre>{{ JSON.stringify(item.data, null, 2) }}</pre>
      </div>
    </div>

    <div class="publish-form">
      <h2>发布数据</h2>
      <form @submit.prevent="publishData">
        <textarea v-model="publishPayload" placeholder='{"sensor": "temperature", "value": 25.5}'></textarea>
        <button type="submit">发布</button>
      </form>
    </div>
  </div>
</template>

<script>
import * as echarts from 'echarts';

export default {
  name: 'App',
  data() {
    return {
      sensorData: [],
      publishPayload: '{"sensor": "temperature", "value": 25.5}',
      refreshInterval: null,
      chart: null,
      chartData: {
        timestamps: [],
        values: []
      }
    }
  },
  mounted() {
    this.initChart();
    this.loadData();
    // 每5秒刷新一次数据
    this.refreshInterval = setInterval(this.loadData, 5000);
  },
  beforeUnmount() {
    clearInterval(this.refreshInterval);
    if (this.chart) {
      this.chart.dispose();
    }
  },
  watch: {
    sensorData() {
      if (this.chart) {
        this.updateChart();
      }
    }
  },
  methods: {
    initChart() {
      this.chart = echarts.init(this.$refs.chart);
      this.updateChart();
    },
    
    updateChart() {
      const option = {
        title: {
          text: '温度变化曲线'
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: ['温度值']
        },
        xAxis: {
          type: 'category',
          data: this.chartData.timestamps,
          name: '时间'
        },
        yAxis: {
          type: 'value',
          name: '温度值(℃)'
        },
        series: [{
          name: '温度值',
          type: 'line',
          data: this.chartData.values,
          smooth: true
        }]
      };
      this.chart.setOption(option);
    },
    
    async loadData() {
      try {
        const response = await fetch('/api/data?limit=10')
        const result = await response.json()
        this.sensorData = result.data
        
        // 更新图表数据
        this.chartData.timestamps = this.sensorData.map(item => item.timestamp);
        this.chartData.values = this.sensorData.map(item => item.data.value);
        
        if (this.chart) {
          this.updateChart();
        }
      } catch (error) {
        console.error('Failed to load data:', error)
      }
    },
    async publishData() {
      try {
        const payload = JSON.parse(this.publishPayload)
        const response = await fetch('/api/publish', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(payload)
        })
        
        if (response.ok) {
          alert('数据发布成功!')
          this.publishPayload = '{"sensor": "temperature", "value": 25.5}'
        } else {
          alert('数据发布失败!')
        }
      } catch (error) {
        alert('数据格式错误!')
        console.error('Publish error:', error)
      }
    }
  }
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.chart-container {
  margin: 20px 0;
  border: 1px solid #eee;
  border-radius: 8px;
  padding: 20px;
  background: #fff;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.data-container {
  margin: 20px 0;
}

.data-item {
  margin: 10px 0;
  padding: 10px;
  border: 1px solid #eee;
  border-radius: 4px;
  text-align: left;
}

.publish-form {
  margin: 20px 0;
}

textarea {
  width: 100%;
  height: 100px;
  margin: 10px 0;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

button {
  padding: 10px 20px;
  background: #42b983;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background: #3aa876;
}
</style>