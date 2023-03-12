#include <Arduino.h>
#include <BLEMidi.h> // https://github.com/max22-/ESP32-BLE-MIDI
#include <Adafruit_MCP4728.h>
#include <Wire.h>

Adafruit_MCP4728 mcp;

// constants
const int TRIGGER_PIN1 = 1;
const int TRIGGER_PIN2 = 2;
const int TRIGGER_PIN3 = 3;
const int TRIGGER_PIN4 = 4;
const int MAX_OUTPUT_VOLTAGE = 5000;
const int LOWNOTE = 48;
const int HIGHNOTE = LOWNOTE + 60; // 5 octaves 

// global variables
float pitchvoltage = 0;

// ---------------------------------------------------------------------------
void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
  if (channel == 0) {
  if (note<LOWNOTE) note=LOWNOTE;
  if (note>HIGHNOTE) note=HIGHNOTE;


  pitchvoltage = (((note-LOWNOTE)/12.0)*1000.0);
  mcp.setChannelValue(MCP4728_CHANNEL_A, pitchvoltage);
  digitalWrite(TRIGGER_PIN1, HIGH);
  }

  else if (channel == 1) {
  if (note<LOWNOTE) note=LOWNOTE;
  if (note>HIGHNOTE) note=HIGHNOTE;


  pitchvoltage = (((note-LOWNOTE)/12.0)*1000.0);
  mcp.setChannelValue(MCP4728_CHANNEL_B, pitchvoltage);
  digitalWrite(TRIGGER_PIN2, HIGH);  
  }

  else if (channel == 2) {
  if (note<LOWNOTE) note=LOWNOTE;
  if (note>HIGHNOTE) note=HIGHNOTE;


  pitchvoltage = (((note-LOWNOTE)/12.0)*1000.0);
  mcp.setChannelValue(MCP4728_CHANNEL_C, pitchvoltage);
  digitalWrite(TRIGGER_PIN3, HIGH);     
  }

  else {
  if (note<LOWNOTE) note=LOWNOTE;
  if (note>HIGHNOTE) note=HIGHNOTE;


  pitchvoltage = (((note-LOWNOTE)/12.0)*1000.0);
  mcp.setChannelValue(MCP4728_CHANNEL_D, pitchvoltage);
  digitalWrite(TRIGGER_PIN4, HIGH);  
  }
  Serial.printf("Note on: MIDI channel %d, note %d, volts %fmV, velocity %d\n", channel+1, note, pitchvoltage, velocity);
}

// ---------------------------------------------------------------------------
void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
  if (channel == 0) {
  digitalWrite(TRIGGER_PIN1, LOW);
  }

  else if (channel == 1) {
  digitalWrite(TRIGGER_PIN2, LOW);
  }

  else if (channel == 2) {
  digitalWrite(TRIGGER_PIN3, LOW);
  }

  else {
  digitalWrite(TRIGGER_PIN4, LOW);
  }
}


// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BLE MIDI device started");
  BLEMidiServer.begin("ESP32 BLE MIDI device");

  BLEMidiServer.setOnConnectCallback([]() {
    Serial.println("Connected");
  });

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);

  // Try to initialize DAC
  if (!mcp.begin(0x64)) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }

// set up various pins
  pinMode(TRIGGER_PIN1, OUTPUT);
  digitalWrite(TRIGGER_PIN1, LOW);
  digitalWrite(TRIGGER_PIN2, LOW);
  digitalWrite(TRIGGER_PIN3, LOW);
  digitalWrite(TRIGGER_PIN4, LOW);
  mcp.setChannelValue(MCP4728_CHANNEL_A, 0);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 0);
  mcp.setChannelValue(MCP4728_CHANNEL_C, 0);
  mcp.setChannelValue(MCP4728_CHANNEL_D, 0);
}

// ---------------------------------------------------------------------------
void loop() {

}
