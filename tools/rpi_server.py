from flask import Flask, request, jsonify
import serial
import threading
import time

app = Flask(__name__)

# ── Serial port to PIC ────────────────────────────────────────────────────────
# Change to /dev/ttyAMA0 if ttyS0 doesn't work
ser = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)

# ── Latest sensor readings (updated by background thread) ─────────────────────
sensor_data = {'front': 0.0, 'back': 0.0, 'left': 0.0, 'right': 0.0}
sensor_lock = threading.Lock()

def read_sensors():
    """
    Background thread: asks the PIC for sensor data every 200 ms.
    Expects the PIC to reply with a CSV line:  front,back,left,right\n
    e.g.  45.3,120.0,33.1,78.9\n
    """
    while True:
        try:
            ser.write(b'Q')           # 'Q' = Query sensors
            line = ser.readline().decode('utf-8').strip()
            if line:
                parts = line.split(',')
                if len(parts) == 4:
                    with sensor_lock:
                        sensor_data['front'] = float(parts[0])
                        sensor_data['back']  = float(parts[1])
                        sensor_data['left']  = float(parts[2])
                        sensor_data['right'] = float(parts[3])
        except Exception as e:
            print(f'[sensor thread] {e}')
        time.sleep(0.2)

# ── Routes ────────────────────────────────────────────────────────────────────
@app.route('/command')
def command():
    cmd = request.args.get('cmd', 'S').upper()
    # F=Forward, B=Backward, L=Left, R=Right, S=Stop, H=Horn
    if cmd in ('F', 'B', 'L', 'R', 'S', 'H'):
        ser.write(cmd.encode())
        return jsonify({'status': 'OK', 'cmd': cmd}), 200
    return jsonify({'status': 'INVALID'}), 400

@app.route('/sensors')
def sensors():
    with sensor_lock:
        data = dict(sensor_data)
    return jsonify(data), 200

# ── Entry point ───────────────────────────────────────────────────────────────
if __name__ == '__main__':
    t = threading.Thread(target=read_sensors, daemon=True)
    t.start()
    print('RC Car server running on port 5000 ...')
    app.run(host='0.0.0.0', port=5000)
