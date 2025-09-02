<template>
  <div id="app">
    <header v-if="$route.path !== '/login' && $route.path !== '/register'">
      <h1>智能家居温湿度监测系统</h1>
      <div class="user-panel" v-if="isAuthenticated">
        <span>欢迎, {{ currentUser.username }}</span>
        <button @click="logout">退出</button>
      </div>
    </header>
    
    <main>
      <router-view/>
    </main>
  </div>
</template>

<script>
export default {
  data() {
    return {
      isAuthenticated: false,
      currentUser: {
        username: ''
      }
    }
  },
  methods: {
    logout() {
      localStorage.removeItem('token');
      this.isAuthenticated = false;
      this.currentUser = { username: '' };
      this.$router.push('/login');
    },
    checkAuth() {
      const token = localStorage.getItem('token');
      if (token) {
        this.isAuthenticated = true;
        const payload = JSON.parse(atob(token.split('.')[1]));
        this.currentUser.username = payload.username;
      }
    }
  },
  created() {
    this.checkAuth();
  },
  watch: {
    $route() {
      this.checkAuth();
    }
  }
}
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
  position: relative;
}

header h1 {
  color: #2c3e50;
  font-size: 2.5rem;
}

.user-panel {
  position: absolute;
  right: 20px;
  top: 20px;
  display: flex;
  align-items: center;
  gap: 10px;
}

.user-panel button {
  padding: 5px 10px;
  background-color: #e74c3c;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.user-panel button:hover {
  background-color: #c0392b;
}
</style>