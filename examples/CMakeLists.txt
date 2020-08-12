cmake_minimum_required (VERSION 2.8)

add_executable (nmea_parse
  nmea_parse.cc
)
target_link_libraries (nmea_parse
  nmeaparser
)