#ifndef OPEN_METEO_H
#define OPEN_METEO_H

#include <Arduino.h>

// Open-Meteo header file
// Empty header - add declarations as needed

// OpenWeatherMap API Info

const String weatherURL = "https://api.open-meteo.com/v1/forecast?latitude=52.35&longitude=4.80&&current=wind_direction_10m,temperature_2m,precipitation,weather_code,wind_speed_10m,rain&models=knmi_seamless";

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

strWeatherInfo getWeatherInfo() ;
void printWeatherInfo(const strWeatherInfo &str) ;


#endif // OPEN_METEO_H