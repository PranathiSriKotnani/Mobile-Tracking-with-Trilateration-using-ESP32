# Mobile-Tracking-with-Trilateration-using-ESP32

## 📖 Project Description

This project implements indoor location tracking using trilateration and RSSI measurements with ESP32 boards. Instead of GPS, three fixed ESP32 towers listen to a broadcasting ESP32 device (target) and calculate distances using signal strength (RSSI) and then derive position using basic trilateration math.

## 🧠 Theory

Trilateration estimates the position of a target by using the known positions of at least three fixed receivers (towers) and their distances to the target. In this project:

- The target ESP32 broadcasts packets over ESP-NOW.

- Each tower ESP32 measures the RSSI from received packets and converts RSSI to distance using a calibrated path-loss model.

- When all three towers have collected distance data, they apply trilateration formulas to calculate the 2D coordinates of the target.

This method is commonly used for indoor positioning where GPS signals are weak or unavailable.

## 🛠️ Requirements

  - 4x ESP32 boards (3 as towers, 1 as mobile target)

  - Arduino IDE with ESP32 board support

  - Libraries :
     - `WiFi.h`
     - `esp_now.h`
     - `esp_wifi.h`

  - Basic knowledge of signal attenuation and coordinates math

  - Serial monitor (or optional OLED) for viewing output

## 📋⚙️ Setup Instructions

1. Flash mobile code to one ESP32 (the moving device)

2. Flash tower code to three ESP32s (change #define TOWER_ID to 1, 2, and 3 respectively)

3. Keep the towers fixed at known coordinates (triangular layout recommended)

4. Power on all devices; mobile ESP will start broadcasting

5. Each tower calculates distance and eventually estimates the mobile’s position

## 📂 Project Folder Structure

Mobile-Tracking-with-Trilateration-using-ESP32/
├── towers/
│   ├── esp32_tower1_receiver.ino
│   ├── esp32_tower2_receiver.ino
│   └── esp32_tower3_receiver.ino
├── mobile/
│   └── esp32_mobile_target.ino
└── README.md



## 🚀 Applications

- Indoor positioning systems

- Smart asset or personnel tracking

- Warehouse and robotics navigation

## ⚠️ Disclaimer

Accuracy depends on environment, interference, and RSSI stability. This implementation is experimental and intended for learning.
