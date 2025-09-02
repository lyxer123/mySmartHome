import paho.mqtt.client as mqtt
import json
from database import insert_data
from config import MQTT_CONFIG
from config import MQTT_CONFIG

# MQTT连接回调
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker")
        client.subscribe(MQTT_CONFIG['topic_subscribe'])
    else:
        print(f"Failed to connect, return code {rc}")

# MQTT消息接收回调
def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        data = json.loads(payload)
        print(f"Received message: {data}")
        
        # 存储到数据库
        insert_data(data)
    except Exception as e:
        print(f"Error processing message: {e}")

# 初始化MQTT客户端
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# 连接到代理服务器
client.connect(
    MQTT_CONFIG['broker_url'], 
    MQTT_CONFIG['port'], 
    MQTT_CONFIG['keepalive']
)