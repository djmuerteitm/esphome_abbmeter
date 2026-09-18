#pragma once

#include "esphome/components/modbus/modbus.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace abb_meter {

class ABBMeterSensor : public sensor::Sensor,
                       public PollingComponent,
                       public modbus::ModbusDevice {
 public:
  void set_register_address(uint16_t value) { this->register_address_ = value; }
  void set_value_type(const std::string &value) { this->value_type_ = value; }
  void set_scale(float value) { this->scale_ = value; }
  void set_swap_words(bool value) { this->swap_words_ = value; }
  void set_register_count(uint8_t value) { this->register_count_ = value; }

  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;

 protected:
  uint16_t register_address_{0};
  uint8_t register_count_{1};
  std::string value_type_{"U_WORD"};
  float scale_{1.0f};
  bool swap_words_{false};
};

}  // namespace abb_meter
}  // namespace esphome
