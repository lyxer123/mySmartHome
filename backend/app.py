from flask import Flask, request, jsonify
from flask_cors import CORS
from mqtt_handler import client as mqtt_client
from database import get_recent_data, init_db
import config
import json

app = Flask(__name__)
CORS(app)  # 允许跨域请求

# 初始化数据库
init_db()

# 启动MQTT客户端循环
mqtt_client.loop_start()

@app.route('/api/data', methods=['GET'])
def get_data():
    """获取存储的数据API接口"""
    limit = request.args.get('limit', 10, type=int)
    data = get_recent_data(limit)
    return jsonify({'data': data})

@app.route('/api/publish', methods=['POST'])
def publish_data():
    """发布数据到MQTT主题API接口"""
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