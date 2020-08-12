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

// @File    :  parser.h
// @Version :  1.0
// @Time    :  2020-01-05 11:09
// @Author  :  Meng Yuming
// @Contact :  mengyuming@hotmail.com
// @Desc    :  None

#ifndef NMEAPARSER_PARSER_H_
#define NMEAPARSER_PARSER_H_

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>


namespace libnmeaparser {

enum GPSLocationFixFlags {
  kGPSHasLatitudeLongitude = 0x1,  // 经纬度更新.
  kGPSHasAltitude = 0x2,  // 地面高程更新.
  kGPSHasSpeed = 0x4,  // 航海速度更新.
  kGPSHasBearing = 0x8,  // 航向角更新.
  kGPSHasAccuracy = 0x10,  // 水平精度因子更新.
  kGPSHasPositionFix = 0x20,  // 定位解状态更新.
  kGPSHasAgeOfDGPS = 0x40,  // 差分龄期更新.
};

struct GPSLocation {
  char latitude_hemisphere;  // 纬度半球.
  char longitude_hemisphere;  // 经度半球.
  char position_status;  // 定位状态.
  char position_mode;  // 定位模式.
  float speed_in_knots;  // 以节(knots)为单位的速度.
  float speed_in_kilometers_per_hour;  // 以千米每小时(km/h)为单位的速度.
  float bearing;  // 航向角.
  float accuracy;  // 水平精度因子.
  float age;  // 差分龄期.
  int position_fix_status;  // 定位解状态.
  int flags;  // 更新标志.
  int satellites_used;  // 正在使用定位的卫星数量.
  double latitude;  // 纬度.
  double longitude;  // 经度.
  double altitude;  // 海拔高度.
  int64_t timestamp;  // UNIX时间戳(UTC时间).
};

using LocationCallback = std::function<void (GPSLocation const&)>;
using NmeaCallback = std::function<void (char const* const)>;

class NmeaParser {
 public:
  NmeaParser() {}
  ~NmeaParser() {}
  void Init(void);
  void Run(void) { }  // 暂时不用.
  void OnNmeaCallback(const NmeaCallback& callback) {
    nmea_callback_ = callback;
  }
  void OnLocationCallback(const LocationCallback &callback) {
    location_callback_ = callback;
  }
  bool PutNmeaLine(std::string const& in);
  int utc_year(void) const { return utc_year_; }
  int utc_month(void) const { return utc_month_; }
  int utc_day(void) const { return utc_day_; }
  int utc_time_difference(void) const { return utc_time_difference_; }
  //
  //  以下函数无须外部调用.
  //
  void UpdateUTCTimeDifference(void);
  void UpdateTime(std::string const& time);
  void UpdateYear(std::string const& year);
  void UpdateMonth(std::string const& month);
  void UpdateDay(std::string const& day);
  void UpdateDate(std::string const& date, std::string const& time);
  void UpdateLatitudeLongitude(
      std::string const& latitude, std::string const& latitude_hemisphere,
      std::string const& longitude, std::string const& longitude_hemisphere);
  void UpdateQualityIndicator(std::string const& quality);
  void UpdateModeIndicator(std::string const& mode);
  void UpdateSatellitesUsed(std::string const& satellites_used);
  void UpdateAccuracy(std::string const& accuracy);
  void UpdateAltitude(std::string const& altitude);
  void UpdateAgeOfDGPS(std::string const& age);
  void UpdatePositingStatus(std::string const& status);
  void UpdatePositingMode(std::string const& mode);
  void UpdateSpeedInKnots(std::string const& speed);
  void UpdateSpeedInKilometersPerHour(std::string const& speed);
  void UpdateBearing(std::string const& bearing);

 private:
  void NmeaParse(void);

  int utc_year_;
  int utc_month_;
  int utc_day_;
  int utc_time_difference_;
  GPSLocation gps_location_ {};
  std::vector<std::string> tokenizer_;
  LocationCallback location_callback_;
  NmeaCallback nmea_callback_;
};

}  // namespace libnmeaparser

#endif  // NMEAPARSER_PARSER_H_
