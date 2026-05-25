"""
data_collector.py
-----------------
Run this FIRST on the Jetson to collect 10-30 min of 
normal baseline data before training.

Usage:
    python3 data_collector.py
    
    Let it run for at least 10 minutes under NORMAL conditions.
    Hit Ctrl+C when done. CSV will be saved to data/baseline.csv
"""

import paho.mqtt.client as mqtt
import csv, time, os

CSV_PATH = "data/baseline.csv"

latest = {
    "esp32-1": {"temperature": None, "humidity": None},
    "esp32-2": {"temperature": None, "humidity": None}
}

row_count = 0

def on_message(client, userdata, msg):
    global row_count
    topic = msg.topic

    try:
        value = float(msg.payload.decode())
    except ValueError:
        return

    node  = "esp32-1" if "esp32-1" in topic else "esp32-2"
    field = "temperature" if "temperature" in topic else "humidity"
    latest[node][field] = value

    # Write a row only when all 4 values are available
    e1 = latest["esp32-1"]
    e2 = latest["esp32-2"]

    if None in e1.values() or None in e2.values():
        return

    row = {
        "timestamp": int(time.time()),
        "t1": round(e1["temperature"], 2),
        "h1": round(e1["humidity"], 2),
        "t2": round(e2["temperature"], 2),
        "h2": round(e2["humidity"], 2)
    }

    write_header = not os.path.exists(CSV_PATH)
    os.makedirs("data", exist_ok=True)

    with open(CSV_PATH, "a") as f:
        writer = csv.DictWriter(f, fieldnames=row.keys())
        if write_header:
            writer.writeheader()
        writer.writerow(row)

    row_count += 1
    print("[Row {}] {}".format(row_count, row))


# ── Change IP to 127.0.0.1 for local test, Jetson IP for deployment ──
BROKER_IP = "127.0.0.1"

client = mqtt.Client(client_id="data-collector")
client.on_message = on_message
client.connect(BROKER_IP, 1883)
client.subscribe("ot/level1/sensor/#")

print("Collecting baseline data...")
print("Run for 10-30 min under NORMAL conditions then Ctrl+C\n")

try:
    client.loop_forever()
except KeyboardInterrupt:
    print("\nDone. {} rows saved to {}".format(row_count, CSV_PATH))