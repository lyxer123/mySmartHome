import paho.mqtt.client as mqtt
import json
import serial
import threading
import time
from database import insert_data
from config import MQTT_CONFIG

# 串口调试相关变量
serial_connection = None
serial_lock = threading.Lock()

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
        
        # 检查是否是串口调试消息
        if 'command' in data and data.get('type') == 'serial_debug':
            handle_serial_command(data['command'])
            
    except Exception as e:
        print(f"Error processing message: {e}")

# 处理串口命令
def handle_serial_command(command):
    """处理串口调试命令"""
    try:
        with serial_lock:
            if serial_connection and serial_connection.is_open:
                # 发送命令到串口
                serial_connection.write((command + '\r\n').encode())
                print(f"[SERIAL] Sent command: {command}")
                
                # 读取响应
                time.sleep(0.1)  # 等待响应
                response = serial_connection.read_all().decode().strip()
                if response:
                    print(f"[SERIAL] Response: {response}")
                    
                    # 发布响应到MQTT
                    response_data = {
                        'type': 'serial_response',
                        'command': command,
                        'response': response,
                        'timestamp': time.time()
                    }
                    client.publish('mySmartHome/debug/serial', json.dumps(response_data))
                    
    except Exception as e:
        print(f"[SERIAL] Error handling command: {e}")

# 初始化串口连接
def init_serial():
    """初始化串口连接"""
    global serial_connection
    try:
        # 尝试常见的串口设备
        ports = ['COM3', 'COM4', 'COM5', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyACM0']
        
        for port in ports:
            try:
                serial_connection = serial.Serial(
                    port=port,
                    baudrate=115200,
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    timeout=1
                )
                print(f"[SERIAL] Connected to {port}")
                return True
            except serial.SerialException:
                continue
                
        print("[SERIAL] No serial device found")
        return False
        
    except Exception as e:
        print(f"[SERIAL] Initialization error: {e}")
        return False

# 初始化MQTT客户端
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# 连接到代理服务器
client.connect(
    MQTT_CONFIG['broker_url'], 
    int(MQTT_CONFIG['port']), 
    int(MQTT_CONFIG['keepalive'])
)

# 初始化串口连接
init_serial()