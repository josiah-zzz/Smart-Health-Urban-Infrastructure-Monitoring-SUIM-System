from flask import Flask, request, jsonify
from flask_cors import CORS
from datetime import datetime
import csv
import os

app = Flask(__name__)
CORS(app)

DATA_FILE = '/home/pi/sensor_data.csv'

# Initialize CSV file if it doesn't exist
if not os.path.exists(DATA_FILE):
    with open(DATA_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['node_id', 'temperature', 'humidity', 'pressure', 'no2', 'pm25', 'pm10', 'co', 'o3', 'nh3', 'so2', 'timestamp'])

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    with open(DATA_FILE, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            data.get('node_id'),
            data.get('temperature'),
            data.get('humidity'),
            data.get('pressure'),
            data.get('no2'),
            data.get('pm25'),
            data.get('pm10'),
            data.get('co'),
            data.get('o3'),
            data.get('nh3'),
            data.get('so2'),
            datetime.now().isoformat()
        ])
    return jsonify({'status': 'success'}), 200

@app.route('/data/latest', methods=['GET'])
def get_latest():
    readings = []
    if os.path.exists(DATA_FILE):
        with open(DATA_FILE, 'r') as f:
            reader = csv.DictReader(f)
            rows = list(reader)
            
            # Get last reading for each node
            for node_id in ['Node1', 'Node2', 'Node3']:
                node_readings = [r for r in rows if r['node_id'] == node_id]
                if node_readings:
                    readings.append(node_readings[-1])
    
    return jsonify(readings), 200

@app.route('/health', methods=['GET'])
def health():
    return jsonify({'status': 'healthy'}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)