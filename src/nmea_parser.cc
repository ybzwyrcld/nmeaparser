// Copyright(c) 2020 Shenzhen Skycaster Microelectronic Co. Ltd.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-05 11:09
// Description:  No.

#include "nmea_parser.h"

#include <math.h>
#include <string.h>


namespace libnmeaparser {

namespace {

constexpr int kSecondsForOneDay = 24 * 3600;

bool NmeaLineSplit(const std::string &str, std::vector<std::string> *v) {
  if (v == nullptr) return false;
  v->clear();
  std::string c = "*";
  std::string::size_type pos1 = 0;
  std::string::size_type pos2 = str.find(c);
  std::string substr = str.substr(pos1, pos2 - pos1);
  std::string checksum = str.substr(++pos2, str.size() - pos2);
  c = ",";
  pos2 = substr.find(c);
  while (std::string::npos != pos2) {
    v->push_back(substr.substr(pos1, pos2 - pos1));

    pos1 = pos2 + c.size();
    pos2 = substr.find(c, pos1);
  }
  if (pos1 != substr.length()) v->push_back(substr.substr(pos1));
  v->push_back(checksum);
}

double ConvertFromHhmm(const double &in) {
  int degrees = static_cast<int>(floor(in));
  double minutes = (in - degrees) * 100.0;
  double dcoord = degrees + minutes / 60.0;
  return dcoord;
}

}  // namespace

NmeaParser::NmeaParser() {
  nmea_callback_ = [](const char *){};
  location_callback_ = [](const GPSLocation &){};
}

void NmeaParser::Init(void) {
  memset(&gps_location_, 0x0, sizeof(gps_location_));
  gps_location_.timestamp = 1578278163U;
  memset(gps_location_.utc_time, 0x30, sizeof(gps_location_.utc_time) - 1);
  gps_location_.utc_time[sizeof(gps_location_.utc_time) - 1] = 0;
}

void NmeaParser::Run(void) {}

bool NmeaParser::PutNmeaLine(const std::string& in) {
  NmeaLineSplit(in, &tokenizer_);
#if 0
  for (auto str : tokenizer_) {
    printf("%s\n", str.c_str());
  }
#endif
  NmeaParse();
  nmea_callback_(in.c_str());
  return true;
}

void NmeaParser::NmeaParse(void) {
  int pos = 0;
  if (tokenizer_.empty()) return;
  if (tokenizer_[0].find("GGA") != std::string::npos) {
    pos = 6;
    for (auto ch : tokenizer_[1]) {
      if (ch != '.') gps_location_.utc_time[pos++] = ch;
    }
    gps_location_.latitude = ConvertFromHhmm(std::stod(tokenizer_[2]) / 100.0);
    gps_location_.latitude_hemisphere = tokenizer_[3][0];
    gps_location_.longitude = ConvertFromHhmm(std::stod(tokenizer_[4]) / 100.0);
    gps_location_.longitude_hemisphere = tokenizer_[5][0];
    gps_location_.position = std::stoi(tokenizer_[6]);
    gps_location_.satellites_used = std::stoi(tokenizer_[7]);
    if (!tokenizer_[8].empty())
      gps_location_.accuracy = std::stof(tokenizer_[8]);
    if (!tokenizer_[9].empty())
      gps_location_.altitude = std::stof(tokenizer_[9]);
    if (!tokenizer_[13].empty())
      gps_location_.age = std::stof(tokenizer_[13]);
    location_callback_(gps_location_);
  } else if (tokenizer_[0].find("RMC") != std::string::npos) {
    pos = 6;
    for (auto ch : tokenizer_[1]) {
      if (ch != '.') gps_location_.utc_time[pos++] = ch;
    }
    gps_location_.latitude = ConvertFromHhmm(std::stod(tokenizer_[3]) / 100.0);
    gps_location_.latitude_hemisphere = tokenizer_[4][0];
    gps_location_.longitude = ConvertFromHhmm(std::stod(tokenizer_[5]) / 100.0);
    gps_location_.longitude_hemisphere = tokenizer_[6][0];
    gps_location_.speed = std::stod(tokenizer_[7]);
    gps_location_.bearing = std::stod(tokenizer_[8]);
    pos = 0;
    std::string date = tokenizer_[9].substr(4, 2) +
                       tokenizer_[9].substr(2, 2) +
                       tokenizer_[9].substr(0, 2);
    for (auto ch : date) gps_location_.utc_time[pos++] = ch;
    location_callback_(gps_location_);
  }
}

}  // namespace libnmeaparser
