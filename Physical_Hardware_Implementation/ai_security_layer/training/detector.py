"""
detector.py
-----------
Main AI anomaly detection loop.
Subscribes to sensor MQTT topics, runs Isolation Forest +
rate-of-change detection, publishes anomaly scores, and
triggers servo on HIGH/CRITICAL anomalies.

Usage:
    python3 detector.py

NOTE for Shardul: Change BROKER_IP to Jetson's IP or 127.0.0.1
"""

import paho.mqtt.client as mqtt
import pickle, json, time, collections, os
import numpy as np

# ── Config ────────────────────────────────────────────────
BROKER_IP   = "127.0.0.1"   # Change to Jetson IP on deployment
BROKER_PORT = 1883
LOG_PATH    = "logs/anomaly_log.json"
WINDOW_SIZE = 20             # Sliding window — last 20 readings

os.makedirs("logs", exist_ok=True)

# ── Load models ───────────────────────────────────────────
print("Loading models...")
with open("models/isolation_forest.pkl", "rb") as f:
    iso_forest = pickle.load(f)

with open("models/scaler.pkl", "rb") as f:
    scaler = pickle.load(f)

with open("models/baseline_stats.json", "r") as f:
    baseline = json.load(f)

print("Models loaded OK")

# ── State ─────────────────────────────────────────────────
latest = {
    "esp32-1": {"temperature": None, "humidity": None},
    "esp32-2": {"temperature": None, "humidity": None}
}

windows = {
    "esp32-1": collections.deque(maxlen=WINDOW_SIZE),
    "esp32-2": collections.deque(maxlen=WINDOW_SIZE)
}

servo_open = False


# ── Scoring logic ─────────────────────────────────────────
def compute_anomaly(node):
    e1 = latest["esp32-1"]
    e2 = latest["esp32-2"]

    if None in e1.values() or None in e2.values():
        return None, "Waiting for all sensors", "LOW", 0

    t1, h1 = e1["temperature"], e1["humidity"]
    t2, h2 = e2["temperature"], e2["humidity"]

    # -- Isolation Forest score --
    raw = np.array([[t1, h1, t2, h2]])
    scaled = scaler.transform(raw)
    decision = iso_forest.decision_function(scaled)[0]
    # Map decision score to 0.0–1.0 (more negative = more anomalous)
    iso_score = float(np.clip(0.5 - decision, 0.0, 1.0))

    # -- Rate-of-change on the triggering node --
    win = windows[node]
    roc_score = 0.0
    reason = "Normal"
    prediction_horizon = 0

    if len(win) >= 3:
        temps = [r["temperature"] for r in win]
        # Change over last 3 readings (~30 seconds)
        dt_per_sec = (temps[-1] - temps[-3]) / 30.0
        dt_per_min = dt_per_sec * 60.0

        t_critical = baseline["{}_critical".format(
            "t1" if node == "esp32-1" else "t2"
        )]
        t_current = temps[-1]

        if dt_per_min > 1.5:
            headroom = t_critical - t_current
            if headroom > 0:
                mins_to_critical = headroom / dt_per_min
                prediction_horizon = int(mins_to_critical * 60)
                reason = "Temp rising {:.1f}C/min — critical in {:.0f} min".format(
                    dt_per_min, mins_to_critical
                )
                roc_score = min(1.0, dt_per_min / 5.0)
            else:
                reason = "Temperature already above critical threshold"
                roc_score = 1.0

        elif dt_per_min < -1.5:
            reason = "Rapid temperature drop — {:.1f}C/min".format(dt_per_min)
            roc_score = 0.5

    # -- Combined score (50/50 weighted) --
    final_score = round(0.5 * iso_score + 0.5 * roc_score, 3)

    # -- Severity --
    if final_score >= 0.85:
        severity = "CRITICAL"
    elif final_score >= 0.65:
        severity = "HIGH"
    elif final_score >= 0.4:
        severity = "MEDIUM"
    else:
        severity = "LOW"
        reason = "Normal"

    return final_score, reason, severity, prediction_horizon


def log_event(node, score, severity, reason):
    entry = {
        "timestamp": int(time.time()),
        "node": node,
        "score": score,
        "severity": severity,
        "reason": reason
    }
    try:
        with open(LOG_PATH, "r") as f:
            logs = json.load(f)
    except (IOError, ValueError):
        logs = []

    logs.append(entry)
    with open(LOG_PATH, "w") as f:
        json.dump(logs, f, indent=2)


# ── MQTT ──────────────────────────────────────────────────
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe("ot/level1/sensor/#")
    else:
        print("Connection failed — code:", rc)


def on_message(client, userdata, msg):
    global servo_open
    topic = msg.topic

    try:
        value = float(msg.payload.decode())
    except ValueError:
        return

    node  = "esp32-1" if "esp32-1" in topic else "esp32-2"
    field = "temperature" if "temperature" in topic else "humidity"

    latest[node][field] = value

    if latest[node]["temperature"] is not None and latest[node]["humidity"] is not None:
        windows[node].append({
            "temperature": latest[node]["temperature"],
            "humidity":    latest[node]["humidity"]
        })

    # Only run detection on temperature updates
    if field != "temperature":
        return

    score, reason, severity, horizon = compute_anomaly(node)
    if score is None:
        return

    anomaly = score >= 0.4

    payload = json.dumps({
        "node": node,
        "score": score,
        "anomaly": anomaly,
        "reason": reason,
        "prediction_horizon": horizon,
        "severity": severity,
        "timestamp": int(time.time())
    })

    client.publish("ot/level1/sensor/{}/anomaly".format(node), payload)
    print("[{}] Score={} | {} | {}".format(node, score, severity, reason))

    # -- Servo trigger --
    if anomaly and severity in ("HIGH", "CRITICAL"):
        if not servo_open:
            client.publish("ot/level1/actuator/esp32-3/servo", "OPEN")
            servo_open = True
            print(">>> SERVO OPENED")
            log_event(node, score, severity, reason)

    elif severity == "LOW" and servo_open:
        client.publish("ot/level1/actuator/esp32-3/servo", "CLOSE")
        servo_open = False
        print(">>> SERVO CLOSED")


client = mqtt.Client(client_id="ai-anomaly-detector")
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER_IP, BROKER_PORT)

print("AI Anomaly Detector starting...")
client.loop_forever()