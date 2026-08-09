# 🖥️ PC Hardware Monitor (ESP8266 + 16x2 I2C LCD)

A sleek, real-time PC hardware monitor built with an ESP8266 and a standard 16x2 character display. This project fetches live telemetry from your PC (CPU, GPU, RAM, Network, and SSD speeds) and streams it to the LCD via USB.

It features **smooth transition animations**, a custom **sub-pixel RAM progress bar**, and an **interactive Python terminal menu** to control what the display shows on the fly—no physical buttons required!
<img width="1402" height="1122" alt="fffff" src="https://github.com/user-attachments/assets/5c1c59fa-91e1-4f24-a96a-e89eb1be26b0" />

---

## ✨ Features
* **6 Dedicated Info Pages:** 
  1. GPU Name, Temp, and Usage
  2. CPU Name, Temp, and Usage
  3. RAM Name and Smooth Usage Bar
  4. Network Adapter, Up & Down Speeds (with custom arrows)
  5. SSD Name, Read & Write Speeds (MB/s)
  6. OS Version and Dynamic Credits
* **Custom Sub-Pixel Progress Bar:** RAM usage isn't just blocky; it fills smoothly pixel-by-pixel.
* **Slick Animations:** Pages transition using random effects (Wipe Right, Wipe Left, Drop-Down, Center-Expand).
* **Interactive PC Menu:** Control the display directly from your PC terminal to either auto-rotate every 4 seconds or lock onto a specific hardware page.

---

## 🛠️ Hardware Requirements
* **ESP8266 Board** (NodeMCU, Wemos D1 Mini, etc.)
* **16x2 Character LCD** (with I2C Backpack)
* **4x Female-to-Female Jumper Wires**
* **Micro-USB Cable** (capable of data transfer)

### 🔌 Wiring Diagram
Connecting the hardware is incredibly simple. Only 4 wires are needed:

| ESP8266 Pin | 16x2 I2C LCD |
| :--- | :--- |
| **VIN / 5V** | VCC |
| **GND** | GND |
| **D1 (GPIO 5)** | SCL |
| **D2 (GPIO 4)** | SDA |

---

## 💻 Software Setup

### 1. ESP8266 Setup (Arduino IDE)
1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries**.
3. Install the following libraries:
   * `LiquidCrystal I2C` by Frank de Brabander
   * `ArduinoJson` by Benoit Blanchon
4. Connect your ESP8266, select the correct COM port and board.
5. Upload the `esp8266_monitor.ino` code to your board.
   * *Note: If your LCD lights up but shows no text, try changing the I2C address in the code from `0x27` to `0x3F`.*

### 2. PC Setup (Python)
The PC script gathers hardware data and streams it over USB to the ESP8266.

1. Install [Python](https://www.python.org/downloads/) (Make sure to check "Add Python to PATH" during installation).
2. Open Command Prompt or Terminal as **Administrator** and install the required dependencies:
   ```bash
   pip install pyserial psutil pynvml wmi
Open hardware_monitor.py in a text editor and update the COM_PORT variable at the top to match your ESP8266's port (e.g., 'COM3' for Windows or '/dev/ttyUSB0' for Linux/Mac).

Python
COM_PORT = 'COM3'  # <-- CHANGE THIS
🚀 How to Run
Plug in your ESP8266. The screen will say "Waiting PC Py..".

Run the Python script as Administrator (required for WMI to read CPU temperatures correctly on Windows):

Bash
python hardware_monitor.py
The terminal will open an interactive menu, and your LCD will immediately come to life!

🎛️ Terminal Menu Controls
While the script is running, type a number in your terminal and press Enter to change the display mode instantly:

0 - Auto Rotation Mode (Cycles all 6 pages every 4 seconds)

1 - Display GPU Info Only

2 - Display CPU Info Only

3 - Display RAM Usage Only

4 - Display Network Speeds Only

5 - Display SSD Speeds Only

6 - Display OS Version & Credits Only

📂 Project Structure
📦 PC-Hardware-Monitor-ESP8266
 ┣ 📂 esp8266_code
 ┃ ┗ 📜 esp8266_monitor.ino     # C++ code for the ESP8266
 ┣ 📂 python_script
 ┃ ┗ 📜 hardware_monitor.py     # PC backend script
 ┗ 📜 README.md
Made by Ridwan :)
