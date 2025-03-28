#include <LiquidCrystal.h>
#include <PulseSensorPlayground.h>
#include <DHT.h>

#define Temp_Hem 5
#define RELAY_PIN A5
#define DHTTYPE DHT11
#define buzzer 13
int LAMP_RELAY = 3;

DHT dht(Temp_Hem, DHTTYPE);
const int PulseWire = 0;
int Threshold = 550;
int myBPM = 0;

PulseSensorPlayground pulseSensor;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

unsigned long previousMillis = 0;
const long interval = 1000; // Temperature/Humidity update interval

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    pulseSensor.analogInput(PulseWire);
    pulseSensor.setThreshold(Threshold);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LAMP_RELAY, OUTPUT);
    pinMode(buzzer, OUTPUT);

    if (pulseSensor.begin()) {
        Serial.println("PulseSensor is ready!");
    }
    dht.begin();
}

void loop() {
    if (pulseSensor.sawStartOfBeat()) {
        myBPM = pulseSensor.getBeatsPerMinute();
        Serial.print("BPM: ");
        Serial.println(myBPM);
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            lcd.setCursor(0, 0);
            lcd.print("Sensor Error   ");
        } else {
            Serial.print("Temperature = ");
            Serial.print(temperature);
            Serial.println("°C");
            Serial.print("Humidity = ");
            Serial.println(humidity);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("T:");
            lcd.print(temperature, 1);  // 1 decimal place
            lcd.print("C ");

            lcd.print("H:");
            lcd.print(humidity, 1);
            lcd.print("%");

            lcd.setCursor(0, 1);
            lcd.print("BPM: ");
            lcd.print(myBPM);

            if (temperature > 30) {
                digitalWrite(RELAY_PIN, HIGH);
                digitalWrite(buzzer,HIGH);
            } else {
                digitalWrite(RELAY_PIN, LOW);
            }

            if (temperature < 20 ) {
                digitalWrite(LAMP_RELAY, LOW);
                digitalWrite(buzzer,HIGH);
            } else {
                digitalWrite(LAMP_RELAY, HIGH);
            }
        }
    }
    delay(20);  // Maintain pulse sensor sensitivity
}
