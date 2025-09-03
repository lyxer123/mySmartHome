<template>
  <div class="serial-debug">
    <h3>TempHum 串口调试</h3>
    
    <div class="debug-controls">
      <div class="form-group">
        <label>串口命令</label>
        <input 
          type="text" 
          v-model="command" 
          placeholder="输入串口命令，如: AT+INFO"
          @keyup.enter="sendCommand"
        />
      </div>
      
      <button @click="sendCommand" :disabled="!command.trim()">
        {{ isSending ? '发送中...' : '发送命令' }}
      </button>
      
      <button @click="clearOutput" class="clear-btn">
        清空输出
      </button>
    </div>
    
    <div class="response-container">
      <h4>响应输出</h4>
      <div class="response-output">
        <pre>{{ responseOutput }}</pre>
      </div>
    </div>
    
    <div class="command-history">
      <h4>常用命令</h4>
      <div class="quick-commands">
        <button 
          v-for="(cmd, index) in quickCommands" 
          :key="index" 
          @click="useQuickCommand(cmd)"
          class="quick-cmd-btn"
        >
          {{ cmd.label }}
        </button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'SerialDebug',
  data() {
    return {
      command: '',
      responseOutput: '',
      isSending: false,
      quickCommands: [
        { label: '设备信息', command: 'AT+INFO' },
        { label: '重启设备', command: 'AT+RESTART' },
        { label: '读取温度', command: 'AT+TEMP' },
        { label: '读取湿度', command: 'AT+HUM' },
        { label: 'WiFi状态', command: 'AT+WIFI' },
        { label: 'MQTT状态', command: 'AT+MQTT' }
      ]
    }
  },
  methods: {
    async sendCommand() {
      if (!this.command.trim()) return;
      
      this.isSending = true;
      const cmd = this.command.trim();
      
      // 添加发送的命令到输出
      this.addToOutput(`> ${cmd}`);
      
      try {
        const token = localStorage.getItem('token');
        if (!token) {
          this.addToOutput('错误: 未登录');
          return;
        }
        
        const response = await fetch('/api/debug/serial', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'Authorization': token
          },
          body: JSON.stringify({
            command: cmd
          })
        });
        
        if (response.ok) {
          const result = await response.json();
          this.addToOutput(result.response);
        } else {
          this.addToOutput(`错误: HTTP ${response.status}`);
        }
      } catch (error) {
        console.error('Serial debug error:', error);
        this.addToOutput('错误: 无法连接到服务器');
      } finally {
        this.isSending = false;
        this.command = '';
      }
    },
    
    addToOutput(text) {
      const timestamp = new Date().toLocaleTimeString();
      this.responseOutput += `[${timestamp}] ${text}\n`;
      // 自动滚动到底部
      this.$nextTick(() => {
        const outputElement = this.$el.querySelector('.response-output');
        if (outputElement) {
          outputElement.scrollTop = outputElement.scrollHeight;
        }
      });
    },
    
    clearOutput() {
      this.responseOutput = '';
    },
    
    useQuickCommand(cmd) {
      this.command = cmd.command;
      this.sendCommand();
    }
  }
}
</script>

<style scoped>
.serial-debug {
  border: 1px solid #e0e0e0;
  border-radius: 8px;
  padding: 20px;
  margin: 20px 0;
  background: #fff;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.serial-debug h3 {
  margin: 0 0 20px 0;
  color: #2c3e50;
  border-bottom: 2px solid #42b983;
  padding-bottom: 10px;
}

.serial-debug h4 {
  margin: 15px 0 10px 0;
  color: #34495e;
}

.debug-controls {
  display: flex;
  gap: 10px;
  align-items: end;
  margin-bottom: 20px;
}

.form-group {
  flex: 1;
}

.form-group label {
  display: block;
  margin-bottom: 5px;
  font-weight: bold;
  color: #555;
}

.form-group input {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 14px;
}

.form-group input:focus {
  outline: none;
  border-color: #42b983;
  box-shadow: 0 0 0 2px rgba(66, 185, 131, 0.2);
}

button {
  padding: 10px 15px;
  background: #42b983;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
  white-space: nowrap;
}

button:hover:not(:disabled) {
  background: #3aa876;
}

button:disabled {
  background: #ccc;
  cursor: not-allowed;
}

.clear-btn {
  background: #f44336;
}

.clear-btn:hover {
  background: #d32f2f;
}

.response-container {
  margin-bottom: 20px;
}

.response-output {
  background: #f8f9fa;
  border: 1px solid #e9ecef;
  border-radius: 4px;
  padding: 15px;
  max-height: 300px;
  overflow-y: auto;
  font-family: 'Courier New', monospace;
  font-size: 13px;
  line-height: 1.4;
}

.response-output pre {
  margin: 0;
  white-space: pre-wrap;
  word-break: break-all;
}

.command-history {
  border-top: 1px solid #eee;
  padding-top: 15px;
}

.quick-commands {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.quick-cmd-btn {
  background: #2196f3;
  font-size: 12px;
  padding: 6px 12px;
}

.quick-cmd-btn:hover {
  background: #1976d2;
}

/* 响应式设计 */
@media (max-width: 768px) {
  .debug-controls {
    flex-direction: column;
    align-items: stretch;
  }
  
  .quick-commands {
    justify-content: center;
  }
}
</style>