// HDSP21xx.cpp
#include "HDSP21xx.h"

HDSP21xx::HDSP21xx() {
  displayInit();
  selftest();
}

void HDSP21xx::displayInit() {
  pinMode(HDSP211xCE, OUTPUT);
  pinMode(HDSP211xWR, OUTPUT);
  //pinMode(HDSP211xRD, OUTPUT);
  pinMode(HDSP211xFL, OUTPUT);
  pinMode(HDSP211xCLS, OUTPUT);
  pinMode(HDSP211xRST, OUTPUT);

  pinMode(HDSP211xA0, OUTPUT);
  pinMode(HDSP211xA1, OUTPUT);
  pinMode(HDSP211xA2, OUTPUT);
  pinMode(HDSP211xA3, OUTPUT);
  pinMode(HDSP211xA4, OUTPUT);

  pinMode(HDSP211xD0, OUTPUT);
  pinMode(HDSP211xD1, OUTPUT);
  pinMode(HDSP211xD2, OUTPUT);
  pinMode(HDSP211xD3, OUTPUT);
  pinMode(HDSP211xD4, OUTPUT);
  pinMode(HDSP211xD5, OUTPUT);
  pinMode(HDSP211xD6, OUTPUT);
  pinMode(HDSP211xD7, OUTPUT);

  // WR inactive/high
  digitalWrite(HDSP211xWR, HIGH);
  //digitalWrite(HDSP211xRD, HIGH);
  digitalWrite(HDSP211xCE, HIGH);
  digitalWrite(HDSP211xFL, HIGH);
  digitalWrite(HDSP211xCLS, HIGH);


  delayMicroseconds(200);

  digitalWrite(HDSP211xRST, HIGH);
  delayMicroseconds(200);
  digitalWrite(HDSP211xRST, LOW);
  delayMicroseconds(200);
}

void HDSP21xx::prepare() {
  // it seems, after the selftest, 3 seconds are nessessary before the screen recieves data
  delay(3000);
}

void HDSP21xx::setBit(uint8_t pin, uint8_t mask, uint8_t data) {
  if (data & mask)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

void HDSP21xx::sendByte(int data, int adr) {

  setBit(HDSP211xD0, 0x01, data);
  setBit(HDSP211xD1, 0x02, data);
  setBit(HDSP211xD2, 0x04, data);
  setBit(HDSP211xD3, 0x08, data);
  setBit(HDSP211xD4, 0x10, data);
  setBit(HDSP211xD5, 0x20, data);
  setBit(HDSP211xD6, 0x40, data);
  setBit(HDSP211xD7, 0x80, data);



  setBit(HDSP211xA0, B00001, adr);
  setBit(HDSP211xA1, B00010, adr);
  setBit(HDSP211xA2, B00100, adr);
  setBit(HDSP211xA3, B01000, adr);
  setBit(HDSP211xA4, B10000, adr);
  delay_12ns(); // min time before CE can be enabled

  digitalWrite(HDSP211xCE, LOW);
  delay_160ns(); // Wait for CE to stablilze

  digitalWrite(HDSP211xWR, LOW);
  delay_20ns();
  digitalWrite(HDSP211xWR, HIGH);
  digitalWrite(HDSP211xCE, HIGH);
  delay_160ns(); // 80 would be sufficient, but hey
}

void HDSP21xx::sendCtrl(uint8_t control_word) {

    // Control Word Register:
    // FL = 1
    // A4 = 1
    // A3 = 0
    // A2:A0 = 000
    uint8_t address = 0x10;

    digitalWrite(HDSP211xFL, HIGH);

    sendByte(control_word, address);
}


void HDSP21xx::selftest() {

    // Bit 6 = 1 starts the self-test
    uint8_t control_word = 0x40;
    sendCtrl(control_word);

    // Do NOT access the display while self-test is running.
    // At 58 kHz it takes approximately 4.5 seconds.
    delay(6000);
}

void HDSP21xx::fulltest() {
  int delaytime = 15;
  for (int i = 0; i < 8; i++) {

    for (char c = 'a'; c <= 'z'; c++) {
      setChar(c, i);
      delay(delaytime);
    }

    for (char c = 'A'; c <= 'Z'; c++) {
      setChar(c, i);
      delay(delaytime);
    }

    for (char c = '0'; c <= '9'; c++) {
      setChar(c, i);
      delay(delaytime);
    }

    setChar(0, i);
    delay(delaytime);
  }
}

void HDSP21xx::setChar(uint8_t data, uint8_t position) {
     // Write ASCII character to character position 0.
    //
    // Character RAM:
    // FL = 1
    // A4 = 1
    // A3 = 1
    // A2:A0 = character position
    //
    
  // Position 0 -> 11000b = 0x18
  int pos = B11000; // 0

  switch (position) {
      case 1:
        pos = B11001;
        break;
      case 2:
        pos = B11010;
        break;
      case 3:
        pos = B11011;
        break;
      case 4:
        pos = B11100;
        break;
      case 5:
        pos = B11101;
        break;
      case 6:
        pos = B11110;
        break;
      case 7:
        pos = B11111;
        break;
      default:
        // statements
        break;
    }

    digitalWrite(HDSP211xFL, HIGH);
    sendByte(data, pos);
}

void HDSP21xx::setText(String text) {
  String padded = text + "        ";
  for (uint8_t i = 0; i < 8; i++) {
    setChar((uint8_t)padded[i], i);
  }
}

void HDSP21xx::marquee(String text, int delayInMs) {
  for (uint16_t i = 0; i <= text.length(); i++) {
        String rotated = text.substring(i) + text.substring(0, i);

        setText(rotated);
        delay(delayInMs);
    }
}


void HDSP21xx::setBrightness(int level) {
  int data = B00000000;

  switch (level) {
      case 0:
        data = B00000111; // off
        break;
      case 1:
        data = B00000110; // 13%
        break;
      case 2:
        data = B00000101; // 20%
        break;
      case 3:
        data = B00000100; // 27%
        break;
      case 4:
        data = B00000011; // 40%
        break;
      case 5:
        data = B00000010; // 53%
        break;
      case 6:
        data = B00000001; // 80%
        break;
      default:
        data = B00000000; // 100%
        break;
    }
    sendCtrl(data);
}
