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

// @File    :  nmea_parse.cc
// @Version :  1.0
// @Time    :  2020-01-05 11:09
// @Author  :  Meng Yuming
// @Contact :  mengyuming@hotmail.com
// @Desc    :  None

#include "nmeaparser/parser.h"


using libnmeaparser::NmeaParser;
using libnmeaparser::GPSLocation;

namespace {

constexpr char gpgga[] =
    "$GPGGA,061758.000,2234.2266175,N,11356.2637186,E,"
    "1,12,1.1,52.952,M,-1.900,M,,0000*75";
constexpr char gngsa1[] = "$GNGSA,A,3,02,05,06,13,29,,,,,,,,1.9,1.1,1.6,1*36";
constexpr char gngsa2[] = "$GNGSA,A,3,,,,,,,,,,,,,1.9,1.1,1.6,2*3D";
constexpr char gngsa3[] = "$GNGSA,A,3,11,19,,,,,,,,,,,1.9,1.1,1.6,3*34";
constexpr char gngsa4[] = "$GNGSA,A,3,08,13,19,22,35,,,,,,,,1.9,1.1,1.6,4*3F";
constexpr char gngll[] =
    "$GNGLL,2234.2266175,N,11356.2637186,E,061758.000,A,A*41";
constexpr char gnrmc[] =
    "$GNRMC,061758.000,A,2234.2266175,N,11356.2637186,E,000.0,"
    "000.0,110820,,,A,V*06";
constexpr char gnvtg[] = "$GNVTG,000.0,T,,M,000.0,N,000.0,K,A*13";
constexpr char gnzda[] = "$GNZDA,061758.000,11,08,2020,00,00*4D";
constexpr char psti1[] = "$PSTI,030,061758.000,A,2234.2266175,N,11356.2637186,E,"
    "52.952,-0.02,-0.04,0.01,110820,A,0.000,0.000*26";
constexpr char psti2[] = "$PSTI,032,061758.000,110820,V,,,,,,,,,,,*4C";

}  // namespace


int main(void) {
  NmeaParser parser;
  parser.Init();
  parser.OnNmeaCallback([] (const char* const line) {
    printf("NMEA: %s\r\n", line);
  });
  parser.OnLocationCallback([] (GPSLocation const& location) {
      printf("timestamp: %lld, status: %d, age: %f, accuracy: %f, "
             "log: %lf%c, lat: %lf%c, alt: %f, su: %d, "
             "speed: %f, bearing: %f\r\n",
             location.timestamp, location.position_fix_status,
             location.age, location.accuracy,
             location.longitude, location.longitude_hemisphere,
             location.latitude, location.latitude_hemisphere,
             location.altitude, location.satellites_used,
             location.speed_in_kilometers_per_hour, location.bearing);
  });
  parser.PutNmeaLine(gpgga);
  parser.PutNmeaLine(gngll);
  parser.PutNmeaLine(gnrmc);
  parser.PutNmeaLine(gnvtg);
  parser.PutNmeaLine(gnzda);
}
