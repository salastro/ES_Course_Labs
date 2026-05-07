**Raspberry Pi ↔ MCU UART wiring & usage**

- Wiring (Pi TTL <-> MCU TTL):
  - Pi TX (e.g. /dev/serial0 TX) -> MCU RX pin (RX pin on MCU UART)
  - Pi RX (e.g. /dev/serial0 RX) -> MCU TX pin (TX pin on MCU UART)
  - Connect GND between Pi and MCU

- MCU baud: 9600, 8N1

- MCU protocol (ASCII):
  - Telemetry lines sent by MCU: F:<front>,B:<back>,S:<speed>\n
  - Commands from Pi to MCU:
    - S<value>\n  — set target motor speed (0-100)

- Quick usage (Pi):

  Install dependencies:

  ```bash
  pip3 install -r tools/requirements.txt
  ```

  Run helper:

  ```bash
  python3 tools/raspi_uart.py --port /dev/serial0
  ```

  Then type `s 80` to set motor speed to 80, or `q` to quit.
