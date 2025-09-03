import sqlite3
from datetime import datetime
import json
import hashlib

def init_db():
    """初始化数据库表"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    
    # 创建传感器数据表
    c.execute('''CREATE TABLE IF NOT EXISTS sensor_data
                 (id INTEGER PRIMARY KEY AUTOINCREMENT,
                  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                  data TEXT)''')
    
    # 创建用户表
    c.execute('''CREATE TABLE IF NOT EXISTS users
                 (id INTEGER PRIMARY KEY AUTOINCREMENT,
                  username TEXT UNIQUE NOT NULL,
                  password TEXT NOT NULL,
                  email TEXT UNIQUE,
                  created_at DATETIME DEFAULT CURRENT_TIMESTAMP)''')
    
    conn.commit()
    conn.close()

def insert_data(data):
    """插入传感器数据"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    c.execute("INSERT INTO sensor_data (data) VALUES (?)", 
              (json.dumps(data),))
    conn.commit()
    conn.close()

def get_recent_data(limit=10):
    """获取最近的数据记录"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    c.execute("SELECT id, timestamp, data FROM sensor_data ORDER BY timestamp DESC LIMIT ?", 
              (limit,))
    
    rows = c.fetchall()
    result = []
    for row in rows:
        result.append({
            'id': row[0],
            'timestamp': row[1],
            'data': json.loads(row[2])
        })
    
    conn.close()
    return result

def hash_password(password):
    """对密码进行哈希处理"""
    return hashlib.sha256(password.encode()).hexdigest()

def register_user(username, password, email=None):
    """注册新用户"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    
    try:
        hashed_password = hash_password(password)
        c.execute("INSERT INTO users (username, password, email) VALUES (?, ?, ?)",
                 (username, hashed_password, email))
        conn.commit()
        conn.close()
        return True, "用户注册成功"
    except sqlite3.IntegrityError:
        conn.close()
        return False, "用户名或邮箱已存在"
    except Exception as e:
        conn.close()
        return False, str(e)

def verify_user(username, password):
    """验证用户登录"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    
    c.execute("SELECT id, username, password FROM users WHERE username = ?", (username,))
    user = c.fetchone()
    conn.close()
    
    if user and user[2] == hash_password(password):
        return True, {"id": user[0], "username": user[1]}
    
    return False, "用户名或密码错误"

def update_user_password(user_id, current_password, new_password):
    """更新用户密码"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    
    # 首先验证当前密码是否正确
    c.execute("SELECT password FROM users WHERE id = ?", (user_id,))
    user = c.fetchone()
    
    if not user:
        conn.close()
        return False, "用户不存在"
    
    if user[0] != hash_password(current_password):
        conn.close()
        return False, "当前密码不正确"
    
    # 更新密码
    try:
        hashed_new_password = hash_password(new_password)
        c.execute("UPDATE users SET password = ? WHERE id = ?", 
                 (hashed_new_password, user_id))
        conn.commit()
        conn.close()
        return True, "密码更新成功"
    except Exception as e:
        conn.rollback()
        conn.close()
        return False, str(e)