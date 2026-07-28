/*
 * Modifed by Jason Hill
 * 2023/06/18 - Switched digital read/write to direct port manipulation.
 *            - Added constructor for streaming mode
 *            - Changed clock and data pin from 'int' to 'byte'
 *            - Added debug compile time option (Levels: none, 0, 1), see 'ProMicro.h'
 */
#include "ProMicro.h"
#include "Ps2Mouse.h"

static const int PS2_CLOCK = 2; //PD2 - Port D, bit 2
static const int PS2_DATA  = 4; //PD4 - Port D, bit 4
static const int RS232_RTS = 3; //PD3 - Port D, bit 3
// JP1 pins 1-2 (A7) and 3-4 (A6). A6/A7 are analog-input-only on the
// ATmega328P (no DDR/PORT bit, no internal pull-up) so they need an
// external pull-up to VBUS and must be read via analogRead(), not
// pinMode()/digitalRead().
static const int JP12 = A7;
static const int JP34 = A6;
// JP1 pins 5-6: debug-mode select, extended onto the same jumper block as
// JP12/JP34. Analog-read like the others (see jumperInstalled()).
static const int JP56 = A5;
// Mouse-activity indicator LED, driven directly by firmware (not the
// hardware TX-monitor LED on the MAX3232's real RS232 output).
static const int ACT_LED_PIN = A1;
static const int LED = 13;
static const unsigned long ACT_LED_BLINK_MS = 15;

static bool jumperInstalled(int pin) {
  return analogRead(pin) < 512;
}

bool debugMode = false;
unsigned long actLedOffTime = 0;

Ps2Mouse *mouse;
int errorCount = 0;
bool stream = 0;
int pullRate = -1;



Ps2Mouse::Data data;
static bool threeButtons = false;


static void sendToSerial(const Ps2Mouse::Data& data) {
  auto dx = constrain(data.xMovement, -127, 127);
  auto dy = constrain(-data.yMovement, -127, 127);
  byte lb = data.leftButton ? 0x20 : 0;
  byte rb = data.rightButton ? 0x10 : 0;
  byte mb = data.middleButton ? 0x20 : 0;
  byte msg[4];
  msg[0]=(0x40 | lb | rb | ((dy >> 4) & 0xC) | ((dx >> 6) & 0x3));
  msg[1]=(dx & 0x3F);
  msg[2]=(dy & 0x3F);
  msg[3]=mb;
  Serial.write(msg,threeButtons?4:3);  
}

static void initSerialPort() {
  if (!debugMode) {
    Serial.begin(1200,SERIAL_7N1);
    byte msg[2];
    msg[0]='M';
    msg[1]='3';
    Serial.write(msg,threeButtons?2:1);
  } else {
    //In debug mode you dont get mouse movements writen to serial
    Serial.begin(115200);
    Serial.println("Starting serial port");
  }
  pinMode(RS232_RTS, INPUT_PULLUP);
  void (*resetHack)() = 0;
  attachInterrupt(digitalPinToInterrupt(RS232_RTS), resetHack, FALLING);
}

static void initPs2Port() {
  errorCount = 0;
  stream = jumperInstalled(JP34);
  if (debugMode) {
    Serial.println("Reseting PS/2 mouse");
  }
  //TODO: Cleanup
  //Identify streaming mode or not.
  mouse->reset(stream);
  mouse->setResolution(2);
  Ps2Mouse::Settings settings;
  if (mouse->getSettings(settings)) {
    if (debugMode) {
      Serial.print("scaling = ");
      Serial.println(settings.scaling);
      Serial.print("resolution = ");
      Serial.println(settings.resolution);
      Serial.print("samplingRate = ");
      Serial.println(settings.sampleRate);
    }
    pullRate = settings.sampleRate;
  }
    mouse->clearData(data);
}

void setup() {
  // PS/2 Data input must be initialized shortly after power on,
  // or the mouse will not initialize
  pinMode(PS2_DATA, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(ACT_LED_PIN, OUTPUT);
  digitalWrite(ACT_LED_PIN, LOW);
  threeButtons = !jumperInstalled(JP12);
  // DEBUG (ProMicro.h) still force-enables debug mode at compile time;
  // the jumper lets it be toggled at runtime without reflashing.
  debugMode = (DEBUG > 0) || jumperInstalled(JP56);
  digitalWrite(LED, HIGH);

  initSerialPort();
  mouse = new Ps2Mouse(PS2_CLOCK, PS2_DATA, jumperInstalled(JP34));
  initPs2Port();
  if (debugMode) {
    Serial.println("Setup done!");
  }
  digitalWrite(LED, LOW);
  
}



void loop() {
  if (actLedOffTime && millis() >= actLedOffTime) {
    digitalWrite(ACT_LED_PIN, LOW);
    actLedOffTime = 0;
  }

  Ps2Mouse::Data newData;
   if(!stream)
        delayMicroseconds(1000000/pullRate);
  int status = mouse->readData(newData);
  if (status==0) {
    mouse->accumulate(newData,data);
    digitalWrite(ACT_LED_PIN, HIGH);
    actLedOffTime = millis() + ACT_LED_BLINK_MS;
    if (Serial.availableForWrite() >= 4){
      if (!debugMode) {
        sendToSerial(data);
      }
      if (debugMode) {
          Serial.print(data.xMovement);
          Serial.print(",");
          Serial.print(data.yMovement);
          Serial.print(",");
          Serial.print(data.leftButton,HEX);
          Serial.print(",");
          Serial.print(data.middleButton,HEX);
          Serial.print(",");
          Serial.println(data.rightButton,HEX);
      }
      mouse->clearData(data);
    }
  }else if(status>1){
    errorCount++;
    if (debugMode) {
      Serial.println("Packet error");
    }
    if(errorCount>RESETON){
      initPs2Port();
    }
  }

  //Note: 'mouse' never needs deleted from memory unless testing.
  //delete mouse;
}
