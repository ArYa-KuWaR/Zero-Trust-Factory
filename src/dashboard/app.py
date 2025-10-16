# src/dashboard/app.py
from flask import Flask, request, jsonify
app = Flask(__name__)

@app.route('/', methods=['POST'])
def receive_telemetry():
    data = request.json
    print(f"Received telemetry data from sensor: {data}")
    return jsonify(status="Data received"), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5005)