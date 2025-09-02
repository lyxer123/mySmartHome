import sqlite3
from datetime import datetime
import json

def init_db():
    """初始化数据库表"""
    conn = sqlite3.connect('app.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS sensor_data
                 (id INTEGER PRIMARY KEY AUTOINCREMENT,
                  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                  data TEXT)''')
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