#include "esphome.h"

unsigned long millis_lastmessage = 0;
const long inverter_timeout = 300000;
int inverter_mode = 99; //0=Offline, 1=Online, 2=Error, 99=Waiting for response...

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
  Sensor *pv1_power = new Sensor;
  Sensor *pv2_voltage = new Sensor;
  Sensor *pv2_current = new Sensor;
  Sensor *pv2_power = new Sensor;
  Sensor *pv3_voltage = new Sensor;
  Sensor *pv3_current = new Sensor;
  Sensor *pv3_power = new Sensor;
  Sensor *pv4_voltage = new Sensor;
  Sensor *pv4_current = new Sensor;
  Sensor *pv4_power = new Sensor;
  Sensor *boost_temperature = new Sensor;
  Sensor *inverter_temperature = new Sensor;
  Sensor *ambient_temperature = new Sensor;
  Sensor *today_yield = new Sensor;
  Sensor *generation_total = new Sensor;
  Sensor *from_grid_yield_generation = new Sensor;
  Sensor *inverter_state = new Sensor;

  void setup() override {
    id(inverter_state).publish_state(inverter_mode);
  }

  std::vector<int> bytes;

  //void loop() override {
  void update() {
    if(millis_lastmessage + inverter_timeout < millis()) {
      inverter_mode = 0; //offline
      id(inverter_state).publish_state(inverter_mode);
    }
    while(available() > 0) {
      bytes.push_back(read());
      //ESP_LOGD("custom", "reading bytes");

      //make sure at least 9 header bytes are available to check
      if(bytes.size() < 9){
        continue;
      }

      if(bytes[0] != 0x7E || bytes[1] != 0x7E || bytes[2] != 0x02) {
        bytes.erase(bytes.begin()); //remove first byte from buffer
        //buffer will never get above 9 until the header is correct
        continue;
      }
      else {
      }

      //if(bytes.size() == 9) { //>=
        TwoByte message_length;
        message_length.Byte[0] = bytes[8];
        message_length.Byte[1] = bytes[7];
        uint16_t total_message_length = message_length.UInt16 + 13;
      //}

      if(bytes.size() == total_message_length) { //>=
      //if(bytes.size() == 165) { //>=
        if(bytes[total_message_length-1] != 0xE7 || bytes[total_message_length-2] != 0xE7) {
        //if(bytes[164] != 0xE7 || bytes[163] != 0xE7) {
          bytes.clear();
          ESP_LOGD("custom", "error in reading message");
          continue;
        }
        ESP_LOGD("custom", "succesfully read realtime data");
        ESP_LOGI("custom", "User data length: %i", message_length.UInt16);
        ESP_LOGI("custom", "Total message length: %i", total_message_length);
        millis_lastmessage = millis();
        inverter_mode = 1;

        TwoByte grid_power_value;
        grid_power_value.Byte[0] = bytes[10];
        grid_power_value.Byte[1] = bytes[9];
        id(grid_power).publish_state(grid_power_value.UInt16);
        delay(50);

        TwoByte generation_power_value;
        generation_power_value.Byte[0] = bytes[12];
        generation_power_value.Byte[1] = bytes[11];
        id(generation_power).publish_state(generation_power_value.UInt16);
        delay(50);

        TwoByte loads_power_value;
        loads_power_value.Byte[0] = bytes[14];
        loads_power_value.Byte[1] = bytes[13];
        id(loads_power).publish_state(loads_power_value.UInt16);
        delay(50);

        TwoByte grid_voltage_r_value;
        grid_voltage_r_value.Byte[0] = bytes[16];
        grid_voltage_r_value.Byte[1] = bytes[15];
        id(grid_voltage_r).publish_state(grid_voltage_r_value.UInt16);
        delay(50);

        TwoByte grid_current_r_value;
        grid_current_r_value.Byte[0] = bytes[18];
        grid_current_r_value.Byte[1] = bytes[17];
        id(grid_current_r).publish_state(grid_current_r_value.UInt16);
        delay(50);

        TwoByte grid_frequency_r_value;
        grid_frequency_r_value.Byte[0] = bytes[20];
        grid_frequency_r_value.Byte[1] = bytes[19];
        id(grid_frequency_r).publish_state(grid_frequency_r_value.UInt16);
        delay(50);

        TwoByte grid_power_r_value;
        grid_power_r_value.Byte[0] = bytes[22];
        grid_power_r_value.Byte[1] = bytes[21];
        id(grid_power_r).publish_state(grid_power_r_value.UInt16);
        delay(50);

        TwoByte grid_voltage_s_value;
        grid_voltage_s_value.Byte[0] = bytes[24];
        grid_voltage_s_value.Byte[1] = bytes[23];
        id(grid_voltage_s).publish_state(grid_voltage_s_value.UInt16);
        delay(50);

        TwoByte grid_current_s_value;
        grid_current_s_value.Byte[0] = bytes[26];
        grid_current_s_value.Byte[1] = bytes[25];
        id(grid_current_s).publish_state(grid_current_s_value.UInt16);
        delay(50);

        TwoByte grid_frequency_s_value;
        grid_frequency_s_value.Byte[0] = bytes[28];
        grid_frequency_s_value.Byte[1] = bytes[27];
        id(grid_frequency_s).publish_state(grid_frequency_s_value.UInt16);
        delay(50);

        TwoByte grid_power_s_value;
        grid_power_s_value.Byte[0] = bytes[30];
        grid_power_s_value.Byte[1] = bytes[29];
        id(grid_power_s).publish_state(grid_power_s_value.UInt16);
        delay(50);

        TwoByte grid_voltage_T_value;
        grid_voltage_T_value.Byte[0] = bytes[32];
        grid_voltage_T_value.Byte[1] = bytes[31];
        id(grid_voltage_T).publish_state(grid_voltage_T_value.UInt16);
        delay(50);

        TwoByte grid_current_T_value;
        grid_current_T_value.Byte[0] = bytes[34];
        grid_current_T_value.Byte[1] = bytes[33];
        id(grid_current_T).publish_state(grid_current_T_value.UInt16);
        delay(50);

        TwoByte grid_frequency_T_value;
        grid_frequency_T_value.Byte[0] = bytes[36];
        grid_frequency_T_value.Byte[1] = bytes[35];
        id(grid_frequency_T).publish_state(grid_frequency_T_value.UInt16);
        delay(50);

        TwoByte grid_power_T_value;
        grid_power_T_value.Byte[0] = bytes[38];
        grid_power_T_value.Byte[1] = bytes[37];
        id(grid_power_T).publish_state(grid_power_T_value.UInt16);
        delay(50);

        TwoByte pv1_voltage_value;
        pv1_voltage_value.Byte[0] = bytes[40];
        pv1_voltage_value.Byte[1] = bytes[39];
        id(pv1_voltage).publish_state(pv1_voltage_value.UInt16);
        delay(50);

        TwoByte pv1_current_value;
        pv1_current_value.Byte[0] = bytes[42];
        pv1_current_value.Byte[1] = bytes[41];
        id(pv1_current).publish_state(pv1_current_value.UInt16);
        delay(50);

        TwoByte pv2_voltage_value;
        pv2_voltage_value.Byte[0] = bytes[46];
        pv2_voltage_value.Byte[1] = bytes[45];
        id(pv2_voltage).publish_state(pv2_voltage_value.UInt16);
        delay(50);

        TwoByte pv2_current_value;
        pv2_current_value.Byte[0] = bytes[48];
        pv2_current_value.Byte[1] = bytes[47];
        id(pv2_current).publish_state(pv2_current_value.UInt16);
        delay(50);

        TwoByte pv3_voltage_value;
        pv3_voltage_value.Byte[0] = bytes[52];
        pv3_voltage_value.Byte[1] = bytes[51];
        id(pv3_voltage).publish_state(pv3_voltage_value.UInt16);
        delay(50);

        TwoByte pv3_current_value;
        pv3_current_value.Byte[0] = bytes[54];
        pv3_current_value.Byte[1] = bytes[53];
        id(pv3_current).publish_state(pv3_current_value.UInt16);
        delay(50);

        TwoByte pv4_voltage_value;
        pv4_voltage_value.Byte[0] = bytes[58];
        pv4_voltage_value.Byte[1] = bytes[57];
        id(pv4_voltage).publish_state(pv4_voltage_value.UInt16);
        delay(50);

        TwoByte pv4_current_value;
        pv4_current_value.Byte[0] = bytes[60];
        pv4_current_value.Byte[1] = bytes[59];
        id(pv4_current).publish_state(pv4_current_value.UInt16);
        delay(50);

        TwoByte boost_temperature_value;
        boost_temperature_value.Byte[0] = bytes[64];
        boost_temperature_value.Byte[1] = bytes[63];
        id(boost_temperature).publish_state(boost_temperature_value.UInt16);
        delay(50);

        TwoByte inverter_temperature_value;
        inverter_temperature_value.Byte[0] = bytes[66];
        inverter_temperature_value.Byte[1] = bytes[65];
        id(inverter_temperature).publish_state(inverter_temperature_value.UInt16);
        delay(50);

        TwoByte ambient_temperature_value;
        ambient_temperature_value.Byte[0] = bytes[68];
        ambient_temperature_value.Byte[1] = bytes[67];
        id(ambient_temperature).publish_state(ambient_temperature_value.UInt16);
        delay(50);

        TwoByte today_yield_value;
        today_yield_value.Byte[0] = bytes[70];
        today_yield_value.Byte[1] = bytes[69];
        id(today_yield).publish_state(today_yield_value.UInt16);
        delay(50);

        uint32_t generation_total_value = int(
            (unsigned char)(bytes[71]) << 24 |
            (unsigned char)(bytes[72]) << 16 |
            (unsigned char)(bytes[73]) << 8 |
            (unsigned char)(bytes[74]));
        id(generation_total).publish_state(generation_total_value);
        delay(50);

        uint32_t FaultMessage1_value = int( 
            (unsigned char)(bytes[125]) << 24 | 
            (unsigned char)(bytes[126]) << 16 | 
            (unsigned char)(bytes[127]) << 8 | 
            (unsigned char)(bytes[128])); 
            delay(50);

        uint32_t FaultMessage2_value = int( 
            (unsigned char)(bytes[129]) << 24 | 
            (unsigned char)(bytes[130]) << 16 | 
            (unsigned char)(bytes[131]) << 8 | 
            (unsigned char)(bytes[132]));  
            delay(50);

        uint32_t FaultMessage3_value = int( 
            (unsigned char)(bytes[133]) << 24 | 
            (unsigned char)(bytes[134]) << 16 | 
            (unsigned char)(bytes[135]) << 8 | 
            (unsigned char)(bytes[136])); 
            delay(50);

        uint32_t FaultMessage4_value = int( 
            (unsigned char)(bytes[137]) << 24 | 
            (unsigned char)(bytes[138]) << 16 | 
            (unsigned char)(bytes[139]) << 8 | 
            (unsigned char)(bytes[140]));
            delay(50);

        uint32_t FaultMessage5_value = int( 
            (unsigned char)(bytes[141]) << 24 | 
            (unsigned char)(bytes[142]) << 16 | 
            (unsigned char)(bytes[143]) << 8 | 
            (unsigned char)(bytes[144])); 
            delay(50);

        uint32_t FaultMessage6_value = int( 
            (unsigned char)(bytes[145]) << 24 | 
            (unsigned char)(bytes[146]) << 16 | 
            (unsigned char)(bytes[147]) << 8 | 
            (unsigned char)(bytes[148]));
            delay(50);

        uint32_t FaultMessage7_value = int( 
            (unsigned char)(bytes[149]) << 24 | 
            (unsigned char)(bytes[150]) << 16 | 
            (unsigned char)(bytes[151]) << 8 | 
            (unsigned char)(bytes[152]));
            delay(50);

        uint32_t FaultMessage8_value = int( 
            (unsigned char)(bytes[153]) << 24 | 
            (unsigned char)(bytes[154]) << 16 | 
            (unsigned char)(bytes[155]) << 8 | 
            (unsigned char)(bytes[156]));
            delay(50);

        if(FaultMessage1_value != 0 || FaultMessage2_value != 0 || FaultMessage3_value != 0 || FaultMessage4_value != 0 || FaultMessage5_value != 0 || FaultMessage6_value != 0 || FaultMessage7_value != 0 || FaultMessage8_value != 0) {
          ESP_LOGI("custom", "Fault message 1: %i", FaultMessage1_value);
          ESP_LOGI("custom", "Fault message 2: %i", FaultMessage2_value);
          ESP_LOGI("custom", "Fault message 3: %i", FaultMessage3_value);
          ESP_LOGI("custom", "Fault message 4: %i", FaultMessage4_value);
          ESP_LOGI("custom", "Fault message 5: %i", FaultMessage5_value);
          ESP_LOGI("custom", "Fault message 6: %i", FaultMessage6_value);
          ESP_LOGI("custom", "Fault message 7: %i", FaultMessage7_value);
          ESP_LOGI("custom", "Fault message 8: %i", FaultMessage8_value);
          inverter_mode = 2; //error
        }
        else {
          inverter_mode = 1; //online
        }

        //calculate PV power from pv current and pv voltage:
        uint32_t pv1_power_value = pv1_current_value.UInt16 * pv1_voltage_value.UInt16 * 0.01;
        id(pv1_power).publish_state(pv1_power_value);
        delay(50);
        uint32_t pv2_power_value = pv2_current_value.UInt16 * pv2_voltage_value.UInt16 * 0.01;
        id(pv2_power).publish_state(pv2_power_value);
        delay(50);
        uint32_t pv3_power_value = pv3_current_value.UInt16 * pv3_voltage_value.UInt16 * 0.01;
        id(pv3_power).publish_state(pv3_power_value);
        delay(50);
        uint32_t pv4_power_value = pv4_current_value.UInt16 * pv4_voltage_value.UInt16 * 0.01;
        id(pv4_power).publish_state(pv4_power_value);
        delay(50);

        id(inverter_state).publish_state(inverter_mode);

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
