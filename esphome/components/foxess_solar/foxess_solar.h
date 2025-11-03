#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include <array>
#include <cstddef>

#define SENSOR_SETTER(X) \
  void set_##X##_sensor(sensor::Sensor *sensor) { this->X##_ = sensor; }

#define PHASE_SENSOR_SETTER(X) \
  void set_phase_##X##_sensor(uint8_t phase, sensor::Sensor *X##_sensor) { \
    this->phases_[phase].X##_sensor_ = X##_sensor; \
  }

#define PV_SENSOR_SETTER(X) \
  void set_pv_##X##_sensor(uint8_t pv, sensor::Sensor *X##_sensor) { this->pvs_[pv].X##_sensor_ = X##_sensor; }

namespace esphome {
namespace foxess_solar {

static const long INVERTER_TIMEOUT = 300000;  // ms
static const std::size_t BUFFER_SIZE = 256;

static const std::array<uint8_t, 3> MSG_HEADER = {0x7E, 0x7E, 0x02};
static const std::array<uint8_t, 2> MSG_FOOTER = {0xE7, 0xE7};

// All offsets in one place
struct MsgOffset {
  // powers
  static constexpr std::size_t GRID_POWER_MSB    = 9;
  static constexpr std::size_t GRID_POWER_LSB    = 10;
  static constexpr std::size_t GEN_POWER_MSB     = 11;
  static constexpr std::size_t GEN_POWER_LSB     = 12;
  static constexpr std::size_t LOAD_POWER_MSB    = 13;
  static constexpr std::size_t LOAD_POWER_LSB    = 14;

  // phases
  static constexpr std::size_t PHASE1_BASE       = 15;  // 15..22
  static constexpr std::size_t PHASE_STRIDE      = 8;   // 23, 31

  // PVs
  static constexpr std::size_t PV1_BASE          = 39;  // 39..42
  static constexpr std::size_t PV_STRIDE         = 6;   // 45, 51, 57

  // temps
  static constexpr std::size_t BOOST_TEMP_MSB    = 63;
  static constexpr std::size_t BOOST_TEMP_LSB    = 64;
  static constexpr std::size_t INVERTER_TEMP_MSB = 65;
  static constexpr std::size_t INVERTER_TEMP_LSB = 66;
  static constexpr std::size_t AMBIENT_TEMP_MSB  = 67;
  static constexpr std::size_t AMBIENT_TEMP_LSB  = 68;

  // energy
  static constexpr std::size_t ENERGY_DAY_MSB    = 69;
  static constexpr std::size_t ENERGY_DAY_LSB    = 70;
  static constexpr std::size_t TOTAL_ENERGY_MSB0 = 71;  // 71..74

  // error block
  static constexpr std::size_t ERROR_BLOCK_BEGIN = 125;
  static constexpr std::size_t ERROR_BLOCK_END   = 157; // exclusive
};

class FoxessSolar : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;

  SENSOR_SETTER(loads_power)
  SENSOR_SETTER(grid_power)
  SENSOR_SETTER(generation_power)
  SENSOR_SETTER(boost_temp)
  SENSOR_SETTER(inverter_temp)
  SENSOR_SETTER(ambient_temp)
  SENSOR_SETTER(energy_production_day)
  SENSOR_SETTER(total_energy_production)
  SENSOR_SETTER(inverter_status)

  PHASE_SENSOR_SETTER(voltage)
  PHASE_SENSOR_SETTER(current)
  PHASE_SENSOR_SETTER(active_power)
  PHASE_SENSOR_SETTER(frequency)

  PV_SENSOR_SETTER(voltage)
  PV_SENSOR_SETTER(current)
  PV_SENSOR_SETTER(active_power)

  void set_fc_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

 protected:
  void parse_message();
  void set_inverter_mode(uint32_t mode);
  optional<bool> check_msg();

  // now just simple helpers, no span:
  void publish_zero_phases();
  void publish_zero_pvs();

  GPIOPin *flow_control_pin_{nullptr};
  uint32_t millis_lastmessage_{0};
  std::array<uint8_t, BUFFER_SIZE> input_buffer{};
  std::size_t buffer_end{0};

  uint32_t inverter_mode_{99};
  sensor::Sensor *inverter_status_{nullptr};  // 0=Offline, 1=Online, 2=Error, 99=Waiting...

  struct SolarPhase {
    sensor::Sensor *voltage_sensor_{nullptr};
    sensor::Sensor *current_sensor_{nullptr};
    sensor::Sensor *active_power_sensor_{nullptr};
    sensor::Sensor *frequency_sensor_{nullptr};
  } phases_[3];

  struct SolarPV {
    sensor::Sensor *voltage_sensor_{nullptr};
    sensor::Sensor *current_sensor_{nullptr};
    sensor::Sensor *active_power_sensor_{nullptr};
  } pvs_[4];

  sensor::Sensor *loads_power_{nullptr};
  sensor::Sensor *grid_power_{nullptr};
  sensor::Sensor *generation_power_{nullptr};

  sensor::Sensor *boost_temp_{nullptr};
  sensor::Sensor *inverter_temp_{nullptr};
  sensor::Sensor *ambient_temp_{nullptr};
  sensor::Sensor *energy_production_day_{nullptr};
  sensor::Sensor *total_energy_production_{nullptr};
};

}  // namespace foxess_solar
}  // namespace esphome
