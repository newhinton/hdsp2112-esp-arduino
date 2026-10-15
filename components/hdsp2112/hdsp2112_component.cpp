#include "hdsp2112_component.h"
#include "esphome/core/log.h"

namespace esphome {
  namespace hdsp2112 {

    static const char *const TAG = "hdsp2112";

    void HDSP2112Component::setup() {
      ESP_LOGCONFIG(TAG, "Initializing HDSP21xx");
      driver_.prepare();
    }

    void HDSP2112Component::dump_config() {
      ESP_LOGCONFIG(TAG, "HDSP21xx");
    }

    void HDSP2112Component::set_text(const std::string &text) {
      driver_.setText(text);
    }


    /*void HDSP2112Component::setChar(uint8_t data, uint8_t position) {
      driver_.setChar(data, position);
    }*/

    void HDSP2112Component::setChar(const char *text, uint8_t position) {
      if (text == nullptr || text[0] == '\0')
        return;

      //this->setChar(static_cast<uint8_t>(text[0]), position);
      driver_.setChar(static_cast<uint8_t>(text[0]), position);
    }

    void HDSP2112Component::set_brightness(int level) {
      driver_.setBrightness(level);
    }

    void HDSP2112Component::self_test() {
      driver_.selftest();
    }

    void HDSP2112Component::full_test() {
      driver_.fulltest();
    }

    void HDSP2112Component::marquee(const std::string &text, int delay_ms) {
      driver_.marquee(text, delay_ms);
    }

  }  // namespace hdsp2112
}  // namespace esphome
