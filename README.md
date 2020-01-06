# NMEA Parser

GNSS NMEA-0183 protocal universal parser.


## Quick Start

Clone and run the build:

```bash
$ git clone https://github.com/hanoi404/nmeaparser && cd nmeaparser
$ make all
```

Run example program:

```bash
./nmea_parse
```

## CMake

Config and build:

```bash
$ mkdir build && cd build
$ cmake .. -DNMEAPARSER_BUILD_EXAMPLES=ON
$ make
```

Output programs in `examples` directory:

```bash
examples/nmea_parse
```