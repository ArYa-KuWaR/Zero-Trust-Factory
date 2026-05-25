# AI Anomaly Detector — Integration Instructions

## Step 1: Install dependencies
pip3 install -r requirements.txt

## Step 2: Collect baseline data (10–30 min, normal conditions)
python3 data_collector.py

## Step 3: Train models
python3 train.py

## Step 4: Run detector
python3 detector.py

## Step 5: Register as service (optional)
sudo cp ai-detector.service /etc/systemd/system/
sudo systemctl enable ai-detector
sudo systemctl start ai-detector

## Notes
- BROKER_IP in detector.py and data_collector.py is set to 127.0.0.1
- Since this runs ON the Jetson (same machine as Mosquitto), no change needed
- Anomaly JSON publishes to: ot/level1/sensor/{node}/anomaly
- Servo triggers on HIGH or CRITICAL severity