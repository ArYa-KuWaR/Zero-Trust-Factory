import paho.mqtt.client as mqtt
import ssl
import json
import numpy as np
from sklearn.ensemble import IsolationForest


X_train = np.array([[30.0, 45.0], [31.0, 46.0], [29.5, 44.0], [30.5, 45.5]])
model = IsolationForest(contamination=0.1)
model.fit(X_train)

latest_temp = 30.0
latest_hum = 45.0

def on_message(client, userdata, msg):
    global latest_temp, latest_hum
    topic = msg.topic
    payload = float(msg.payload.decode('utf-8'))
    
    if "temperature" in topic:
        latest_temp = payload
    elif "humidity" in topic:
        latest_hum = payload

 
    data_point = np.array([[latest_temp, latest_hum]])
    prediction = model.predict(data_point)
    score = model.score_samples(data_point)[0]
    
    severity = "low"
    if score < -0.60:
        severity = "High"
    
    print(f"AI Export -> esp32-1: Score={score:.2f}, Severity={severity}")
    
 
    if severity == "High":
        client.publish("ot/internal/ai/alerts", "CRITICAL")

client = mqtt.Client()
client.tls_set(ca_certs="/etc/mosquitto/certs/ca.crt",
               certfile="/etc/mosquitto/certs/server.crt",
               keyfile="/etc/mosquitto/certs/server.key",
               tls_version=ssl.PROTOCOL_TLSv1_2)

client.on_message = on_message
client.connect("10.200.155.193", 8883, 60)
client.subscribe("ot/level1/sensor/#")
client.loop_forever()
