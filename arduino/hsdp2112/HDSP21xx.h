// HDSP21xx.h
#ifndef HDSP21xx_h
#define HDSP21xx_h


#define HDSP211xA0 16
#define HDSP211xA1 17
#define HDSP211xA2 18
#define HDSP211xA3 19
#define HDSP211xA4 21

#define HDSP211xD0 12
#define HDSP211xD1 14
#define HDSP211xD2 26
#define HDSP211xD3 27
#define HDSP211xD4 32
#define HDSP211xD5 33
#define HDSP211xD6 15
#define HDSP211xD7 13

#define HDSP211xFL 2
#define HDSP211xCE 0
#define HDSP211xWR 22
//#define HDSP211xRD 0 // always set high
#define HDSP211xCLS 23
#define HDSP211xRST 4

#include <Arduino.h>
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
    int e_pulse = 300;
    void setBit(uint8_t pin, uint8_t mask, uint8_t data);
    void sendByte(int data, int adr);
    void sendCtrl(uint8_t control_word);
    void displayInit();

  public:
    HDSP21xx();
    void prepare();
    void selftest();
    void fulltest();
    void setChar(uint8_t data, uint8_t position);
    void setText(String text);
    void marquee(String text, int delayInMs);
    void setBrightness(int level);
};

#endif
