#include <Arduino.h>
#include <BLEMidi.h>
#include <Adafruit_MCP4728.h>
#include <Wire.h>
#include <WiFi.h>
#include <UMS3.h>

UMS3 ums3;
Adafruit_MCP4728 mcp;

const int midiOffset = 24;
const int channels = 4;
int newCV[4] = {0, 0, 0, 0};
int oldCV[4] = {0, 0, 0, 0};
int gateValues[4] = {LOW, LOW, LOW, LOW};
int ldac = 6;


int getStep(uint8_t pitch) {
  int steps = 4095;
  int octaves = 4;
  int pitchInt = pitch;
  return max(0, min((steps / (octaves * 12) * (pitchInt - midiOffset)), steps));
}

void onNoteOn(uint8_t midiChannel, uint8_t pitch, uint8_t velocity, uint16_t time) {
  newCV[midiChannel] = getStep(pitch);
  gateValues[midiChannel] = HIGH;
  //Serial.printf("Channel %d: midiChannel %d: Pitch %d: Velocity %d: Step %d:\n", channel, midiChannel, pitch, velocity, getStep);     
}


void onNoteOff(uint8_t midiChannel, uint8_t pitch, uint8_t velocity, uint16_t time) {
  gateValues[midiChannel] = LOW;
}

void setup() {
  ums3.begin();
  ums3.setPixelPower(false);
  /* ums3.setPixelBrightness(255/3);  not using the LED right now
  ums3.setPixelColor(255,0,0);
  Serial.begin(115200);
  */delay(10);
  setCpuFrequencyMhz(80);
  disableWiFi(); // this will save us some mA hopefully
  //Serial.println("ESP32 BLE MIDI device started");
  BLEMidiServer.begin("ESP32 BLE MIDI device");

  BLEMidiServer.setOnConnectCallback([]() {
  //  Serial.println("Connected");
  //  ums3.setPixelColor(0,128, 255); // neopixel BLUE indicates connection
  });
  BLEMidiServer.setOnDisconnectCallback([]() {
  //  ums3.setPixelColor(255,0,0); // revert neopixel to RED on disconnect
    for (int i = 1; i < 5; i++) {
      digitalWrite(i, LOW); // clear all gate pins
    }
    mcp.fastWrite(0, 0, 0, 0); // reset all CVs
  });

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);


  // initialise DAC
  if (!mcp.begin(0x64)) {
  //  Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }

  // setup various pins & clear outputs
  for (int p = 1; p < 5; p++) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
  pinMode(ldac, OUTPUT);
  digitalWrite(ldac, HIGH);

  mcp.fastWrite(0, 0, 0, 0);
}

void loop() {
  digitalWrite(ldac, HIGH);
  for (int updateValue = 0; updateValue < 4; updateValue++) {
    if (newCV[updateValue] != oldCV[updateValue]) {
      mcp.setChannelValue(MCP4728_channel_t(updateValue), newCV[updateValue]);
      oldCV[updateValue] = newCV[updateValue];
    }    
    digitalWrite(updateValue+1, gateValues[updateValue]);
  }
  digitalWrite(ldac, LOW);
}

void disableWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}