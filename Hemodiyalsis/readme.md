# Hemodialysis Monitoring System

## Overview
Arduino-based hemodialysis system that automatically calculates treatment time based on patient weight and monitors safety parameters during dialysis sessions.


## Features
- **Patient Weight Detection**: Uses load cell to measure patient weight
- **Automatic Time Calculation**: Treatment duration based on 70 mL per kg body weight
- **Fluid Pumping**: Dual motor system for precise fluid control
- **Safety Monitoring**: Temperature and blood detection with emergency stop
- **Real-time Display**: LCD shows temperature and countdown timer

## Hardware Components
- Arduino microcontroller
- Load cell with HX711 amplifier (weight measurement)
- DS18B20 temperature sensor
- TCS3200 color sensor (blood detection)
- Flow sensor
- Motor drivers and pumps
- 16x2 LCD display
- Buzzer for alarms

## Pin Connections
- **Motors**: ENA(9), IN1(8), IN2(7), ENB(3), IN3(5), IN4(4)
- **Color Sensor**: S0(11), S1(12), S2(10), S3(6), OUT(A0)
- **Load Cell**: DOUT(A1), SCK(A2)
- **Temperature**: Pin 13
- **Flow Sensor**: Pin 2
- **Buzzer**: Pin A3
- **LCD**: I2C (SDA/SCL

## How It Works
1. System measures patient weight
2. Calculates required fluid volume (70 mL per kg)
3. Monitors flow rate to determine treatment time
4. Displays countdown timer on LCD
5. Continuously monitors temperature (20-30°C) and blood presence
6. Emergency stop if abnormal conditions detected

## Safety Features
- Temperature monitoring with automatic shutdown
- Blood detection using color sensor
- Audible alarms
- Emergency pump shutdown

## Pin Connections
- **Motors**: ENA(9), IN1(8), IN2(7), ENB(3), IN3(5), IN4(4)
- **Color Sensor**: S0(11), S1(12), S2(10), S3(6), OUT(A0)
- **Load Cell**: DOUT(A1), SCK(A2)
- **Temperature**: Pin 13
- **Flow Sensor**: Pin 2
- **Buzzer**: Pin A3
- **LCD**: I2C (SDA/SCL)

## Setup
1. Wire components according to pin connections
2. Calibrate load cell with known weight
3. Upload code to Arduino
4. System ready for operation

---
