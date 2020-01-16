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
    "$GPGGA,013353.00,2234.2187840,N,11356.2551977,E,"
    "4,18,1.6,52.6895,M,-3.509,M,02,3397*76";

constexpr char gprmc[] =
    "$GPRMC,013353.00,A,2234.2187840,N,11356.2551977,E,"
    "000.013,308.5,020719,0.0,W,D*29";

}  // namespace

int main(void) {
  NmeaParser parser;
  parser.Init();
  parser.OnNmeaCallback([](const char *line) {
    printf("NMEA: %s\r\n", line);
  });
  parser.OnLocationCallback([](const GPSLocation &location) {
      printf("time: %s, status: %d, age: %f, accuracy: %f, "
             "log: %lf%c, lat: %lf%c, alt: %f, su: %d, "
             "speed: %f, bearing: %f\r\n",
             location.utc_time, location.position,
             location.age, location.accuracy,
             location.longitude, location.longitude_hemisphere,
             location.latitude, location.latitude_hemisphere,
             location.altitude, location.satellites_used,
             location.speed, location.bearing);
  });
  parser.PutNmeaLine(gpgga);
  parser.PutNmeaLine(gprmc);
}
