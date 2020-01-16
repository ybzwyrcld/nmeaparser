// Copyright(c) 2020 Shenzhen Skycaster Microelectronic Co. Ltd.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-05 11:09
// Description:  No.

#ifndef NMEAPARSER_NMEA_PARSER_H_
#define NMEAPARSER_NMEA_PARSER_H_

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>


namespace libnmeaparser {

struct GPSLocation {
  char latitude_hemisphere;
  char longitude_hemisphere;
  float speed;
  float bearing;
  float accuracy;
  float age;
  int position;
  int flags;
  int satellites_used;
  double latitude;
  double longitude;
  double altitude;
  int64_t timestamp;
  char utc_time[15];  // yymmddhhmmss{ms}{ms}{\0}
};

using LocationCallback = std::function<void(const GPSLocation &)>;
using NmeaCallback = std::function<void(const char *)>;

class NmeaParser {
 public:
  NmeaParser();
  ~NmeaParser() {}
  void Init(void);
  void Run(void);
  void OnNmeaCallback(const NmeaCallback& callback) {
    nmea_callback_ = callback;
  }
  void OnLocationCallback(const LocationCallback &callback) {
    location_callback_ = callback;
  }
  bool PutNmeaLine(const std::string &in);

 private:
  void NmeaParse(void);
  GPSLocation gps_location_;
  std::vector<std::string> tokenizer_;
  LocationCallback location_callback_;
  NmeaCallback nmea_callback_;
};

}  // namespace libnmeaparser

#endif  // NMEAPARSER_NMEA_PARSER_H_
