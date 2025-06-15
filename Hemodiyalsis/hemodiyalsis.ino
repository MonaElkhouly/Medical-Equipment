#include <Arduino.h>
#include "HX711.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // <-- make sure 0x27 is the correct address

#define ENA 9
#define IN1 8
#define IN2 7
#define ENB 3
#define IN3 5
#define IN4 4

#define s0 11       
#define s1 12
#define s2 10
#define s3 6
#define out A0

#define buzzer A3

const int tempSENSOR_PIN = 13;

OneWire oneWire(tempSENSOR_PIN);         // setup a oneWire instance
DallasTemperature tempSensor(&oneWire); // pass oneWire to DallasTemperature library

int   Red=0, Blue=0, Green=0;  //RGB values 

float tempCelsius;    // temperature in Celsius

const int LOADCELL_DOUT_PIN = A1;
const int LOADCELL_SCK_PIN = A2;
const int FLOW_SENSOR_PIN = 2; // Flow sensor connected to digital pin 2 (interrupt)

volatile unsigned long pulseCount = 0;
unsigned long lastPulseTime = 0;
float flowRate = 0.0; // Flow rate in L/min
float scale_factor = 1.0;

HX711 scale;

// Flags
bool weightMeasured = false;
float measuredWeight = 0;
float estimatedVolume = 0; // in milliliters
float expectedTime = 0;    // in seconds

void setup() {
  // put your setup code here, to run once:
    // put your setup code here, to run once:

   lcd.init();
   lcd.backlight();
   lcd.clear();

   pinMode(s0,OUTPUT);     //pin modes
   pinMode(s1,OUTPUT);
   pinMode(s2,OUTPUT);
   pinMode(s3,OUTPUT);
   pinMode(out,INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(buzzer,OUTPUT);

  tempSensor.begin();    // initialize the sensor

   Serial.begin(57600);

   digitalWrite(s0,HIGH); //Putting S0/S1 on HIGH/HIGH levels   means the output frequency scalling is at 100% (recommended)
   digitalWrite(s1,HIGH);   //LOW/LOW is off HIGH/LOW is 20% a


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  Serial.println("Starting calibration...");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  Serial.println("Remove all weight...");
  delay(3000);
  scale.tare();  // Reset the scale to 0

  Serial.println("Now place a known weight (example 100g)...");
  delay(5000);

  long reading = scale.get_units(10);
  Serial.print("Units reading: ");
  Serial.println(reading);

  float known_weight = 175;  // <-- Change this to your known weight
  //scale_factor = (float)reading / known_weight;
  scale_factor = -0.005;
  Serial.print("Calculated scale factor: ");
  Serial.println(scale_factor, 5);

  Serial.println("Calibration complete!");
  scale.set_scale(scale_factor);

  // Setup flow sensor
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);

}

void loop() {

  GetColors();  
  
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);


  // put your main code here, to run repeatedly:
  tempSensor.requestTemperatures();             // send the command to get temperatures
  tempCelsius = tempSensor.getTempCByIndex(0);  // read temperature in Celsius

  Serial.print("Temperature: ");
  Serial.print(tempCelsius);    // print the temperature in Celsius
  Serial.println("°C");
  delay(500);

  // Show temperature on LCD (first row)
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempCelsius, 1); // one decimal place
  lcd.print((char)223); // degree symbol
  lcd.print("C    ");    // extra spaces to clear old chars

 // Emergency stop flag
bool emergencyStop = false;
bool alarm = false;
// Check temperature
if (tempCelsius >= 30 || tempCelsius <= 20) {
  Serial.println("Abnormal Temperature Detected!");
  emergencyStop = true;
}

// Check color sensor
if (Red > 15 && Red < 23 && (Green - Blue) <= 5) {
  Serial.println("Blood Detected!");
  emergencyStop = true;
}

// Apply emergency stop
if (emergencyStop) {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  lcd.setCursor(0, 1);
  lcd.print("EMERGENCY STOP ");
  digitalWrite(buzzer,HIGH);
  delay(2000);
  digitalWrite(buzzer,LOW);
  delay(10000);
} else {
  digitalWrite(buzzer,LOW);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}


  //float weight = scale.get_units(10);
  float weight = 200;
  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.println(" grams");

  if (!weightMeasured && weight > 100.0) {
    measuredWeight = weight; // Save the weight
    estimatedVolume = (measuredWeight) * 70.0; // 70 mL per 1 kg
    Serial.print("Measured weight: ");
    Serial.print(measuredWeight, 2);
    Serial.println(" grams");

    Serial.print("Estimated volume: ");
    Serial.print(estimatedVolume, 2);
    Serial.println(" mL");

    weightMeasured = true; // Do this only once
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastPulseTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    
    float pulsesPerLiter = 450.0; // depends on your sensor, example 450 pulses per liter
    flowRate = (pulseCount / pulsesPerLiter) * 60.0; // L/min

    Serial.print("Flow rate: ");
    Serial.print(flowRate, 2);
    Serial.println(" L/min");

    if (weightMeasured && flowRate > 0.0) {
      float flowRate_mL_per_sec = (flowRate * 1000.0) / 60.0; // Convert L/min to mL/sec
      expectedTime = estimatedVolume / flowRate_mL_per_sec;

      Serial.print("Expected time: ");
      Serial.print(expectedTime, 2);
      Serial.println(" seconds");

     int countdown = (int)expectedTime;
      Serial.println("Starting countdown:");
      while (countdown > 0) {
        // Re-check color sensor
      analogWrite(ENA, 255);
      analogWrite(ENB, 255);
        GetColors();  
        if (Red > 15 && Red < 23 && Green - Blue <= 5) {
       break;
}


        Serial.print(countdown);
        Serial.println(" seconds remaining...");
        delay(1000); // Wait 1 second
        countdown--;

        // Show countdown on LCD (second row)
        lcd.setCursor(0, 1);
        lcd.print("Time: ");
        lcd.print(countdown);
        lcd.print(" sec     "); // clear extra chars

      }
      

      if (countdown == 0) {
        Serial.println("Countdown finished!");
      }
    }

    pulseCount = 0;
    lastPulseTime = currentMillis;
    
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  }

  delay(500); 

}

void GetColors()  
{    
  digitalWrite(s2,   LOW);                                           //S2/S3 levels define which set   of photodiodes we are using LOW/LOW is for RED LOW/HIGH is for Blue and HIGH/HIGH   is for green 
  digitalWrite(s3, LOW);                                           
   Red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);       //here we wait   until "out" go LOW, we start measuring the duration and stops when "out" is   HIGH again, if you have trouble with this expression check the bottom of the code
   delay(20);  
  digitalWrite(s3, HIGH);                                         //Here   we select the other color (set of photodiodes) and measure the other colors value   using the same techinque
  Blue = pulseIn(out, digitalRead(out) == HIGH ? LOW   : HIGH);
  delay(20);  
  digitalWrite(s2, HIGH);  
  Green = pulseIn(out,   digitalRead(out) == HIGH ? LOW : HIGH);
  delay(20);
 
}
void pulseCounter() {
  pulseCount++;
}
