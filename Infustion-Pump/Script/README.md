# Flow Meter and Pump Control System

## Overview
This project is an Arduino-based flow meter and pump control system that measures water flow using a flow sensor and adjusts the pump speed based on user input. The system includes an LCD for real-time display of flow rate and total volume, and a buzzer to indicate low flow conditions.

## Features
- Measures real-time flow rate in L/min
- Displays total volume pumped in liters
- Adjustable pump speed using a button
- Buzzer alert for low flow conditions
- Automatic pump shut-off when 1L is reached
- LCD display for real-time monitoring

## Components Used
- Arduino Uno
- Flow sensor (Hall effect type)
- 16x2 LCD display with I2C interface
- Motor driver (L298N or similar)
- Water pump
- Push button
- Buzzer

## Wiring Diagram
| Component  | Arduino Pin |
|------------|------------|
| Flow Sensor | D2         |
| LCD (RS, EN, D4, D5, D6, D7) | 12, 11, 5, 4, 3, 13 |
| Pump Motor (IN3, IN4, ENB) | 7, 6, 8 |
| Button | 9 |
| Buzzer | 10 |

## Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/your-username/flow-meter-pump.git
   ```
2. Open the `.ino` file in the Arduino IDE.
3. Connect the components as per the wiring diagram.
4. Upload the code to the Arduino.

## Usage
- Press the button to control the pump speed.
- The LCD will display the real-time flow rate and total volume pumped.
- If the flow rate drops below 0.3 L/min, the buzzer will sound an alert.
- The pump will automatically stop when the total volume reaches 1L.

## Future Improvements
- Add an OLED or TFT display for a better user interface.
- Implement wireless monitoring via Bluetooth or Wi-Fi.
- Integrate a mobile app for remote control.

## License
This project is open-source and available under the MIT License.

## Author
[Your Name] - [Your GitHub Profile]

---
Feel free to contribute to this project by submitting pull requests or reporting issues!

