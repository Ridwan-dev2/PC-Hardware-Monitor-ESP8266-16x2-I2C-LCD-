import serial
import time
import json
import psutil
import pynvml
import wmi
import threading

# --- CONFIGURATION ---
COM_PORT = 'COM4'  # Change to your ESP8266's COM Port (e.g. COM3, COM4, /dev/ttyUSB0)
BAUD_RATE = 115200
# ---------------------

# Global mode selection variable
selected_mode = 0  # Default to 0 (Auto Rotation)

def user_menu_thread():
    """ Runs a terminal menu allowing real-time switching of display mode """
    global selected_mode
    while True:
        print("\=======================================================")
        print("    PC HARDWARE MONITOR DISPLAY CONTROLLER BY R1DWAN    ")
        print("========================================================")
        print(" [0] Auto Rotation Mode (Cycle all 6 pages)")
        print(" [1] Display GPU Info Only")
        print(" [2] Display CPU Info Only")
        print(" [3] Display RAM Usage Only")
        print(" [4] Display Network Speeds Only")
        print(" [5] Display SSD Speeds Only")
        print(" [6] Display OS Version & Credits Only")
        print("==============================================")
        
        try:
            choice = input("Enter choice (0-6): ").strip()
            if choice.isdigit() and 0 <= int(choice) <= 6:
                selected_mode = int(choice)
                print(f">> Display mode updated to: {selected_mode}\n")
            else:
                print(">> Invalid input. Please enter a number between 0 and 6.")
        except Exception as e:
            pass

# Connect to Serial
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for ESP to reset upon connection
    print("Connected to ESP8266 successfully!")
except Exception as e:
    print(f"Error opening Serial port {COM_PORT}: {e}")
    exit()

# Initialize Nvidia GPU reader
has_nvidia = False
try:
    pynvml.nvmlInit()
    gpu_handle = pynvml.nvmlDeviceGetHandleByIndex(0)
    has_nvidia = True
except:
    print("Nvidia GPU library initialization skipped or unavailable.")

w = wmi.WMI()

last_net_io = psutil.net_io_counters()
last_disk_io = psutil.disk_io_counters()
last_time = time.time()

# Start interactive CLI thread
menu_thread = threading.Thread(target=user_menu_thread, daemon=True)
menu_thread.start()

print("Streaming hardware metrics to ESP8266...")

# Main Data Streaming Loop
while True:
    try:
        current_time = time.time()
        time_elapsed = max(current_time - last_time, 0.1)

        # 1. GPU Temp & Usage
        gT, gU = 0, 0
        if has_nvidia:
            gT = pynvml.nvmlDeviceGetTemperature(gpu_handle, pynvml.NVML_TEMPERATURE_GPU)
            gU = pynvml.nvmlDeviceGetUtilizationRates(gpu_handle).gpu

        # 2. CPU Usage & Temp
        cU = int(psutil.cpu_percent())
        cT = 0
        try:
            wmi_temps = w.MSAcpi_ThermalZoneTemperature()
            cT = int((wmi_temps[0].CurrentTemperature / 10.0) - 273.15)
        except:
            cT = 48  # Default mock value if access denied

        # 3. RAM Usage
        rU = int(psutil.virtual_memory().percent)

        # 4. Network Speeds (MB/s)
        net_io = psutil.net_io_counters()
        nU = ((net_io.bytes_sent - last_net_io.bytes_sent) / 1024 / 1024) / time_elapsed
        nD = ((net_io.bytes_recv - last_net_io.bytes_recv) / 1024 / 1024) / time_elapsed
        last_net_io = net_io

        # 5. SSD Disk Speeds (MB/s)
        disk_io = psutil.disk_io_counters()
        sR = int(((disk_io.read_bytes - last_disk_io.read_bytes) / 1024 / 1024) / time_elapsed)
        sW = int(((disk_io.write_bytes - last_disk_io.write_bytes) / 1024 / 1024) / time_elapsed)
        last_disk_io = disk_io

        last_time = current_time

        # Create JSON Payload containing mode 'm' and hardware stats
        data = {
            "m": selected_mode,
            "gT": gT, "gU": gU,
            "cT": cT, "cU": cU,
            "rU": rU,
            "nU": round(nU, 1), "nD": round(nD, 1),
            "sR": sR, "sW": sW
        }
        
        json_str = json.dumps(data) + '\n'
        ser.write(json_str.encode('utf-8'))
        
        time.sleep(1) # Send update once every 1 second

    except KeyboardInterrupt:
        print("\nExiting PC Monitor...")
        break
    except Exception as e:
        time.sleep(1)