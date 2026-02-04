# src/robot_controller/app.py
import requests
import time
import os

# Get service URLs from environment variables for easy configuration in Kubernetes
DESIGN_DB_URL = os.getenv("DESIGN_DB_URL", "http://design-db-service:5003")
PAYROLL_DB_URL = os.getenv("PAYROLL_DB_URL", "http://payroll-db-service:5004")
POLL_INTERVAL = 5 # seconds

def fetch_data(service_name, url):
    """Tries to fetch data from a given URL and prints the outcome."""
    print(f"Attempting to connect to {service_name} at {url}...")
    try:
        response = requests.get(url, timeout=3)
        print(f"✅ SUCCESS: Connected to {service_name}. Status: {response.status_code}\n")
    except requests.exceptions.RequestException as e:
        print(f"❌ FAILED: Could not connect to {service_name}. Error: {e}\n")

if __name__ == "__main__":
    print("🤖 Robot Controller starting...")
    print("This robot will try to access the Design DB (Allowed) and Payroll DB (Blocked).")
    
    while True:
        # 1. Legitimate Request (This should be ALLOWED by our policy)
        fetch_data("Design DB", DESIGN_DB_URL)

        # 2. Simulated Attack (This should be BLOCKED by our policy)
        fetch_data("Payroll DB (Sensitive)", PAYROLL_DB_URL)

        print(f"--- Cycle complete. Waiting for {POLL_INTERVAL} seconds... ---")
        time.sleep(POLL_INTERVAL)