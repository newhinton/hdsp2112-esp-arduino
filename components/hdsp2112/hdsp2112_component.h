#pragma once

#include "esphome/core/component.h"
#include "HDSP21xx.h"

namespace esphome {
  namespace hdsp2112 {

    class HDSP2112Component : public Component {
    public:
      HDSP2112Component() = default;

      void setup() override;
      void dump_config() override;

      void set_text(const std::string &text);
      void setChar(const char *text, uint8_t position);
      void set_brightness(int level);
      void self_test();
      void full_test();
      void marquee(const std::string &text, int delay_ms);

    protected:
      HDSP21xx driver_;
    };

  }  // namespace hdsp2112
}  // namespace esphome
