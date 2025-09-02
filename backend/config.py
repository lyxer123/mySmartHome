# 后端Web服务器配置
WEB_SERVER_CONFIG = {
    'host': '0.0.0.0',      # 允许所有IP访问
    'port': 5000,           # FlaskWeb服务端口
    'debug': True           # 调试模式
}

# MQTT代理服务器配置
MQTT_CONFIG = {
    'broker_url': 'localhost',  # MQTT代理地址
    'port': 1883,               # MQTT标准端口
    'keepalive': 60,            # 心跳间隔
    'topic_subscribe': 'data/pub',   # 订阅主题
    'topic_publish': 'data/sub',     # 发布主题
    'username': None,           # 认证用户名(如有)
    'password': None           # 认证密码(如有)
}

# 数据库配置
DATABASE_CONFIG = {
    'database_path': 'app.db',  # SQLite数据库文件路径
    'table_name': 'sensor_data' # 数据表名
}