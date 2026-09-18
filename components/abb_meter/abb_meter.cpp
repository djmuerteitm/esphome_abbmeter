#include "abb_meter.h"

#include <algorithm>
#include <cstdint>

namespace esphome {
namespace abb_meter {

void ABBMeterSensor::update() {
  this->send(0x03, this->register_address_, this->register_count_);
}

void ABBMeterSensor::on_modbus_data(const std::vector<uint8_t> &data) {
  const size_t byte_count = static_cast<size_t>(this->register_count_) * 2;
  if (data.size() != byte_count) {
    ESP_LOGW("abb_meter", "Registro 0x%04X: recibidos %u bytes; esperados %u",
             this->register_address_, data.size(), byte_count);
    return;
  }

  std::vector<uint8_t> value = data;
  if (this->swap_words_ && value.size() > 2) {
    for (size_t offset = 0; offset < value.size(); offset += 4) {
      std::swap(value[offset], value[offset + 2]);
      std::swap(value[offset + 1], value[offset + 3]);
    }
  }

  uint64_t raw = 0;
  for (const uint8_t byte : value) raw = (raw << 8) | byte;

  int64_t signed_raw = static_cast<int64_t>(raw);
  const uint8_t bits = static_cast<uint8_t>(value.size() * 8);
  if ((this->value_type_.front() == 'S') && (raw & (uint64_t{1} << (bits - 1)))) {
    // Two's-complement sign extension. Shifting by 64 is undefined, so the
    // 64-bit case uses the normal uint64_t-to-int64_t conversion directly.
    if (bits < 64) signed_raw = static_cast<int64_t>(raw - (uint64_t{1} << bits));
  }
  const double number = this->value_type_.front() == 'S' ? signed_raw : raw;
  this->publish_state(static_cast<float>(number * this->scale_));
}

}  // namespace abb_meter
}  // namespace esphome
