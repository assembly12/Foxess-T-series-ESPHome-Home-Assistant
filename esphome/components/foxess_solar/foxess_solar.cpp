#include <algorithm>
#include <cmath>

#include "foxess_solar.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace foxess_solar {

namespace {
	
static const char *const TAG = "foxess_solar";

static inline void publish_sensor_state(sensor::Sensor *sensor, int32_t raw, float scale) {
  if (!sensor)
    return;
  if (std::isnan(scale)) {
    sensor->publish_state(NAN);
  } else {
    sensor->publish_state(static_cast<float>(raw) * scale);
  }
}

static inline int16_t decode_int16(uint8_t msb, uint8_t lsb) {
  uint16_t u = static_cast<uint16_t>((static_cast<uint16_t>(msb) << 8) | lsb);
  return static_cast<int16_t>(u);
}

static inline uint16_t decode_uint16(uint8_t msb, uint8_t lsb) {
  return static_cast<uint16_t>((static_cast<uint16_t>(msb) << 8) | lsb);
}

static inline uint32_t decode_uint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
  return (static_cast<uint32_t>(b0) << 24) |
         (static_cast<uint32_t>(b1) << 16) |
         (static_cast<uint32_t>(b2) << 8)  |
         (static_cast<uint32_t>(b3));
}

}

// -----------------------------------------------------------------------------

void FoxessSolar::setup() {
  ESP_LOGVV(TAG, "setup start");

  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }

  this->millis_lastmessage_ = millis();
}

void FoxessSolar::publish_zero_phases() {
  for (auto &ph : this->phases_) {
    publish_sensor_state(ph.voltage_sensor_, 0, 1.0f);
    publish_sensor_state(ph.current_sensor_, 0, 1.0f);
    publish_sensor_state(ph.frequency_sensor_, 0, NAN);
    publish_sensor_state(ph.active_power_sensor_, 0, 1.0f);
  }
}

void FoxessSolar::publish_zero_pvs() {
  for (auto &pv : this->pvs_) {
    publish_sensor_state(pv.voltage_sensor_, 0, 1.0f);
    publish_sensor_state(pv.current_sensor_, 0, 1.0f);
    publish_sensor_state(pv.active_power_sensor_, 0, 1.0f);
  }
}

void FoxessSolar::update() {
  ESP_LOGVV(TAG, "update start");

  // handle timeout
  if (millis() - this->millis_lastmessage_ >= INVERTER_TIMEOUT) {
    if (this->inverter_mode_ != 0) {
      this->set_inverter_mode(0);  // OFFLINE

      publish_sensor_state(this->generation_power_, 0, 1.0f);
      publish_sensor_state(this->grid_power_, 0, NAN);
      publish_sensor_state(this->loads_power_, 0, NAN);

      publish_sensor_state(this->boost_temp_, 0, NAN);
      publish_sensor_state(this->ambient_temp_, 0, NAN);
      publish_sensor_state(this->inverter_temp_, 0, NAN);

      this->publish_zero_phases();
      this->publish_zero_pvs();
    }
  }

  // read bytes
  while (this->available() > 0) {
    this->read_byte(&this->input_buffer[this->buffer_end]);
    optional<bool> state = this->check_msg();

    if (!state.has_value()) {
      // still reading
      this->buffer_end++;
    } else if (*state) {
      // good complete message
      this->status_clear_warning();
      this->parse_message();
      this->buffer_end = 0;
      this->millis_lastmessage_ = millis();
    } else {
      // invalid message
      this->buffer_end = 0;
    }
  }
}

// Return std::nullopt -> more bytes needed
// Return true         -> complete & valid
// Return false        -> invalid, clear
optional<bool> FoxessSolar::check_msg() {
  const std::size_t idx = this->buffer_end;

  // 1) header check
  if (idx <= 2) {
    if (this->input_buffer[idx] == MSG_HEADER[idx]) {
      return {};
    } else {
      ESP_LOGVV(TAG, "header mismatch at %d: 0x%x", (int) idx, this->input_buffer[idx]);
      return false;
    }
  }

  // 2) need length bytes
  if (idx < 9) {
    return {};
  }

  const uint16_t payload_len = decode_uint16(this->input_buffer[7], this->input_buffer[8]);
  const uint16_t msg_len     = payload_len + 13;

  if (msg_len > BUFFER_SIZE) {
    ESP_LOGE(TAG, "message too long: %u", msg_len);
    this->status_set_warning();
    return false;
  }

  // not fully read yet?
  if (idx + 1 < msg_len) {
    ESP_LOGVV(TAG, "message incomplete: have %u, need %u", (unsigned)(idx + 1), (unsigned)msg_len);
    return {};
  }

  // footer
  if (this->input_buffer[msg_len - 2] != MSG_FOOTER[0] ||
      this->input_buffer[msg_len - 1] != MSG_FOOTER[1]) {
    ESP_LOGE(TAG, "bad footer: ... 0x%x 0x%x",
             this->input_buffer[msg_len - 2], this->input_buffer[msg_len - 1]);
    this->status_set_warning();
    return false;
  }

 // CRC
  const uint16_t calc_crc = crc16(&this->input_buffer[2], msg_len - 6);
  const uint16_t msg_crc  = decode_uint16(this->input_buffer[msg_len - 3],
                                          this->input_buffer[msg_len - 4]);
  if (calc_crc != msg_crc) {
    ESP_LOGE(TAG, "checksum mismatch, calc: 0x%x, msg: 0x%x", calc_crc, msg_crc);
    this->status_set_warning();
    return false;
  }

  return true;
}

void FoxessSolar::parse_message() {
  ESP_LOGVV(TAG, "parse_message start");

  const std::size_t total_len = this->buffer_end + 1;
  auto &msg = this->input_buffer;

  if (total_len != 163) {
    ESP_LOGW(TAG, "unexpected message length: %u (expected 163)", (unsigned) total_len);
    this->status_set_warning();
  }

  // powers
  publish_sensor_state(this->grid_power_,
                       decode_int16(msg[MsgOffset::GRID_POWER_MSB],
                                    msg[MsgOffset::GRID_POWER_LSB]),
                       1.0f);
  publish_sensor_state(this->generation_power_,
                       decode_uint16(msg[MsgOffset::GEN_POWER_MSB],
                                     msg[MsgOffset::GEN_POWER_LSB]),
                       1.0f);
  publish_sensor_state(this->loads_power_,
                       decode_int16(msg[MsgOffset::LOAD_POWER_MSB],
                                    msg[MsgOffset::LOAD_POWER_LSB]),
                       1.0f);

  // phases
  for (std::size_t i = 0; i < 3; i++) {
    const std::size_t base = MsgOffset::PHASE1_BASE + i * MsgOffset::PHASE_STRIDE;
    auto &ph = this->phases_[i];

    publish_sensor_state(ph.voltage_sensor_,   decode_uint16(msg[base + 0], msg[base + 1]), 0.1f);
    publish_sensor_state(ph.current_sensor_,   decode_uint16(msg[base + 2], msg[base + 3]), 0.1f);
    publish_sensor_state(ph.frequency_sensor_, decode_uint16(msg[base + 4], msg[base + 5]), 0.01f);
    publish_sensor_state(ph.active_power_sensor_, decode_uint16(msg[base + 6], msg[base + 7]), 1.0f);
  }

  // pvs
  for (std::size_t i = 0; i < 4; i++) {
    const std::size_t base = MsgOffset::PV1_BASE + i * MsgOffset::PV_STRIDE;
    const uint16_t volt = decode_uint16(msg[base + 0], msg[base + 1]);
    const uint16_t amps = decode_uint16(msg[base + 2], msg[base + 3]);
    auto &pv = this->pvs_[i];

    publish_sensor_state(pv.voltage_sensor_, volt, 0.1f);
    publish_sensor_state(pv.current_sensor_, amps, 0.1f);
    publish_sensor_state(pv.active_power_sensor_,
                         static_cast<int32_t>(volt) * static_cast<int32_t>(amps),
                         0.01f);
  }

  // temps
  publish_sensor_state(this->boost_temp_,
                       decode_int16(msg[MsgOffset::BOOST_TEMP_MSB],
                                    msg[MsgOffset::BOOST_TEMP_LSB]),
                       1.0f);
  publish_sensor_state(this->inverter_temp_,
                       decode_int16(msg[MsgOffset::INVERTER_TEMP_MSB],
                                    msg[MsgOffset::INVERTER_TEMP_LSB]),
                       1.0f);
  publish_sensor_state(this->ambient_temp_,
                       decode_int16(msg[MsgOffset::AMBIENT_TEMP_MSB],
                                    msg[MsgOffset::AMBIENT_TEMP_LSB]),
                       1.0f);

  // energy
  publish_sensor_state(this->energy_production_day_,
                       decode_uint16(msg[MsgOffset::ENERGY_DAY_MSB],
                                     msg[MsgOffset::ENERGY_DAY_LSB]),
                       0.1f);
  publish_sensor_state(this->total_energy_production_,
                       decode_uint32(msg[MsgOffset::TOTAL_ENERGY_MSB0],
                                     msg[MsgOffset::TOTAL_ENERGY_MSB0 + 1],
                                     msg[MsgOffset::TOTAL_ENERGY_MSB0 + 2],
                                     msg[MsgOffset::TOTAL_ENERGY_MSB0 + 3]),
                       0.1f);

  // error block check
  if (!std::all_of(msg.begin() + MsgOffset::ERROR_BLOCK_BEGIN,
                   msg.begin() + MsgOffset::ERROR_BLOCK_END,
                   [](uint8_t b) { return b == 0; })) {
    this->set_inverter_mode(2);  // ERROR
    return;
  }

  this->set_inverter_mode(1);  // ONLINE
}

void FoxessSolar::set_inverter_mode(uint32_t mode) {
  this->inverter_mode_ = mode;
  if (this->inverter_status_ != nullptr)
    this->inverter_status_->publish_state(mode);
}

}  // namespace foxess_solar
}  // namespace esphome
