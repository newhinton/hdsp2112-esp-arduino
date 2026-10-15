// HDSP21xx.cpp
#include "HDSP21xx.h"
#include "esphome/core/log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_timer.h>
#include "esphome/core/hal.h"

namespace {

void delay_ms(uint32_t milliseconds) {
  //TickType_t ticks = pdMS_TO_TICKS(milliseconds);
  //vTaskDelay(ticks == 0 ? 1 : ticks);

  esphome::delay_microseconds_safe(milliseconds*20);
}

}  // namespace

HDSP21xx::HDSP21xx() {
}

void HDSP21xx::displayInit() {

  ESP_LOGI("hdsp2112", "displayInit: CE");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xCE));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xCE), GPIO_MODE_OUTPUT);
  ESP_LOGI("hdsp2112", "displayInit: WR");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xWR));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xWR), GPIO_MODE_OUTPUT);
  ESP_LOGI("hdsp2112", "displayInit: FL");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xFL));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xFL), GPIO_MODE_OUTPUT);
  ESP_LOGI("hdsp2112", "displayInit: CLS");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xCLS));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xCLS), GPIO_MODE_OUTPUT);
  ESP_LOGI("hdsp2112", "displayInit: RST");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xRST));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xRST), GPIO_MODE_OUTPUT);

  ESP_LOGI("hdsp2112", "displayInit: A0-A4");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xA0));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xA0), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xA1));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xA1), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xA2));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xA2), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xA3));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xA3), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xA4));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xA4), GPIO_MODE_OUTPUT);

  ESP_LOGI("hdsp2112", "displayInit: D0-D7");
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD0));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD0), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD1));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD1), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD2));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD2), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD3));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD3), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD4));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD4), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD5));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD5), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD6));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD6), GPIO_MODE_OUTPUT);
  gpio_reset_pin(static_cast<gpio_num_t>(HDSP211xD7));
  gpio_set_direction(static_cast<gpio_num_t>(HDSP211xD7), GPIO_MODE_OUTPUT);


  ESP_LOGI("hdsp2112", "displayInit: GPIO initialization complete");

  // WR inactive/high
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xWR), 1);
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xCE), 1);
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xFL), 1);
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xCLS), 1);

  ESP_LOGI("hdsp2112", "displayInit: initial outputs complete");

  delay_ms(1);

  gpio_set_level(static_cast<gpio_num_t>(HDSP211xRST), 1);
  delay_ms(1);
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xRST), 0);
  delay_ms(1);

  ESP_LOGI("hdsp2112", "displayInit: complete");
}

void HDSP21xx::prepare() {


  ESP_LOGD("hdsp2112", "prepare: displayInit");
  displayInit();
  ESP_LOGD("hdsp2112", "prepare: displayInit done");

  selftest();
  ESP_LOGD("hdsp2112", "prepare: selftest done");

  // it seems, after the selftest, 3 seconds are nessessary before the screen recieves data
  //delay_ms(3000);
}

void HDSP21xx::setBit(uint8_t pin, uint8_t mask, uint8_t data) {
  if (data & mask)
    gpio_set_level(static_cast<gpio_num_t>(pin), 1);
  else
    gpio_set_level(static_cast<gpio_num_t>(pin), 0);
}

void HDSP21xx::sendByte(int data, int adr) {

  // CE and WR must be inactive
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xWR), 1);
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xCE), 1);

  // Set data bus
  setBit(HDSP211xD0, 0x01, data);
  setBit(HDSP211xD1, 0x02, data);
  setBit(HDSP211xD2, 0x04, data);
  setBit(HDSP211xD3, 0x08, data);
  setBit(HDSP211xD4, 0x10, data);
  setBit(HDSP211xD5, 0x20, data);
  setBit(HDSP211xD6, 0x40, data);
  setBit(HDSP211xD7, 0x80, data);


  // Set address bus
  setBit(HDSP211xA0, 0b00001, adr);
  setBit(HDSP211xA1, 0b00010, adr);
  setBit(HDSP211xA2, 0b00100, adr);
  setBit(HDSP211xA3, 0b01000, adr);
  setBit(HDSP211xA4, 0b10000, adr);

  // Allow address/data to settle
  // min time before CE can be enabled
  //delay_12ns();
  delay_ms(1);

  // Select display
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xCE), 0);

  // CE must be active before WR
  // Wait for CE to stablilze
  //delay_160ns();
  delay_ms(1);

  // Write
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xWR), 0);

  //delay_20ns();
  delay_ms(1);

  gpio_set_level(static_cast<gpio_num_t>(HDSP211xWR), 1);

  // Finish cycle
  gpio_set_level(static_cast<gpio_num_t>(HDSP211xCE), 1);

  //delay_160ns(); // 80 would be sufficient, but hey
  delay_ms(1);
}

void HDSP21xx::sendCtrl(uint8_t control_word) {

    // Control Word Register:
    // FL = 1
    // A4 = 1
    // A3 = 0
    // A2:A0 = 000
    uint8_t address = 0x10;

    gpio_set_level(static_cast<gpio_num_t>(HDSP211xFL), 1);

    sendByte(control_word, address);
}

bool HDSP21xx::isInitialized() {
  uint64_t now = esp_timer_get_time() / 1000ULL;
  if (now - initStartedAt < 10000) { // assuming 10 second initialization time
    ESP_LOGD("hdsp2112", "Display not initialized!");
    return false;
  }
  return true;
}

void HDSP21xx::selftest() {

    // Bit 6 = 1 starts the self-test
    uint8_t control_word = 0x40;
    sendCtrl(control_word);

    // Do NOT access the display while self-test is running.
    // At 58 kHz it takes approximately 4.5 seconds.

    for (int i = 0; i < 60; i++) {
      // vTaskDelay(pdMS_TO_TICKS(100));
    }

    initStartedAt = esp_timer_get_time() / 1000ULL;
}

void HDSP21xx::fulltest() {

  if(!isInitialized()) {
    return;
  }

  int delaytime = 15;
  for (int i = 0; i < 8; i++) {

    for (char c = 'a'; c <= 'z'; c++) {
      setChar(c, i);
      delay_ms(delaytime);
    }

    for (char c = 'A'; c <= 'Z'; c++) {
      setChar(c, i);
      delay_ms(delaytime);
    }

    for (char c = '0'; c <= '9'; c++) {
      setChar(c, i);
      delay_ms(delaytime);
    }

    setChar(0, i);
    delay_ms(delaytime);
  }
}

void HDSP21xx::setChar(uint8_t data, uint8_t position) {


    if(!isInitialized()) {
      return;
    }
    // early exit if no update is required.
    if (text_buffer[position] == (char)data) {
      return;
    }

    // Write ASCII character to character position 0.
    //
    // Character RAM:
    // FL = 1
    // A4 = 1
    // A3 = 1
    // A2:A0 = character position
    //
    
    // Position 0 -> 11000b = 0x18
    uint8_t pos = 0x18 + position;

    text_buffer[position] = (char)data;
    gpio_set_level(static_cast<gpio_num_t>(HDSP211xFL), 1);
    sendByte(data, pos);


    ESP_LOGI("hdsp2112", "setText: %c; %d", data, position);
}

void HDSP21xx::setText(const std::string &text) {

  if(!isInitialized()) {
    return;
  }
  std::string padded = text + "        ";
  for (uint8_t i = 0; i < 8; i++) {
    setChar((uint8_t)padded[i], i);
    setChar((uint8_t)padded[i], i);
  }
}

void HDSP21xx::marquee(const std::string &text, int delayInMs) {

  if(!isInitialized()) {
    return;
  }
  for (size_t i = 0; i <= text.length(); i++) {
        std::string rotated = text.substr(i) + text.substr(0, i);

        setText(rotated);
        delay_ms(delayInMs);
    }
}


void HDSP21xx::setBrightness(int level) {

  if(!isInitialized()) {
    return;
  }
  int data = 0b00000000;

  switch (level) {
      case 0:
        data = 0b00000111; // off
        break;
      case 1:
        data = 0b00000110; // 13%
        break;
      case 2:
        data = 0b00000101; // 20%
        break;
      case 3:
        data = 0b00000100; // 27%
        break;
      case 4:
        data = 0b00000011; // 40%
        break;
      case 5:
        data = 0b00000010; // 53%
        break;
      case 6:
        data = 0b00000001; // 80%
        break;
      default:
        data = 0b00000000; // 100%
        break;
    }
    sendCtrl(data);
}
