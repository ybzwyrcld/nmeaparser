// MIT License
//
// Copyright (c) 2020 Yuming Meng
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @File    :  parser.cc
// @Version :  1.0
// @Time    :  2020-01-05 11:09
// @Author  :  Meng Yuming
// @Contact :  mengyuming@hotmail.com
// @Desc    :  None

#include "parser.h"

#include <math.h>
#include <string.h>
#ifndef _POSIX_THREAD_SAFE_FUNCTIONS
#define _POSIX_THREAD_SAFE_FUNCTIONS
#endif  // _POSIX_THREAD_SAFE_FUNCTIONS
#include <time.h>

#include <chrono>


namespace libnmeaparser {

namespace {

constexpr int kSecondsForOneDay = 24 * 3600;

bool NmeaLineSplit(const std::string &str, std::vector<std::string> *v) {
  if (v == nullptr) return false;
  v->clear();
  // 提取校验符.
  std::string c = "*";
  std::string::size_type pos1 = 0;
  std::string::size_type pos2 = str.find(c);
  std::string substr = str.substr(pos1, pos2 - pos1);
  std::string checksum = str.substr(pos2+1, 2);
  // 以','为切割符提取每个字段, 首个字段为"$XXX".
  c = ",";
  pos2 = substr.find(c);
  while (std::string::npos != pos2) {
    v->push_back(substr.substr(pos1, pos2 - pos1));
    pos1 = pos2 + c.size();
    pos2 = substr.find(c, pos1);
  }
  if (pos1 != substr.length()) v->push_back(substr.substr(pos1));
  v->push_back(checksum);
  return true;
}

double ConvertFromHhmm(const double &in) {
  int degrees = static_cast<int>(floor(in));
  double minutes = (in - degrees) * 100.0;
  double dcoord = degrees + minutes / 60.0;
  return dcoord;
}

}  // namespace

void NmeaParser::Init(void) {
  utc_year_ = -1;
  utc_month_ = -1;
  utc_day_ = -1;
  utc_time_difference_ = 0;
  nmea_callback_ = [] (char const* const) -> void { return; };
  location_callback_ = [] (GPSLocation const&) -> void { return; };
  UpdateUTCTimeDifference();
}

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

// 获取UTC时间与本地时间的差值.
void NmeaParser::UpdateUTCTimeDifference(void) {
  auto tt = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now());
	struct tm utc_now;
	struct tm local_now;
	gmtime_r(&tt, &utc_now);
	localtime_r(&tt, &local_now);
	utc_time_difference_ = static_cast<int>(mktime(&local_now)-mktime(&utc_now));
}

// 部分定位模块可能未开启'ZDA'语句输出, 这样就无法得到完整的年份,
// 因此需要获取系统的UTC日期.
void NmeaParser::UpdateTime(std::string const& time) {
  if (time.empty()) return;
  struct tm time_now {};
  if (utc_year_ < 2000) {
    auto tt = std::chrono::system_clock::to_time_t(
			  std::chrono::system_clock::now());
    gmtime_r(&tt, &time_now);
    utc_year_ = (time_now.tm_year+1900)/100*100+utc_year_%100;
    utc_month_ = time_now.tm_mon+1;
    utc_day_ = time_now.tm_mday;
  } else {
    time_now.tm_year = utc_year_-1900;
    time_now.tm_mon = utc_month_-1;
    time_now.tm_mday = utc_day_;
    time_now.tm_isdst = -1;
  }
  int hour = std::stoi(time.substr(0, 2));
  int minute = std::stoi(time.substr(2, 2));
  int seconds = std::stoi(time.substr(4, 2));
  time_now.tm_hour = hour;
  time_now.tm_min = minute;
  time_now.tm_sec = seconds;
  gps_location_.timestamp = mktime(&time_now) + utc_time_difference_;
}

void NmeaParser::UpdateYear(std::string const& year) {
  if (!year.empty()) {
    int i_year = std::stoi(year);
    if (i_year > 0) {
      if (i_year < 100) {
        i_year += utc_year_/100*100;
      }
      utc_year_ = i_year;
    }
  }
}

void NmeaParser::UpdateMonth(std::string const& month) {
  if (!month.empty()) {
    int i_month = std::stoi(month);
    if (i_month > 0) utc_month_ = i_month;
  }
}

void NmeaParser::UpdateDay(std::string const& day) {
  if (!day.empty()) {
    int i_day = std::stoi(day);
    if (i_day > 0) utc_day_ = i_day;
  }
}

void NmeaParser::UpdateDate(std::string const& date,
                            std::string const& time) {
  if (!date.empty()) {
    UpdateYear(date.substr(4, 2));
    UpdateMonth(date.substr(2, 2));
    UpdateDay(date.substr(0, 2));
  }
  UpdateTime(time);
}

void NmeaParser::UpdateLatitudeLongitude(
    std::string const& latitude, std::string const& latitude_hemisphere,
    std::string const& longitude, std::string const& longitude_hemisphere) {
  if (!latitude.empty()) {
    gps_location_.latitude = ConvertFromHhmm(std::stod(latitude)/100.0);
  }
  if (!latitude_hemisphere.empty()) {
    gps_location_.latitude_hemisphere = latitude_hemisphere[0];
  }
  if (!longitude.empty()) {
    gps_location_.longitude = ConvertFromHhmm(std::stod(longitude)/100.0);
  }
  if (!longitude_hemisphere.empty()) {
    gps_location_.longitude_hemisphere = longitude_hemisphere[0];
  }
  gps_location_.flags |= kGPSHasLatitudeLongitude;
}

void NmeaParser::UpdateQualityIndicator(std::string const& quality) {
  if (!quality.empty()) {
    gps_location_.position_fix_status = std::stoi(quality);
    gps_location_.flags |= kGPSHasPositionFix;
  }
}

void NmeaParser::UpdateSatellitesUsed(std::string const& satellites_used) {
  if (!satellites_used.empty()) {
    gps_location_.satellites_used = std::stoi(satellites_used);
  }
}

void NmeaParser::UpdateAccuracy(std::string const& accuracy) {
  if (!accuracy.empty()) {
    gps_location_.accuracy = std::stof(accuracy);
    gps_location_.flags |= kGPSHasAccuracy;
  }
}

void NmeaParser::UpdateAltitude(std::string const& altitude) {
  if (!altitude.empty()) {
    gps_location_.altitude = std::stof(altitude);
    gps_location_.flags |= kGPSHasAltitude;
  }
}

void NmeaParser::UpdateAgeOfDGPS(std::string const& age) {
  if (!age.empty()) {
    gps_location_.age = std::stof(age);
    gps_location_.flags |= kGPSHasAgeOfDGPS;
  }
}

void NmeaParser::UpdatePositingStatus(std::string const& status) {
  if (!status.empty()) {
    gps_location_.position_status = status[0];
  }
}

void NmeaParser::UpdatePositingMode(std::string const& mode) {
  if (!mode.empty()) {
    gps_location_.position_mode = mode[0];
  }
}

void NmeaParser::UpdateSpeedInKnots(std::string const& speed) {
  if (!speed.empty()) {
    gps_location_.speed_in_knots = std::stof(speed);
    gps_location_.flags |= kGPSHasSpeed;
  }
}

void NmeaParser::UpdateSpeedInKilometersPerHour(std::string const& speed) {
  if (!speed.empty()) {
    gps_location_.speed_in_kilometers_per_hour = std::stof(speed);
    gps_location_.flags |= kGPSHasSpeed;
  }
}

void NmeaParser::UpdateBearing(std::string const& bearing) {
  if (!bearing.empty()) {
    gps_location_.bearing = std::stof(bearing);
    gps_location_.flags |= kGPSHasBearing;
  }
}

void NmeaParser::NmeaParse(void) {
  if (tokenizer_.empty()) return;
  if (tokenizer_[0].find("GGA") != std::string::npos) {
    UpdateLatitudeLongitude(tokenizer_[2], tokenizer_[3],
                            tokenizer_[4], tokenizer_[5]);
    UpdateQualityIndicator(tokenizer_[6]);
    UpdateSatellitesUsed(tokenizer_[7]);
    UpdateAccuracy(tokenizer_[8]);
    UpdateAltitude(tokenizer_[9]);
    UpdateAgeOfDGPS(tokenizer_[13]);
    UpdateTime(tokenizer_[1]);
  } else if (tokenizer_[0].find("GLL") != std::string::npos) {
    UpdateLatitudeLongitude(tokenizer_[1], tokenizer_[2],
                            tokenizer_[3], tokenizer_[4]);
    UpdateTime(tokenizer_[5]);
    UpdatePositingStatus(tokenizer_[6]);
    UpdatePositingMode(tokenizer_[7]);
  } else if (tokenizer_[0].find("RMC") != std::string::npos) {
    UpdatePositingStatus(tokenizer_[2]);
    UpdateLatitudeLongitude(tokenizer_[3], tokenizer_[4],
                            tokenizer_[5], tokenizer_[6]);
    UpdateSpeedInKnots(tokenizer_[7]);
    UpdateBearing(tokenizer_[8]);
    UpdateDate(tokenizer_[9], tokenizer_[1]);
  } else if (tokenizer_[0].find("VTG") != std::string::npos) {
    UpdateBearing(tokenizer_[1]);
    UpdateSpeedInKnots(tokenizer_[5]);
    UpdateSpeedInKilometersPerHour(tokenizer_[7]);
    UpdatePositingMode(tokenizer_[9]);
  } else if (tokenizer_[0].find("ZDA") != std::string::npos) {
    UpdateDay(tokenizer_[2]);
    UpdateMonth(tokenizer_[3]);
    UpdateYear(tokenizer_[4]);
    UpdateTime(tokenizer_[1]);
  } else {
    return;
  }
  location_callback_(gps_location_);
}

}  // namespace libnmeaparser
