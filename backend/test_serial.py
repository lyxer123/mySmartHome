#!/usr/bin/env python3
"""
串口调试功能测试脚本
用于测试TempHum设备的串口调试功能
"""

import requests
import json
import time

def test_serial_debug():
    """测试串口调试API"""
    base_url = "http://10.1.95.252:5000"
    
    # 模拟登录获取token
    login_data = {
        "username": "admin",
        "password": "password123"
    }
    
    try:
        # 登录获取token
        response = requests.post(f"{base_url}/api/login", json=login_data)
        if response.status_code != 200:
            print("登录失败，请先创建用户")
            return False
            
        result = response.json()
        token = result.get('token')
        if not token:
            print("获取token失败")
            return False
            
        print(f"登录成功，token: {token[:20]}...")
        
        # 测试串口调试API
        debug_data = {
            "command": "AT+INFO"
        }
        
        headers = {
            "Content-Type": "application/json",
            "Authorization": token
        }
        
        response = requests.post(
            f"{base_url}/api/debug/serial", 
            json=debug_data, 
            headers=headers
        )
        
        if response.status_code == 200:
            result = response.json()
            print(f"串口调试测试成功: {result}")
            return True
        else:
            print(f"串口调试测试失败: HTTP {response.status_code}")
            print(response.text)
            return False
            
    except requests.exceptions.ConnectionError:
        print("无法连接到服务器，请确保服务器正在运行")
        return False
    except Exception as e:
        print(f"测试过程中出现错误: {e}")
        return False

if __name__ == "__main__":
    print("开始测试TempHum串口调试功能...")
    success = test_serial_debug()
    if success:
        print("✅ 串口调试功能测试通过")
    else:
        print("❌ 串口调试功能测试失败")