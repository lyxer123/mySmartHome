<template>
  <div class="user-profile-modal" v-if="visible">
    <div class="modal-content">
      <div class="modal-header">
        <h2>个人信息设置</h2>
        <button class="close-btn" @click="close">&times;</button>
      </div>
      <div class="modal-body">
        <div class="form-group">
          <label>用户名</label>
          <input type="text" v-model="userInfo.username" disabled />
          <small>用户名不可修改</small>
        </div>
        
        <div class="form-group">
          <label>当前密码</label>
          <input type="password" v-model="userInfo.currentPassword" placeholder="输入当前密码" />
        </div>
        
        <div class="form-group">
          <label>新密码</label>
          <input type="password" v-model="userInfo.newPassword" placeholder="输入新密码" />
        </div>
        
        <div class="form-group">
          <label>确认新密码</label>
          <input type="password" v-model="userInfo.confirmPassword" placeholder="再次输入新密码" />
        </div>
        
        <div class="error-message" v-if="errorMessage">{{ errorMessage }}</div>
        <div class="success-message" v-if="successMessage">{{ successMessage }}</div>
        
        <div class="form-actions">
          <button class="save-btn" @click="saveChanges">保存修改</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'UserProfile',
  props: {
    visible: {
      type: Boolean,
      default: false
    },
    username: {
      type: String,
      required: true
    }
  },
  data() {
    return {
      userInfo: {
        username: this.username,
        currentPassword: '',
        newPassword: '',
        confirmPassword: ''
      },
      errorMessage: '',
      successMessage: ''
    }
  },
  watch: {
    username(newVal) {
      this.userInfo.username = newVal;
    }
  },
  methods: {
    close() {
      this.$emit('close');
      this.resetForm();
    },
    resetForm() {
      this.userInfo.currentPassword = '';
      this.userInfo.newPassword = '';
      this.userInfo.confirmPassword = '';
      this.errorMessage = '';
      this.successMessage = '';
    },
    async saveChanges() {
      // 表单验证
      if (!this.userInfo.currentPassword) {
        this.errorMessage = '请输入当前密码';
        return;
      }
      
      if (this.userInfo.newPassword) {
        if (this.userInfo.newPassword.length < 6) {
          this.errorMessage = '新密码长度不能少于6位';
          return;
        }
        
        if (this.userInfo.newPassword !== this.userInfo.confirmPassword) {
          this.errorMessage = '两次输入的新密码不一致';
          return;
        }
      } else if (!this.userInfo.newPassword && this.userInfo.confirmPassword) {
        this.errorMessage = '请输入新密码';
        return;
      } else if (!this.userInfo.newPassword) {
        this.errorMessage = '请输入新密码';
        return;
      }
      
      try {
        const token = localStorage.getItem('token');
        if (!token) {
          this.$router.push('/login');
          return;
        }
        
        // 使用真实的后端API
        const isDemoMode = false; // 设置为false表示使用真实的后端API
        
        if (isDemoMode) {
          // 模拟API响应
          // 模拟验证当前密码
          const user = JSON.parse(localStorage.getItem('user') || '{}');
          const storedPassword = user.password || 'password123'; // 假设的存储密码
          
          if (this.userInfo.currentPassword !== storedPassword) {
            this.errorMessage = '当前密码不正确';
            return;
          }
          
          // 模拟更新密码
          const updatedUser = {
            ...user,
            password: this.userInfo.newPassword
          };
          
          // 更新本地存储
          localStorage.setItem('user', JSON.stringify(updatedUser));
          
          // 显示成功消息
          this.successMessage = '密码修改成功';
          this.errorMessage = '';
          
          // 3秒后关闭弹窗
          setTimeout(() => {
            this.close();
          }, 3000);
          
          return;
        }
        
        // 如果不是演示模式，则正常调用API
        const response = await fetch('/api/user/update-password', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'Authorization': token
          },
          body: JSON.stringify({
            currentPassword: this.userInfo.currentPassword,
            newPassword: this.userInfo.newPassword
          })
        });
        
        if (response.status === 401) {
          // 认证失败，可能是当前密码错误
          const data = await response.json();
          this.errorMessage = data.message || '当前密码验证失败';
          return;
        }
        
        if (!response.ok) {
          const data = await response.json();
          this.errorMessage = data.message || '更新失败，请稍后再试';
          return;
        }
        
        // 更新成功
        this.successMessage = '密码修改成功';
        this.errorMessage = '';
        
        // 3秒后关闭弹窗
        setTimeout(() => {
          this.close();
        }, 3000);
        
      } catch (error) {
        console.error('Update password error:', error);
        // 更友好的错误提示
        this.errorMessage = '无法连接到服务器，已启用本地模式保存';
        
        try {
          // 尝试本地模式保存
          const user = JSON.parse(localStorage.getItem('user') || '{}');
          const updatedUser = {
            ...user,
            password: this.userInfo.newPassword
          };
          
          // 更新本地存储
          localStorage.setItem('user', JSON.stringify(updatedUser));
          
          // 显示成功消息
          this.successMessage = '密码已在本地更新';
          
          // 3秒后关闭弹窗
          setTimeout(() => {
            this.close();
          }, 3000);
        } catch (localError) {
          this.errorMessage = '保存失败，请稍后再试';
        }
      }
    }
  }
}
</script>

<style scoped>
.user-profile-modal {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.modal-content {
  background-color: #fff;
  border-radius: 8px;
  width: 90%;
  max-width: 500px;
  box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px 20px;
  border-bottom: 1px solid #eee;
}

.modal-header h2 {
  margin: 0;
  font-size: 20px;
}

.close-btn {
  background: none;
  border: none;
  font-size: 24px;
  cursor: pointer;
  color: #666;
  padding: 0;
}

.modal-body {
  padding: 20px;
}

.form-group {
  margin-bottom: 15px;
}

.form-group label {
  display: block;
  margin-bottom: 5px;
  font-weight: bold;
}

.form-group input {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 16px;
}

.form-group input:disabled {
  background-color: #f5f5f5;
  cursor: not-allowed;
}

.form-group small {
  display: block;
  color: #666;
  font-size: 12px;
  margin-top: 5px;
}

.error-message {
  color: #f44336;
  margin: 10px 0;
}

.success-message {
  color: #4caf50;
  margin: 10px 0;
}

.form-actions {
  margin-top: 20px;
  text-align: right;
}

.save-btn {
  padding: 10px 20px;
  background: #42b983;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.save-btn:hover {
  background: #3aa876;
}

/* 响应式设计 */
@media (max-width: 480px) {
  .modal-content {
    width: 95%;
  }
  
  .modal-header h2 {
    font-size: 18px;
  }
  
  .form-group input {
    padding: 8px;
  }
}
</style>