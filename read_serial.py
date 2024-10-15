import serial
import json
import time

# Adjust the serial port and baud rate to match your Arduino setup
SERIAL_PORT = '/dev/ttyACM0'  # For Linux (or '/dev/ttyUSB0')
#SERIAL_PORT = 'COM5'  # For Windows (change COM5 to your actual port)
BAUD_RATE = 9600

def read_serial():
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
        time.sleep(2)  # Wait for the serial connection to establish
        print("Connected to the serial port")
        while True:
            line = ser.readline().decode('utf-8').strip()  # Read and decode the line
            if line:
                try:
                    # Parse JSON data
                    data = json.loads(line)
                    
                    # Process the parsed data
                    if "spikeStatus" in data:
                        if data["spikeStatus"] == "start":
                            print("Spike started.")
                        elif data["spikeStatus"] == "end":
                            print(f"Spike ended. Duration: {data['spikeDuration']} ms")
                            
                            # Check if spikeIntensity is available in the data
                            if "spikeIntensity" in data:
                                print(f"Spike Intensity: {data['spikeIntensity']:.2f}%")
                        else:
                            print("No spike detected.")
                    
                except json.JSONDecodeError as e:
                    print(f"Error decoding JSON: {e}")
            time.sleep(0.1)

if __name__ == "__main__":
    read_serial()
