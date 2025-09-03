from flask import Flask, request, jsonify
from flask_cors import CORS
from mqtt_handler import client as mqtt_client
from database import get_recent_data, init_db, register_user, verify_user, update_user_password
import config
import json
from functools import wraps

app = Flask(__name__)
CORS(app)  # 允许跨域请求

# 初始化数据库
init_db()

# 启动MQTT客户端循环
mqtt_client.loop_start()

# 简单的令牌存储（实际生产环境应使用更安全的方式，如JWT）
active_tokens = {}

def token_required(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        token = request.headers.get('Authorization')
        
        if not token:
            return jsonify({'status': 'error', 'message': '缺少认证令牌'}), 401
            
        if token not in active_tokens:
            return jsonify({'status': 'error', 'message': '无效或过期的令牌'}), 401
            
        # 将用户信息添加到请求中
        request.user = active_tokens[token]
        return f(*args, **kwargs)
    return decorated

@app.route('/api/register', methods=['POST'])
def register():
    """用户注册API接口"""
    try:
        data = request.get_json()
        username = data.get('username')
        password = data.get('password')
        email = data.get('email')
        
        if not username or not password:
            return jsonify({'status': 'error', 'message': '用户名和密码不能为空'}), 400
            
        success, message = register_user(username, password, email)
        
        if success:
            return jsonify({'status': 'success', 'message': message})
        else:
            return jsonify({'status': 'error', 'message': message}), 400
            
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/login', methods=['POST'])
def login():
    """用户登录API接口"""
    try:
        data = request.get_json()
        username = data.get('username')
        password = data.get('password')
        
        if not username or not password:
            return jsonify({'status': 'error', 'message': '用户名和密码不能为空'}), 400
            
        success, result = verify_user(username, password)
        
        if success:
            # 生成简单的令牌（实际应使用JWT）
            import uuid
            token = str(uuid.uuid4())
            active_tokens[token] = result
            
            return jsonify({
                'status': 'success', 
                'message': '登录成功',
                'token': token,
                'user': {
                    'id': result['id'],
                    'username': result['username']
                }
            })
        else:
            return jsonify({'status': 'error', 'message': result}), 401
            
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/logout', methods=['POST'])
@token_required
def logout():
    """用户登出API接口"""
    token = request.headers.get('Authorization')
    if token in active_tokens:
        del active_tokens[token]
    return jsonify({'status': 'success', 'message': '登出成功'})

@app.route('/api/data', methods=['GET'])
@token_required
def get_data():
    """获取存储的数据API接口（需要认证）"""
    limit = request.args.get('limit', 10, type=int)
    data = get_recent_data(limit)
    return jsonify({'data': data})

@app.route('/api/user/update-password', methods=['POST'])
@token_required
def update_password():
    """更新用户密码API接口（需要认证）"""
    try:
        data = request.get_json()
        current_password = data.get('currentPassword')
        new_password = data.get('newPassword')
        
        if not current_password or not new_password:
            return jsonify({'status': 'error', 'message': '当前密码和新密码不能为空'}), 400
            
        # 获取当前用户ID
        user_id = request.user['id']
        
        # 更新密码
        success, message = update_user_password(user_id, current_password, new_password)
        
        if success:
            return jsonify({'status': 'success', 'message': message})
        else:
            return jsonify({'status': 'error', 'message': message}), 401
            
    except Exception as e:
        print(f"[DEBUG] Error updating password: {str(e)}")  # 添加异常日志
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/publish', methods=['POST'])
@token_required
def publish_data():
    """发布数据到MQTT主题API接口（需要认证）"""
    try:
        data = request.get_json()
        topic = config.MQTT_CONFIG['topic_publish']
        
        print(f"[DEBUG] Received data from frontend: {data}")  # 添加调试日志
        
        # 发布消息到MQTT
        result = mqtt_client.publish(topic, json.dumps(data))
        
        if result[0] == 0:
            print("[DEBUG] Data successfully published to MQTT")  # 添加成功日志
            return jsonify({'status': 'success', 'message': 'Data published'})
        else:
            print(f"[DEBUG] Failed to publish data to MQTT, return code: {result[0]}")  # 添加失败日志
            return jsonify({'status': 'error', 'message': 'Failed to publish'}), 500
            
    except Exception as e:
        print(f"[DEBUG] Error processing publish request: {str(e)}")  # 添加异常日志
        return jsonify({'status': 'error', 'message': str(e)}), 500

if __name__ == '__main__':
    app.run(
        host=config.WEB_SERVER_CONFIG['host'],
        port=config.WEB_SERVER_CONFIG['port'],
        debug=config.WEB_SERVER_CONFIG['debug']
    )