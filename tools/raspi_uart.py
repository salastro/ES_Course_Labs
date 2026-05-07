#!/usr/bin/env python3
"""
Simple Raspberry Pi UART tool to read telemetry and send speed commands.
Usage:
  - Install pyserial: pip3 install pyserial
  - Run: python3 raspi_uart.py --port /dev/serial0

Commands (type and Enter):
  s <0-100>    Set speed (example: `s 80`)
  q            Quit

Telemetry lines are expected in the format: F:<front>,B:<back>,S:<speed>
"""
import argparse
import serial
import threading
import sys

parser = argparse.ArgumentParser()
parser.add_argument('--port', default='/dev/serial0')
parser.add_argument('--baud', default=9600, type=int)
args = parser.parse_args()

try:
    ser = serial.Serial(args.port, args.baud, timeout=1)
except Exception as e:
    print('Failed to open serial port:', e)
    sys.exit(1)

running = True

def reader():
    while running:
        try:
            line = ser.readline().decode('ascii', errors='ignore').strip()
            if not line:
                continue
            print('[TELEMETRY]', line)
        except Exception as e:
            print('Read error:', e)
            break

thr = threading.Thread(target=reader, daemon=True)
thr.start()

print('Connected to', args.port, 'at', args.baud)
print('Type `s <value>` to set speed (0-100), `q` to quit')

try:
    while True:
        cmd = input('> ').strip()
        if not cmd:
            continue
        if cmd.lower() == 'q':
            break
        if cmd.lower().startswith('s '):
            parts = cmd.split()
            try:
                v = int(parts[1])
                if v < 0 or v > 100:
                    print('Value out of range')
                    continue
                send = f'S{v}\n'
                ser.write(send.encode('ascii'))
                print('Sent:', send.strip())
            except Exception as e:
                print('Invalid value')
        else:
            print('Unknown command')
except KeyboardInterrupt:
    pass

running = False
ser.close()
print('Exited')
