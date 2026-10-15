// HDSP21xx.h
#ifndef HDSP21xx_h
#define HDSP21xx_h

#include <cstdint>
#include <string>

#include "driver/gpio.h"
#include "xtensa/core-macros.h" // Used for nanosecond delay


static inline void delay_4xns(int times) {
    // XTHAL_GET_CCOUNT() accurate clock cycle
    // 1 cycles / 4 ns = 250 MHz
    // the cpu runs at 240Mhz. So a cycle will take slightly longer than 4ns
    uint32_t start = XTHAL_GET_CCOUNT();
    while ((uint32_t)(XTHAL_GET_CCOUNT() - start) < times) {
        __asm__ volatile ("nop");
    }
}

static inline void delay_160ns() {
    delay_4xns(40); // roughly 160ns
}

static inline void delay_12ns() {
    delay_4xns(3); // roughly 12ns
}

static inline void delay_20ns() {
    delay_4xns(5); // roughly 20ns
}

class HDSP21xx {


  private:
    // Address pins
    uint8_t HDSP211xA0 = 16;
    uint8_t HDSP211xA1 = 17;
    uint8_t HDSP211xA2 = 18;
    uint8_t HDSP211xA3 = 19;
    uint8_t HDSP211xA4 = 21;

    // Data pins
    uint8_t HDSP211xD0 = 12;
    uint8_t HDSP211xD1 = 14;
    uint8_t HDSP211xD2 = 26;
    uint8_t HDSP211xD3 = 27;
    uint8_t HDSP211xD4 = 32;
    uint8_t HDSP211xD5 = 33;
    uint8_t HDSP211xD6 = 15;
    uint8_t HDSP211xD7 = 13;

    // Control pins
    uint8_t HDSP211xFL  = 2;
    uint8_t HDSP211xCE  = 0;
    uint8_t HDSP211xWR  = 22;
    uint8_t HDSP211xCLS = 23;
    uint8_t HDSP211xRST = 4;
    //#define HDSP211xRD 0 // always set high

    int e_pulse = 300;
    int initStartedAt = 0;
    void setBit(uint8_t pin, uint8_t mask, uint8_t data);
    void sendByte(int data, int adr);
    void sendCtrl(uint8_t control_word);
    void displayInit();
    std::string text_buffer = "        ";

  public:
    HDSP21xx();
    void prepare();
    bool isInitialized();
    void selftest();
    void fulltest();
    void setChar(uint8_t data, uint8_t position);
    void setText(const std::string &text);
    void marquee(const std::string &text, int delayInMs);
    void setBrightness(int level);
};

#endif
