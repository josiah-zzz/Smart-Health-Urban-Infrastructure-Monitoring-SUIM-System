from flask import Flask, request, jsonify
from flask_cors import CORS
from datetime import datetime
import csv
import os

app = Flask(__name__)
CORS(app)

DATA_FILE = '/home/pi/sensor_data.csv'

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
    try:
        readings = []
        if not os.path.exists(DATA_FILE):
            return jsonify(readings), 200
        
        with open(DATA_FILE, 'r') as f:
            reader = csv.DictReader(f)
            rows = list(reader)
            
            for node_id in ['Node1', 'Node2', 'Node3']:
                node_readings = [r for r in rows if r.get('node_id') == node_id]
                if node_readings:
                    latest = node_readings[-1]
                    for key in latest:
                        if latest[key] == 'None' or latest[key] == '' or latest[key] == 'null':
                            latest[key] = None
                    readings.append(latest)
        return jsonify(readings), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health', methods=['GET'])
def health():
    return jsonify({'status': 'ok', 'timestamp': datetime.now().isoformat()}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
