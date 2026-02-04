# src/payroll_db/app.py
from flask import Flask, jsonify
app = Flask(__name__)

@app.route('/')
def get_payroll_data():
    print("WARNING: Received a request for sensitive payroll data!")
    # In a real Zero Trust setup, this request should never even reach the app.
    return jsonify(error="ACCESS DENIED"), 403

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5004)