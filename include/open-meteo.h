#ifndef OPEN_METEO_H
#define OPEN_METEO_H

#include <Arduino.h>
#include "Weather.h"

// Open-Meteo header file
// Empty header - add declarations as needed

// OpenWeatherMap API Info

const String WeatherURL = "https://api.open-meteo.com/v1/forecast?latitude=52.35&longitude=4.80&&current=wind_direction_10m,temperature_2m,precipitation,weather_code,wind_speed_10m,rain&models=knmi_seamless";


strWeatherInfo getWeatherInfo() ;
void printWeatherInfo(const strWeatherInfo &str) ;


#endif // OPEN_METEO_H