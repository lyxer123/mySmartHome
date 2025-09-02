import axios from 'axios';

const api = axios.create({
  baseURL: 'http://localhost:3000/api'
});

export default {
  login(credentials) {
    return api.post('/auth/login', credentials);
  },
  register(userData) {
    return api.post('/auth/register', userData);
  },
  getSensorData() {
    return api.get('/sensor-data/latest');
  }
}