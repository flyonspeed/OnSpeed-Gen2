# Compression and Decompression for Arduino

Shox96 is a technique for compressing short strings.  This library provides for Arduino the same API as the desktop version at [Shox96 - Guaranteed compression for Short Strings](https://github.com/siara-cc/Shox96).

To find out how Shox96 works, read [this article](https://github.com/siara-cc/Shox96/blob/master/Shox96_Article_0_2_0.pdf?raw=true).

## Usage

The following API can be used for compressing and decompressing strings or files:

```
long shox96_0_2_0_compress(const char *in, long len, char *out, struct lnk_lst *ll);
long shox96_0_2_0_decompress(const char *in, long len, char *out, struct lnk_lst *ll);
```

The last parameter can be NULL.  See example provided for demonstration of usage or see the documentation at https://github.com/siara-cc/Shox96 for further details.

For using Shox96 to store compressed data in Progmem of Arduino Uno and up, see the library at https://github.com/siara-cc/Shox96_Arduino_Progmem_lib.

## Screenshots

The example provided with this library allows you to enter a string and see how much compression can be achieved.  In general, compression utilities such as `zip`, `gzip` do not compress short strings well and often expand them.  They also use lots of memory which makes them unusable in constrained environments like Arduino.

![](ss_compress_ss.png?raw=true)

## Related projects

- [Storing compressed text in Arduino Flash memory](https://github.com/siara-cc/Shox96_Arduino_Progmem_lib)
- [Shox96 Sqlite3 User Defined Function as loadable extension](https://github.com/siara-cc/Shox96_Sqlite_UDF)
- [Sqlite3 Library for ESP32](https://github.com/siara-cc/esp32_arduino_sqlite3_lib)
- [Sqlite3 Library for ESP8266](https://github.com/siara-cc/esp_arduino_sqlite3_lib)
- [Sqlite3 Library for ESP-IDF](https://github.com/siara-cc/esp32-idf-sqlite3)
