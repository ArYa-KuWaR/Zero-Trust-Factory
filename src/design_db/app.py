# src/design_db/app.py
from flask import Flask, jsonify
app = Flask(__name__)

@app.route('/')
def get_design():
    print("Received a valid request for design files.")
    return jsonify(message="Design file data for Robot Arm #1234")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5003)