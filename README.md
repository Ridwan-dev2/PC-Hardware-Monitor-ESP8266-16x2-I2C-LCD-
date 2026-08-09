<div align="center">

# 🖥️ PC Hardware Monitor
### *Real-Time Microcontroller System Telemetry Display*

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![ESP8266](https://img.shields.io/badge/Hardware-ESP8266-red.svg?style=for-the-badge&logo=expressif&logoColor=white)](https://www.espressif.com/)
[![Python](https://img.shields.io/badge/Python-3.8+-yellow.svg?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino_C++-00979D.svg?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Build Passing](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=for-the-badge)](https://github.com/)

---

A low-latency, modular PC hardware telemetry monitoring system. Powered by an **ESP8266** microcontroller and a **16x2 I2C LCD Display**, this system displays real-time metrics (CPU, GPU, RAM, Network, SSD, OS) streamed via USB Serial from an asynchronous multi-threaded Python host engine.

<img width="1402" height="1122" alt="fffff" src="https://github.com/user-attachments/assets/b1fc55d0-017e-4fff-998f-51d9d435516f" />


[Key Features](#-key-features) • [System Architecture](#-system-architecture) • [Pinout & Wiring](#-pinout--wiring) • [Quick Start](#-quick-start) • [PC CLI Controls](#-pc-cli-controls)

---

</div>

## 📌 Features At A Glance

┌────────────────────────────────────────────────────────────────────────┐
│                        LCD ROTATION CYCLE (4s)                         │
├───────────┬───────────┬───────────┬───────────┬───────────┬────────────┤
│   PAGE 1  │   PAGE 2  │   PAGE 3  │   PAGE 4  │   PAGE 5  │   PAGE 6   │
│ ┌───────┐ │ ┌───────┐ │ ┌───────┐ │ ┌───────┐ │ ┌───────┐ │ ┌────────┐ │
│ │  GPU  │ │ │  CPU  │ │ │  RAM  │ │ │  NET  │ │ │  SSD  │ │ │  INFO  │ │
│ └───────┘ │ └───────┘ │ └───────┘ │ └───────┘ │ └───────┘ │ └────────┘ │
│ RTX 3060  │ R7 5700X  │ HyperX    │ Giga LAN  │ 980 SSD   │ Windows 11 │
│ Temp & Usg│ Temp & Usg│ 45px Bar  │ Up/Down MB│ R/W Speeds│ Credit Text│
└───────────┴───────────┴───────────┴───────────┴───────────┴────────────┘


* **⚡ Sub-Pixel Smooth RAM Engine:** custom 5-bit character generation mapping 0-100% memory usage across a granular **45-pixel precision bar** (no crude 1-block jumping).
* **🔀 Dynamic Motion Transitions:** 4 randomized hardware rendering transition effects (*Wipe Left, Wipe Right, Vertical Drop, Center Expand*) applied automatically between display pages.
* **🕹️ CLI Display Switcher:** Multi-threaded Python server allowing real-time screen pinning or auto-rotation toggles directly from your command line.
* **⚡ Zero Tearing Buffer:** Dual-screen double buffering on the C++ firmware prevents display flickers during continuous 1000ms updates.
* **✨ Dynamic Credits Effect:** Randomized micro-animations (Typewriter, Slide-In, Blink Reveal) compiled natively for custom credits string rendering.

---

## 🛠️ System Architecture

```ascii
 ┌───────────────────────────┐                ┌───────────────────────────┐
 │       HOST SYSTEM         │                │     EMBEDDED HARDWARE     │
 │                           │                │                           │
 │  ┌─────────────────────┐  │                │  ┌─────────────────────┐  │
 │  │ Hardware Telemetry  │  │                │  │    JSON Parser &    │  │
 │  │ (PyNVML/WMI/Psutil) │  │                │  │    Buffer Controller│  │
 │  └──────────┬──────────┘  │                │  └──────────┬──────────┘  │
 │             │             │   USB Serial   │             │             │
 │  ┌──────────▼──────────┐  │  115200 Baud   │  ┌──────────▼──────────┐  │
 │  │ Multi-Threaded CLI  ├──┼────────────────┼─►│ 16x2 I2C Display    │  │
 │  │ JSON Packet Streamer│  │                │  │ Character Generator │  │
 │  └─────────────────────┘  │                │  └─────────────────────┘  │
 └───────────────────────────┘                └───────────────────────────┘
🔌 Pinout & Wiring
Only 4 wires are required. Power is drawn directly from the ESP8266 onboard rail.

ESP8266 Pin	16x2 I2C LCD Module	Description
VIN / 5V	VCC	5V Main Power Supply
GND	GND	System Common Ground
D1 (GPIO 5)	SCL	I2C Serial Clock Line
D2 (GPIO 4)	SDA	I2C Serial Data Line
💻 Tech Stack & Dependencies
Microcontroller Firmware
Framework: Arduino C++ (ESP8266 Core)

Libraries:

Wire.h - I2C Communication

LiquidCrystal_I2C - LCD Controller Interface

ArduinoJson (v6+) - High-efficiency JSON Serialization

Host Software
Environment: Python 3.8+

Packages:

pyserial - Serial Interface Communication

psutil - System Metrics Fetching Engine

pynvml - NVIDIA GPU Management Library

wmi - Windows Management Instrumentation Engine

🚀 Quick Start Guide
Step 1: Flash ESP8266 Firmware
Open esp8266_code/esp8266_monitor.ino in Arduino IDE.

Install required libraries via Sketch > Include Library > Manage Libraries:

ArduinoJson by Benoit Blanchon

LiquidCrystal I2C by Frank de Brabander

Select your ESP8266 board model and upload the firmware.

Step 2: Configure & Run Host Engine
Clone this repository:

Bash
git clone [https://github.com/YOUR_USERNAME/PC-Hardware-Monitor-ESP8266.git](https://github.com/YOUR_USERNAME/PC-Hardware-Monitor-ESP8266.git)
cd PC-Hardware-Monitor-ESP8266
Install host Python requirements (Run terminal as Administrator for hardware sensor access):

Bash
pip install pyserial psutil pynvml wmi
Update COM_PORT in hardware_monitor.py to match your ESP8266 board (e.g., 'COM3'):

Python
COM_PORT = 'COM3'  # Windows: COMx | Linux: /dev/ttyUSB0
Execute the monitor engine:

Bash
python hardware_monitor.py
🎛️ PC CLI Controls
The Python host runs an interactive CLI thread while streaming hardware data. Enter any key from 0 to 6 to instantly adjust the display:

Plaintext
==================================================
    PC HARDWARE MONITOR DISPLAY CONTROLLER        
==================================================
 [0] 🔄 Auto Rotation Mode (Cycles pages every 4s)
 [1] 🎮 Display GPU Info Only
 [2] 🧠 Display CPU Info Only
 [3] ⚡ Display RAM Usage Only
 [4] 🌐 Display Network Speeds Only
 [5] 💾 Display SSD Speeds Only
 [6] 🖥️ Display OS Version & Credits Only
==================================================
Enter choice (0-6): 
📂 Repository Structure
Plaintext
.
├── 📁 esp8266_code
│   └── 📜 esp8266_monitor.ino    # Arduino C++ Firmware source code
├── 📁 python_script
│   └── 📜 hardware_monitor.py    # Python telemetry engine & CLI menu
├── 📜 LICENSE                    # MIT License
└── 📜 README.md                  # Project documentation
🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the issues page.

Fork the Project

Create your Feature Branch (git checkout -b feature/AmazingFeature)

Commit your Changes (git commit -m 'Add some AmazingFeature')

Push to the Branch (git checkout -b feature/AmazingFeature)

Open a Pull Request

📜 License
Distributed under the MIT License. See LICENSE for more information.

Crafted with ❤️ by Ridwan
