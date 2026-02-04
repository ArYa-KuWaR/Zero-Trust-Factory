# src/iot_sensor/app.py
import requests
import time
import os
import random

DASHBOARD_URL = os.getenv("DASHBOARD_URL", "http://dashboard-service:5005")
POLL_INTERVAL = 7 # seconds

def send_telemetry():
    """Simulates sending a temperature reading to the dashboard."""
    temperature = round(random.uniform(25.0, 27.5), 2)
    payload = {"sensor_id": "temp_sensor_01", "temperature": temperature}
    
    print(f"Sending telemetry: {payload}")
    try:
        response = requests.post(DASHBOARD_URL, json=payload, timeout=3)
        print(f"✅ SUCCESS: Sent telemetry to Dashboard. Status: {response.status_code}\n")
    except requests.exceptions.RequestException as e:
        print(f"❌ FAILED: Could not send telemetry to Dashboard. Error: {e}\n")

if __name__ == "__main__":
    print("🌡️ IoT Sensor starting...")
    while True:
        send_telemetry()
        time.sleep(POLL_INTERVAL)