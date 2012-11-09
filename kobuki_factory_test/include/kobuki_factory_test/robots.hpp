/**
 * @file /include/kobuki_factory_test/robots.hpp
 *
 * @brief Evaluated robots list
 *
 * @date October 2012
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef KOBUKI_FACTORY_TEST_ROBOTS_HPP_
#define KOBUKI_FACTORY_TEST_ROBOTS_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

#include <iostream>
#include <fstream>

#include <ros/ros.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace kobuki_factory_test {

/*****************************************************************************
** Types
*****************************************************************************/

typedef long long int int64;

/*****************************************************************************
** Helper functions
*****************************************************************************/

template<class T> std::string to_string(T i) {
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();

    return ss.str();
}

/*****************************************************************************
** Class
*****************************************************************************/

class Robot {

public:
  enum State {
    UNKNOWN = -1,
    OK      =  0,
    WARN    =  1,
    ERROR   =  2
  };

  enum Device {
    V_INFO,
    IR_DOCK_L,
    IR_DOCK_C,
    IR_DOCK_R,
    IMU_DEV,
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUMPER_L,
    BUMPER_C,
    BUMPER_R,
    W_DROP_L,
    W_DROP_R,
    CLIFF_L,
    CLIFF_C,
    CLIFF_R,
    PWR_JACK,
    PWR_DOCK,
    CHARGING,
    EXT_PWR,
    INPUT,
    OUTPUT,
    LED_1,
    LED_2,
    SOUNDS,
    MOTOR_L,
    MOTOR_R,
    DEVICES_COUNT
  };

  Robot(unsigned int seq_nb) :
    seq_nb(seq_nb),
    serial(to_string(seq_nb)), // temporally, until we have a real serial number
    state(UNKNOWN),
    device_ok(DEVICES_COUNT),
    device_val(DEVICES_COUNT),
    imu_data(5) // test 1, diff 1, test 2, diff 2, current value
    {

    for (unsigned int i = 0; i < DEVICES_COUNT; i++) {
      device_ok[i] = false;
      device_val[i] = 0;
    }
  };

  ~Robot() { };

  bool all_ok() {
                                              device_ok[EXT_PWR] = true;
                                                        device_ok[INPUT] = true;
                                                                  device_ok[OUTPUT] = true;
    for (unsigned int i = 0; i < DEVICES_COUNT; i++) {
      if (device_ok[i] == false)
        return false;
    }

    return true;
  };

  bool motors_ok()  { return std::max(device_val[MOTOR_L], device_val[MOTOR_R]) <= 10; }
  bool ir_dock_ok() { return device_ok[IR_DOCK_L] && device_ok[IR_DOCK_C] && device_ok[IR_DOCK_R]; };
  bool buttons_ok() { return device_ok[BUTTON_0]  && device_ok[BUTTON_1]  && device_ok[BUTTON_2]; };
  bool bumpers_ok() { return device_ok[BUMPER_L]  && device_ok[BUMPER_C]  && device_ok[BUMPER_R]; };
  bool w_drop_ok()  { return device_ok[W_DROP_L]  && device_ok[W_DROP_R]; };
  bool cliffs_ok()  { return device_ok[CLIFF_L]   && device_ok[CLIFF_C]   && device_ok[CLIFF_R]; };
  bool pwr_src_ok() { return device_ok[PWR_JACK]  && device_ok[PWR_DOCK]; };

  std::string version_nb(char separator = '/') {
    // Version number is stored as 0xhhhhffffssssssss
    //  - first 16 bits for hardware
    //  - next 16 bits for firmware
    //  - remaining 32 bits for software
    char str[64];
    snprintf(str, 64, "%lld%c%lld%c%lld", (device_val[V_INFO] >> 48),          separator,
                                          (device_val[V_INFO] >> 32) & 0xFFFF, separator,
                                          (device_val[V_INFO] & 0xFFFFFFFF));
    return std::string(str);
  };

  bool saveToCSVFile(const std::string& path) {
    std::ofstream os;
    os.open(path.c_str(), std::ios::out | std::ios::app);
    if (os.good() == false) {
      ROS_ERROR("Unable to open %s for writing", path.c_str());
      return false;
    }

    if (os.tellp() == 0) {
      // Empty file; write header
//      os << ",SN,DCJ,MOP,DST,VER,FW,HW,DOCK,BRU,S-Brush,,VAC,PSD,,,FIR,,,DIR,,,Forward,,Backward,,PE,,HALL,DOCK,CHR,G-Test,RESULT";
      os << "SN,VER,FW,HW,JACK,DOCK,CHR,PSD-L,PSD-C,PSD-R,BMP-L,BMP-C,BMP-R,IRD-L,IRD-C,IRD-R,WD-L,WD-R,MOT-L,MOT-R,IMU-DIFF,IMU,BUT-1,BUT-2,BUT-3,LED-1,LED-2,SND,RESULT\n";
    }

    os << serial << "," << version_nb(',') << ","
       << device_val[PWR_DOCK]  << "," << device_val[PWR_JACK]  << "," << device_val[CHARGING]  << ","
       << device_val[CLIFF_L]   << "," << device_val[CLIFF_C]   << "," << device_val[CLIFF_R]   << ","
       << device_val[BUMPER_L]  << "," << device_val[BUMPER_C]  << "," << device_val[BUMPER_R]  << ","
       << device_val[IR_DOCK_L] << "," << device_val[IR_DOCK_C] << "," << device_val[IR_DOCK_R] << ","
       << device_val[W_DROP_L]  << "," << device_val[W_DROP_R]  << ","
       << device_val[MOTOR_L]   << "," << device_val[MOTOR_R]   << ","
       << imu_data[1] - imu_data[3]                             << "," << device_ok[IMU_DEV]  << ","
       << device_ok[BUTTON_0]   << "," << device_ok[BUTTON_1]   << "," << device_ok[BUTTON_2] << ","
       << device_ok[LED_1]      << "," << device_ok[LED_2]      << "," << device_ok[SOUNDS]   << ","
       << all_ok() << std::endl;

    if (!all_ok())
      os << serial << "," << device_ok[V_INFO] << "," << device_ok[V_INFO] << "," << device_ok[V_INFO] << ","
         << device_ok[PWR_DOCK]  << "," << device_ok[PWR_JACK]  << "," << device_ok[CHARGING]  << ","
         << device_ok[CLIFF_L]   << "," << device_ok[CLIFF_C]   << "," << device_ok[CLIFF_R]   << ","
         << device_ok[BUMPER_L]  << "," << device_ok[BUMPER_C]  << "," << device_ok[BUMPER_R]  << ","
         << device_ok[IR_DOCK_L] << "," << device_ok[IR_DOCK_C] << "," << device_ok[IR_DOCK_R] << ","
         << device_ok[W_DROP_L]  << "," << device_ok[W_DROP_R]  << ","
         << device_ok[MOTOR_L]   << "," << device_ok[MOTOR_R]   << ","
         << imu_data[1] - imu_data[3]                           << "," << device_ok[IMU_DEV]  << ","
         << device_ok[BUTTON_0]   << "," << device_ok[BUTTON_1] << "," << device_ok[BUTTON_2] << ","
         << device_ok[LED_1]      << "," << device_ok[LED_2]    << "," << device_ok[SOUNDS]   << ","
         << all_ok() << std::endl;

    os.close();
    return true;


//    EXT_PWR,
//    INPUT,
//    OUTPUT,
  }

  unsigned int seq_nb;
  std::string  serial;
  State         state;

  std::string      diagnostics;
  std::vector<bool>  device_ok;
  std::vector<int64> device_val;

  // Some special (non integer) data
  std::vector<double> imu_data;


//  bool v_info_ok;
//  bool beacon_ok;
//  bool gyro_ok;
//  bool button_ok;
//  bool bumper_ok;
//  bool w_drop_ok;
//  bool cliff_ok;
//  bool power_ok;
//  bool input_ok;
//  bool output_ok;
//  bool ext_pwr_ok;
//  bool leds_ok;
//  bool sound_ok;
//  bool motor_ok;

//  std::vector<int> v_info;
//  std::vector<int> beacon;
//  std::vector<int> button;
//  std::vector<int> bumper;
//  std::vector<int> w_drop;
//  std::vector<int> cliff;
//  std::vector<int> power;
//  std::vector<int> input;
//  std::vector<int> motor;
//  std::vector<int> leds;

};

class RobotList : public std::list<Robot> {

};

// Define a postfix increment operator for Robot::Device
inline Robot::Device operator++(Robot::Device& rd, int)
{
  return rd = (Robot::Device)(rd + 1);
}

}  // namespace kobuki_factory_test

#endif /* KOBUKI_FACTORY_TEST_ROBOTS_HPP_ */
