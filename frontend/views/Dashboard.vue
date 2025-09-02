<template>
  <div class="dashboard">
    <header class="dashboard-header">
      <h1>IoT数据监控系统</h1>
      <div class="user-info">
        <span>欢迎，{{ username }}</span>
        <button @click="logout" class="logout-btn">退出登录</button>
      </div>
    </header>
    
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
  name: 'DashboardView',
  data() {
    return {
      username: '',
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
    // 从localStorage获取用户信息
    const userStr = localStorage.getItem('user');
    if (userStr) {
      try {
        const user = JSON.parse(userStr);
        this.username = user.username;
      } catch (e) {
        console.error('Failed to parse user data');
      }
    }
    
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
      
      // 响应式调整图表大小
      window.addEventListener('resize', () => {
        this.chart.resize();
      });
    },
    
    async loadData() {
      try {
        const token = localStorage.getItem('token');
        if (!token) {
          this.$router.push('/login');
          return;
        }
        
        const response = await fetch('/api/data?limit=10', {
          headers: {
            'Authorization': token
          }
        });
        
        if (response.status === 401) {
          // 认证失败，重定向到登录页
          localStorage.removeItem('token');
          localStorage.removeItem('user');
          this.$router.push('/login');
          return;
        }
        
        const result = await response.json();
        this.sensorData = result.data;
        
        // 更新图表数据
        this.chartData.timestamps = this.sensorData.map(item => item.timestamp);
        this.chartData.values = this.sensorData.map(item => item.data.value);
        
        if (this.chart) {
          this.updateChart();
        }
      } catch (error) {
        console.error('Failed to load data:', error);
      }
    },
    
    async publishData() {
      try {
        const token = localStorage.getItem('token');
        if (!token) {
          this.$router.push('/login');
          return;
        }
        
        const payload = JSON.parse(this.publishPayload);
        const response = await fetch('/api/publish', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'Authorization': token
          },
          body: JSON.stringify(payload)
        });
        
        if (response.status === 401) {
          // 认证失败，重定向到登录页
          localStorage.removeItem('token');
          localStorage.removeItem('user');
          this.$router.push('/login');
          return;
        }
        
        if (response.ok) {
          alert('数据发布成功!');
          this.publishPayload = '{"sensor": "temperature", "value": 25.5}';
        } else {
          alert('数据发布失败!');
        }
      } catch (error) {
        alert('数据格式错误!');
        console.error('Publish error:', error);
      }
    },
    
    async logout() {
      try {
        const token = localStorage.getItem('token');
        if (token) {
          await fetch('/api/logout', {
            method: 'POST',
            headers: {
              'Authorization': token
            }
          });
        }
      } catch (error) {
        console.error('Logout error:', error);
      } finally {
        // 无论如何，清除本地存储并重定向
        localStorage.removeItem('token');
        localStorage.removeItem('user');
        this.$router.push('/login');
      }
    }
  }
}
</script>

<style scoped>
.dashboard {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.dashboard-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 1px solid #eee;
}

.dashboard-header h1 {
  margin: 0;
  font-size: 24px;
}

.user-info {
  display: flex;
  align-items: center;
  gap: 15px;
}

.logout-btn {
  padding: 8px 12px;
  background: #f44336;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
}

.logout-btn:hover {
  background: #d32f2f;
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

/* 响应式设计 */
@media (max-width: 768px) {
  .dashboard-header {
    flex-direction: column;
    align-items: flex-start;
    gap: 10px;
  }
  
  .user-info {
    width: 100%;
    justify-content: space-between;
  }
  
  .chart-container {
    padding: 10px;
  }
}

@media (max-width: 480px) {
  .dashboard {
    padding: 10px;
  }
  
  .dashboard-header h1 {
    font-size: 20px;
  }
  
  button, .logout-btn {
    padding: 8px 12px;
    font-size: 14px;
  }
}
</style>