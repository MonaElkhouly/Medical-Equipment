#include <LiquidCrystal.h>

// Flow sensor and pump control pins
int flowPin = 2;
double flowRate;
volatile int count;
double outputvolume;
double totalvolume = 0;
#define ENB 8      // Motor speed control pin
#define IN3 7      // Motor direction control pin 1
#define IN4 6      // Motor direction control pin 2
#define BUZZ 10    // Buzzer pin

int BUTTON = 9;  // Button to control pump speed

// LCD configuration
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  pinMode(flowPin, INPUT);                // Set flow sensor pin as input
  attachInterrupt(0, Flow, RISING);       // Interrupt for flow sensor
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);          // Button with pull-up resistor

  Serial.begin(9600);                     // Start serial communication
  lcd.begin(16, 2);                       // Initialize LCD
  lcd.print("Flow: 0 L/min");
  lcd.setCursor(0, 1);
  lcd.print("Total: 0 L");
}

void loop() {
  // Set pump direction
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  bool button = digitalRead(BUTTON); // Read button state

  if (button == HIGH) {
    analogWrite(ENB, 255); // Run pump at full speed
  } else {
    analogWrite(ENB, 100); // Reduce pump speed
  }

  count = 0;
  interrupts();    // Enable interrupts to count flow pulses
  delay(1000);     // Measure flow for 1 second
  noInterrupts();  // Disable interrupts after measurement

  // Calculate flow rate and total volume
  flowRate = (count * 2.25) * 60 / 1000;
  outputvolume = flowRate / 60;
  totalvolume += outputvolume;

  // Display flow rate and total volume on Serial Monitor
  Serial.print("Flow Rate: ");
  Serial.print(flowRate);
  Serial.println(" L/min");

  // Update LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Flow: ");
  lcd.print(flowRate);
  lcd.print(" L/min");
  
  lcd.setCursor(0, 1);
  lcd.print("Total: ");
  lcd.print(totalvolume);
  lcd.print(" L");

  // Low flow rate warning
  if (flowRate < 0.3) {
    digitalWrite(BUZZ, HIGH); // Activate buzzer if flow rate is too low
    Serial.println("The flow rate is low");
  } else {
    digitalWrite(BUZZ, LOW);
  }

  // Stop pump when total volume reaches 1L
  if (totalvolume >= 1) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0); // Stop pump
    Serial.println("Pump stopped: Reached 1L.");
  }
}

// Interrupt function to count flow sensor pulses
void Flow() {
  count++;
}
