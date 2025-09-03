#!/usr/bin/env python3
"""
SmartRelay MQTT测试工具
用于测试继电器控制功能和状态监控
"""

import paho.mqtt.client as mqtt
import json
import time
import argparse

class SmartRelayTester:
    def __init__(self, broker="broker.emqx.io", port=1883):
        self.broker = broker
        self.port = port
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.device_mac = None
        self.status_received = False
        
    def on_connect(self, client, userdata, flags, rc):
        print(f"连接到MQTT服务器: {self.broker}:{self.port}")
        if rc == 0:
            print("连接成功")
        else:
            print(f"连接失败，错误代码: {rc}")
            
    def on_message(self, client, userdata, msg):
        try:
            payload = msg.payload.decode()
            data = json.loads(payload)
            
            if "deviceId" in data:
                self.device_mac = data["deviceId"]
                print(f"发现设备: {self.device_mac}")
                print(f"设备类型: {data.get('deviceType', '未知')}")
                print(f"继电器数量: {data.get('numOutputs', 0)}")
                
                # 打印继电器状态
                for relay in data.get("relays", []):
                    state = "ON" if relay.get("state") else "OFF"
                    print(f"继电器 {relay.get('index')}: {state}")
                
                self.status_received = True
                
        except Exception as e:
            print(f"消息处理错误: {e}")
    
    def connect(self):
        self.client.connect(self.broker, self.port, 60)
        self.client.loop_start()
        
    def disconnect(self):
        self.client.loop_stop()
        self.client.disconnect()
        
    def discover_devices(self, timeout=10):
        """发现在线设备"""
        print("正在搜索设备...")
        self.client.subscribe("mySmartHome/relay/+/status")
        time.sleep(timeout)
        
    def control_relay(self, device_mac, relay_index, state):
        """控制指定设备的继电器"""
        topic = f"mySmartHome/relay/{device_mac}/control"
        message = json.dumps({
            "relay": relay_index,
            "state": state
        })
        
        self.client.publish(topic, message)
        print(f"发送控制命令: 设备 {device_mac}, 继电器 {relay_index}, 状态 {'ON' if state else 'OFF'}")
        
    def monitor_status(self, device_mac, duration=30):
        """监控设备状态"""
        topic = f"mySmartHome/relay/{device_mac}/status"
        self.client.subscribe(topic)
        print(f"开始监控设备 {device_mac}，持续时间: {duration}秒")
        time.sleep(duration)

def main():
    parser = argparse.ArgumentParser(description="SmartRelay MQTT测试工具")
    parser.add_argument("--broker", default="broker.emqx.io", help="MQTT服务器地址")
    parser.add_argument("--port", type=int, default=1883, help="MQTT服务器端口")
    
    subparsers = parser.add_subparsers(dest="command", help="命令")
    
    # 发现设备命令
    discover_parser = subparsers.add_parser("discover", help="发现在线设备")
    discover_parser.add_argument("--timeout", type=int, default=10, help="搜索超时时间(秒)")
    
    # 控制命令
    control_parser = subparsers.add_parser("control", help="控制继电器")
    control_parser.add_argument("device", help="设备MAC地址")
    control_parser.add_argument("relay", type=int, help="继电器索引(0-3)")
    control_parser.add_argument("state", choices=["on", "off"], help="开关状态")
    
    # 监控命令
    monitor_parser = subparsers.add_parser("monitor", help="监控设备状态")
    monitor_parser.add_argument("device", help="设备MAC地址")
    monitor_parser.add_argument("--duration", type=int, default=30, help="监控持续时间(秒)")
    
    args = parser.parse_args()
    
    tester = SmartRelayTester(args.broker, args.port)
    tester.connect()
    
    try:
        if args.command == "discover":
            tester.discover_devices(args.timeout)
            
        elif args.command == "control":
            state = args.state.lower() == "on"
            if not 0 <= args.relay <= 3:
                print("错误: 继电器索引必须在0-3之间")
                return
                
            tester.control_relay(args.device, args.relay, state)
            time.sleep(2)  # 等待状态更新
            
        elif args.command == "monitor":
            tester.monitor_status(args.device, args.duration)
            
        else:
            print("请指定命令: discover, control 或 monitor")
            
    except KeyboardInterrupt:
        print("\n测试中断")
    finally:
        tester.disconnect()

if __name__ == "__main__":
    main()