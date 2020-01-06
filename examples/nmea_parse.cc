// Copyright(c) 2020 Shenzhen Skycaster Microelectronic Co. Ltd.
// All rights reserved.
//
// Author:  Yuming Meng
// Date:  2020-01-05 11:09
// Description:  No.

#include "nmea_parser.h"


using libnmeaparser::NmeaParser;
using libnmeaparser::GPSLocation;

namespace {
constexpr char gpgga[] =
    "$GPGGA,084935.00,2232.6706239,N,11406.2752992,E,2,06,5.8,12.3588,M,"
    "-2.855,M,02,3572*78";

};  // namespace

int main(void) {
  NmeaParser parser;
  parser.Init();
  parser.OnNmeaCallback([](const char *line) { printf("NMEA: %s", line); });
  parser.OnLocationCallback([](const GPSLocation &location) {
      printf("timestamp: %ld, status: %d, age: %f, accuracy: %f, "
             "log: %lf%c, lat: %lf%c, alt: %f, su: %d\n",
             location.timestamp, location.position,
             location.age, location.accuracy,
             location.longitude, location.longitude_hemisphere,
             location.latitude, location.latitude_hemisphere,
             location.altitude, location.satellites_used);
  });
  parser.PutNmeaLine(gpgga);
}
