from prometheus_client import start_http_server, Gauge
import paho.mqtt.client as mqtt
import ssl

temp_gauge = Gauge('ot_temperature_celsius', 'Temperature', ['node'])
hum_gauge = Gauge('ot_humidity_percent', 'Humidity', ['node'])

def on_message(client, userdata, msg):
    topic = msg.topic
    try:
        payload = float(msg.payload.decode('utf-8'))
        parts = topic.split('/')
        if len(parts) >= 5:
            node = parts[3]
            sensor_type = parts[4]
            if sensor_type == 'temperature':
                temp_gauge.labels(node=node).set(payload)
            elif sensor_type == 'humidity':
                hum_gauge.labels(node=node).set(payload)
    except Exception as e:
        print(f"Error parsing message: {e}")

client = mqtt.Client()
client.tls_set(ca_certs="/etc/mosquitto/certs/ca.crt",
               certfile="/etc/mosquitto/certs/server.crt",
               keyfile="/etc/mosquitto/certs/server.key",
               tls_version=ssl.PROTOCOL_TLSv1_2)

client.on_message = on_message
client.connect("10.200.155.193", 8883, 60)
client.subscribe("ot/level1/sensor/#")

start_http_server(8000)
client.loop_forever()
