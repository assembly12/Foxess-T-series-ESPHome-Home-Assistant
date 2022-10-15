#include "esphome.h"

class foxesscomponent : public PollingComponent, public Sensor, public UARTDevice {
  public:
    foxesscomponent(UARTComponent *parent) : PollingComponent(600), UARTDevice(parent) {}
  
  Sensor *frame_header = new Sensor;
  Sensor *function_code = new Sensor;
  Sensor *time_stamp = new Sensor;
  Sensor *data_length = new Sensor;
  Sensor *grid_power = new Sensor;
  Sensor *generation_power = new Sensor;
  Sensor *loads_power = new Sensor;
  Sensor *grid_voltage_r = new Sensor;
  Sensor *grid_current_r = new Sensor;
  Sensor *grid_frequency_r = new Sensor;
  Sensor *grid_power_r = new Sensor;
  Sensor *grid_voltage_s = new Sensor;
  Sensor *grid_current_s = new Sensor;
  Sensor *grid_frequency_s = new Sensor;
  Sensor *grid_power_s = new Sensor;
  Sensor *grid_voltage_T = new Sensor;
  Sensor *grid_current_T = new Sensor;
  Sensor *grid_frequency_T = new Sensor;
  Sensor *grid_power_T = new Sensor;
  Sensor *pv1_voltage = new Sensor;
  Sensor *pv1_current = new Sensor;
  //Sensor *pv1_power = new Sensor;
  Sensor *pv2_voltage = new Sensor;
  Sensor *pv2_current = new Sensor;
  //Sensor *pv2_power = new Sensor;
  Sensor *pv3_voltage = new Sensor;
  Sensor *pv3_current = new Sensor;
  //Sensor *pv3_power = new Sensor;
  Sensor *pv4_voltage = new Sensor;
  Sensor *pv4_current = new Sensor;
  //Sensor *pv4_power = new Sensor;
  Sensor *boost_temperature = new Sensor;
  Sensor *inverter_temperature = new Sensor;
  Sensor *ambient_temperature = new Sensor;
  Sensor *today_yield = new Sensor;
  Sensor *generation_total = new Sensor;
  Sensor *from_grid_yield_generation = new Sensor;
  Sensor *feedin_generation1 = new Sensor;
  Sensor *feedin_generation2 = new Sensor;
  Sensor *consumption_generation1 = new Sensor;
  Sensor *consumption_generation2 = new Sensor;
  Sensor *loads_generation = new Sensor;
  Sensor *eps_voltage_r = new Sensor;
  Sensor *eps_current_r = new Sensor;
  Sensor *eps_power_r = new Sensor;

  void setup() override {

  }

  std::vector<int> bytes;

  //void loop() override {
  void update() {
    int total_message_length = 0;
    while(available() > 0) {
      bytes.push_back(read());
      //ESP_LOGD("custom", "reading bytes");

      //make sure at least 3 header bytes are available to check
      if(bytes.size() < 3){
        continue;
      }

      if(bytes[0] != 0x7E || bytes[1] != 0x7E || bytes[2] != 0x02) {
        bytes.erase(bytes.begin()); //remove first byte from buffer
        //buffer will never get above 3 until the header is correct
        continue;
      }
      else {
      }

      if(bytes.size() == 9) { //>=
        TwoByte message_length;
        message_length.Byte[0] = bytes[8];
        message_length.Byte[1] = bytes[7];
        total_message_length = message_length.UInt16 + 13;
        ESP_LOGI("custom", "User data length: %i", message_length.UInt16);
        ESP_LOGI("custom", "Total message length: %i", total_message_length);
        //continue;
        }

      if(bytes.size() == total_message_length) { //>=
        if(bytes[total_message_length-1] != 0xE7 || bytes[total_message_length-2] != 0xE7) {
          bytes.clear();
          ESP_LOGD("custom", "error in reading message");
          continue;
        }
        ESP_LOGD("custom", "succesfully read realtime data");
        TwoByte grid_power_value;
        grid_power_value.Byte[0] = bytes[10];
        grid_power_value.Byte[1] = bytes[9];
        id(grid_power).publish_state(grid_power_value.UInt16);

        TwoByte generation_power_value;
        generation_power_value.Byte[0] = bytes[12];
        generation_power_value.Byte[1] = bytes[11];
        id(generation_power).publish_state(generation_power_value.UInt16);

        TwoByte loads_power_value;
        loads_power_value.Byte[0] = bytes[14];
        loads_power_value.Byte[1] = bytes[13];
        id(loads_power).publish_state(loads_power_value.UInt16);

        TwoByte grid_voltage_r_value;
        grid_voltage_r_value.Byte[0] = bytes[16];
        grid_voltage_r_value.Byte[1] = bytes[15];
        id(grid_voltage_r).publish_state(grid_voltage_r_value.UInt16);

        TwoByte grid_current_r_value;
        grid_current_r_value.Byte[0] = bytes[18];
        grid_current_r_value.Byte[1] = bytes[17];
        id(grid_current_r).publish_state(grid_current_r_value.UInt16);

        TwoByte grid_frequency_r_value;
        grid_frequency_r_value.Byte[0] = bytes[20];
        grid_frequency_r_value.Byte[1] = bytes[19];
        id(grid_frequency_r).publish_state(grid_frequency_r_value.UInt16);

        TwoByte grid_power_r_value;
        grid_power_r_value.Byte[0] = bytes[22];
        grid_power_r_value.Byte[1] = bytes[21];
        id(grid_power_r).publish_state(grid_power_r_value.UInt16);

        TwoByte grid_voltage_s_value;
        grid_voltage_s_value.Byte[0] = bytes[24];
        grid_voltage_s_value.Byte[1] = bytes[23];
        id(grid_voltage_s).publish_state(grid_voltage_s_value.UInt16);

        TwoByte grid_current_s_value;
        grid_current_s_value.Byte[0] = bytes[26];
        grid_current_s_value.Byte[1] = bytes[25];
        id(grid_current_s).publish_state(grid_current_s_value.UInt16);

        TwoByte grid_frequency_s_value;
        grid_frequency_s_value.Byte[0] = bytes[28];
        grid_frequency_s_value.Byte[1] = bytes[27];
        id(grid_frequency_s).publish_state(grid_frequency_s_value.UInt16);

        TwoByte grid_power_s_value;
        grid_power_s_value.Byte[0] = bytes[30];
        grid_power_s_value.Byte[1] = bytes[29];
        id(grid_power_s).publish_state(grid_power_s_value.UInt16);

        TwoByte grid_voltage_T_value;
        grid_voltage_T_value.Byte[0] = bytes[32];
        grid_voltage_T_value.Byte[1] = bytes[31];
        id(grid_voltage_T).publish_state(grid_voltage_T_value.UInt16);

        TwoByte grid_current_T_value;
        grid_current_T_value.Byte[0] = bytes[34];
        grid_current_T_value.Byte[1] = bytes[33];
        id(grid_current_T).publish_state(grid_current_T_value.UInt16);

        TwoByte grid_frequency_T_value;
        grid_frequency_T_value.Byte[0] = bytes[36];
        grid_frequency_T_value.Byte[1] = bytes[35];
        id(grid_frequency_T).publish_state(grid_frequency_T_value.UInt16);

        TwoByte grid_power_T_value;
        grid_power_T_value.Byte[0] = bytes[38];
        grid_power_T_value.Byte[1] = bytes[37];
        id(grid_power_T).publish_state(grid_power_T_value.UInt16);

        TwoByte pv1_voltage_value;
        pv1_voltage_value.Byte[0] = bytes[40];
        pv1_voltage_value.Byte[1] = bytes[39];
        id(pv1_voltage).publish_state(pv1_voltage_value.UInt16);

        TwoByte pv1_current_value;
        pv1_current_value.Byte[0] = bytes[42];
        pv1_current_value.Byte[1] = bytes[41];
        id(pv1_current).publish_state(pv1_current_value.UInt16);

        TwoByte pv2_voltage_value;
        pv2_voltage_value.Byte[0] = bytes[46];
        pv2_voltage_value.Byte[1] = bytes[45];
        id(pv2_voltage).publish_state(pv2_voltage_value.UInt16);

        TwoByte pv2_current_value;
        pv2_current_value.Byte[0] = bytes[48];
        pv2_current_value.Byte[1] = bytes[47];
        id(pv2_current).publish_state(pv2_current_value.UInt16);

        TwoByte pv3_voltage_value;
        pv3_voltage_value.Byte[0] = bytes[52];
        pv3_voltage_value.Byte[1] = bytes[51];
        id(pv3_voltage).publish_state(pv3_voltage_value.UInt16);

        TwoByte pv3_current_value;
        pv3_current_value.Byte[0] = bytes[54];
        pv3_current_value.Byte[1] = bytes[53];
        id(pv3_current).publish_state(pv3_current_value.UInt16);

        TwoByte pv4_voltage_value;
        pv4_voltage_value.Byte[0] = bytes[58];
        pv4_voltage_value.Byte[1] = bytes[57];
        id(pv4_voltage).publish_state(pv4_voltage_value.UInt16);

        TwoByte pv4_current_value;
        pv4_current_value.Byte[0] = bytes[60];
        pv4_current_value.Byte[1] = bytes[59];
        id(pv4_current).publish_state(pv4_current_value.UInt16);

        TwoByte boost_temperature_value;
        boost_temperature_value.Byte[0] = bytes[64];
        boost_temperature_value.Byte[1] = bytes[63];
        id(boost_temperature).publish_state(boost_temperature_value.UInt16);

        TwoByte inverter_temperature_value;
        inverter_temperature_value.Byte[0] = bytes[66];
        inverter_temperature_value.Byte[1] = bytes[65];
        id(inverter_temperature).publish_state(inverter_temperature_value.UInt16);

        TwoByte ambient_temperature_value;
        ambient_temperature_value.Byte[0] = bytes[68];
        ambient_temperature_value.Byte[1] = bytes[67];
        id(ambient_temperature).publish_state(ambient_temperature_value.UInt16);

        TwoByte today_yield_value;
        today_yield_value.Byte[0] = bytes[70];
        today_yield_value.Byte[1] = bytes[69];
        id(today_yield).publish_state(today_yield_value.UInt16);

        uint32_t generation_total_value = int(
            (unsigned char)(bytes[71]) << 24 |
            (unsigned char)(bytes[72]) << 16 |
            (unsigned char)(bytes[73]) << 8 |
            (unsigned char)(bytes[74]));
        id(generation_total).publish_state(generation_total_value);

        bytes.clear();
      }
      else {
      }
    }
  }

  typedef union
  {
    unsigned char Byte[2];
    int16_t Int16;
    uint16_t UInt16;
    unsigned char UChar;
    char Char;
  }TwoByte;};
