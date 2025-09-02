export function isAuthenticated() {
  return !!localStorage.getItem('token');
}

export function getCurrentUser() {
  const token = localStorage.getItem('token');
  if (!token) return null;
  
  try {
    const payload = JSON.parse(atob(token.split('.')[1]));
    return {
      username: payload.username
    };
  } catch (error) {
    return null;
  }
}