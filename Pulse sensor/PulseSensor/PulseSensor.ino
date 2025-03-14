#include <PulseSensorPlayground.h>

#define BUTTON_PIN 4
const int PulseWire = A0;  // PulseSensor connected to Analog pin A0
const int LED = LED_BUILTIN;  // Built-in LED
int Threshold = 550;  // Threshold for detecting beats

PulseSensorPlayground pulseSensor;
int t = 0;  // Time variable
int myBPM = 0;  // BPM variable, initialized to 0 (no reading)
const int MIN_BPM = 40;  // Minimum realistic BPM
const int MAX_BPM = 180; // Maximum realistic BPM
bool abnormalityMode = false;  // Flag to switch ECG mode
bool lastButtonState = HIGH;  // Store previous button state
unsigned long lastDebounceTime = 0;  // Store last button press time
const unsigned long debounceDelay = 50;  // Debounce time (50ms)

int filterBPM(int bpm) {
    static int lastValidBPM = MIN_BPM;  // Keep track of last valid BPM

    if (bpm >= MIN_BPM && bpm <= MAX_BPM) {
        lastValidBPM = bpm;  // Accept valid readings directly
        return bpm;
    }

    // Apply filtering only for out-of-range values
    lastValidBPM = (lastValidBPM * 0.7) + (bpm * 0.3);
    return lastValidBPM;
}


void setup() {
    Serial.begin(115200);  // Start Serial Monitor & Plotter

    // Configure the PulseSensor object
    pulseSensor.analogInput(PulseWire);
    pulseSensor.blinkOnPulse(LED);
    pulseSensor.setThreshold(Threshold);
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable pull-up resistor for button

    if (pulseSensor.begin()) {
        Serial.println("PulseSensor initialized!");
    }
}

void loop() {
    if (pulseSensor.sawStartOfBeat()) {  
        int rawBPM = pulseSensor.getBeatsPerMinute();  
        int originalBPM = rawBPM;  // Store raw BPM before filtering
        myBPM = filterBPM(rawBPM);  // Apply BPM filtering only on out-of-range values

        float errorPercentage = 0.0;
        if (originalBPM > 0 && originalBPM != myBPM) {  // Compare original BPM with filtered one
            errorPercentage = abs((originalBPM - myBPM) / (float)originalBPM) * 100;
        }


        // Print BPM to Serial Monitor
        Serial.print("Raw BPM: ");
        Serial.print(rawBPM);
        Serial.print(" | Filtered BPM: ");
        Serial.print(myBPM);
        Serial.print(" | Error %: ");
        Serial.println(errorPercentage, 2);

    } else {
        // If no heartbeat detected, reduce BPM gradually to simulate natural behavior
        myBPM = max(myBPM - 1, 30);  // Prevents BPM from dropping to 0 instantly
    }

    // *Button Handling with Debounce and Toggle Mode*
    bool buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
        abnormalityMode = !abnormalityMode;  // Toggle hypokalemia mode
        Serial.print("abnormality Mode: ");
        Serial.println(abnormalityMode ? "ON" : "OFF");
        lastDebounceTime = millis();  // Reset debounce timer
    }

    lastButtonState = buttonState;  // Update last button state

    // Generate ECG signal based on mode
    int ecgValue = (myBPM > 30) ? generateECG(t, myBPM, abnormalityMode) : 512;  // Default baseline if no finger

    // Send ECG waveform values to Serial Plotter
    Serial.println(ecgValue + myBPM);  

    t += 5;
    delay(10);  // Control waveform speed
}

// Function to normalize BPM between 0 and 1
float normalizeBPM(int bpm) {
    bpm = constrain(bpm, MIN_BPM, MAX_BPM);  // Keep within valid range
    return (float)(bpm - MIN_BPM) / (MAX_BPM - MIN_BPM);
}

// Function to generate ECG waveform, with U-wave if hypokalemia is active
int generateECG(int t, int bpm, bool abnormality) {
    float T = 60000.0 / bpm;  // Adjust period based on BPM
    float phase = (t % (int)T) / T * TWO_PI;  // Normalize time to one cycle
    
    float baseline = 512;  // Midpoint (baseline ECG value)
    
    // Simulating ECG wave components using Gaussian peaks
    float P_wave = 20 * exp(-pow(phase - 0.6, 2) / 0.02);  // Small P-wave
    float Q_wave = -100 * exp(-pow(phase - 1.2, 2) / 0.01); // Small downward Q-wave
    float R_wave = 300 * exp(-pow(phase - 1.3, 2) / 0.005); // Sharp upward R-peak
    float S_wave = -80 * exp(-pow(phase - 1.4, 2) / 0.005); // Small downward S-wave
    float T_wave = 30 * exp(-pow(phase - 2.2, 2) / 0.03); // Broad T-wave

    float ecgSignal = baseline + P_wave + Q_wave + R_wave + S_wave + T_wave;

    // If hypokalemia mode is active, add a *prominent U-wave*
    if (abnormality) {
        float ab_wave = -1200 * exp(-pow(phase - 1.3, 2) / 0.005); // *Prominent U-wave*
        ecgSignal += ab_wave;
    }

    return constrain((int)ecgSignal, 0, 1023);  // Keep within ADC range
}
