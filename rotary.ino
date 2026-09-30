// library for rotary encoder
#include "AiEsp32RotaryEncoder.h"

// used pins for rotary encoder
#define ROTARY_ENCODER_A_PIN 33
#define ROTARY_ENCODER_B_PIN 32
#define ROTARY_ENCODER_BUTTON_PIN 27 // WICHTIG: 27 stat 34 wegen Pull-Up
#define ROTARY_ENCODER_VCC_PIN                                                 \
  -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else    \
        you can use declared output pin for powering rotary encoder */

// depending on your encoder - try 1,2 or 4 to get expected behaviour
// #define ROTARY_ENCODER_STEPS 1
// #define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

// instance for rotary encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(
    ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN,
    ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

// to be called in 'loop()'
// handle events from rotary encoder
void rotary_loop() {
  // dont do anything unless value changed
  if (rotaryEncoder.encoderChanged()) {
    uint16_t v = rotaryEncoder.readEncoder();
    Serial.printf("Station: %i\n", v);
    // set new currtent station and show its name
    if (v < STATIONS) {
      curStation = v;
      // showStation();
      lastchange = millis();
    }
  }
  
  if ((lastchange > 0) && ((millis() - lastchange) > 600)) {
    if (curStation != actStation) {
      actStation = curStation;
      Serial.printf("Auto-switching to station %s... Loading stream!\n", stationlist[actStation].name);
      pref.putUShort("station", curStation);
      
      // We switch the stream directly without restarting!
      // ESP.restart() causes this specific ESP32 board to hang in the bootloader.
      startUrl();
    }
    lastchange = 0; // Reset timer
  }
}

// interrupt handling for rotary encoder
void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); }

// to be called in 'setup()'
void setup_rotary() {
  // start rotary encoder instance
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(
      0, STATIONS - 1, true); // minValue, maxValue, circleValues true|false (when
                          // max go to min and vice versa)
  rotaryEncoder.disableAcceleration();
}
