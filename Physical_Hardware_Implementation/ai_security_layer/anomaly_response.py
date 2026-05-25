import paho.mqtt.client as mqtt
import ssl

def on_message(client, userdata, msg):
    alert = msg.payload.decode('utf-8')
    if alert == "CRITICAL":
        print("Received CRITICAL AI alert. Deploying physical countermeasures.")
        client.publish("ot/level1/actuator/esp32-3/servo", "OPEN")

client = mqtt.Client()
client.tls_set(ca_certs="/etc/mosquitto/certs/ca.crt",
               certfile="/etc/mosquitto/certs/server.crt",
               keyfile="/etc/mosquitto/certs/server.key",
               tls_version=ssl.PROTOCOL_TLSv1_2)

client.on_message = on_message
client.connect("10.200.155.193", 8883, 60)
client.subscribe("ot/internal/ai/alerts")
client.loop_forever()
