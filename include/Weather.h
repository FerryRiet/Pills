#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// Bitmap definitions for weather icons
extern const unsigned char epd_bitmap_broken_clouds[];
extern const unsigned char epd_bitmap_clear_sky[];
extern const unsigned char epd_bitmap_few_clouds[];
extern const unsigned char epd_bitmap_mist[];
extern const unsigned char epd_bitmap_rain[];
extern const unsigned char epd_bitmap_scattered_clouds[];
extern const unsigned char epd_bitmap_shower_rain[];
extern const unsigned char epd_bitmap_snow[];
extern const unsigned char epd_bitmap_thunderstorm[];

extern const unsigned char epd_bitmap_temperature[];
extern const unsigned char wind_bitmap[];
extern const unsigned char epd_bitmap_warning[];
extern const unsigned char epd_bitmap_humidity[];
extern const unsigned char wind_new[];
extern const unsigned char stop_bits[]; 

// ESP32 display object
//extern GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display;
extern GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display;
//extern GxEPD2_3C<GxEPD2_420c_Z21, 300> display;

// Weather data globals

struct strWeatherInfo
{
  float temperature ;
  float surface_pressure ;
  int   weather_code ;
  int   relative_humidity_2m;
  float precipitation ;
  float wind_speed_10m;
  int   wind_direction_10m;
  boolean valid;
};

// Icon pointer
extern const unsigned char *icon;

// Function to select weather icon based on weather code
extern const unsigned char *selectWeatherIcon(int weather_code);
extern String createWindData(float speed, int direction) ;

#endif // WEATHER_H
