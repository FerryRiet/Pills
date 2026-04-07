#ifndef UpdateDisplay_H
#define UpdateDisplay_H

#include <Arduino.h>

#include "Weather.h"
#include "NTPtimeESP.h"
#include "Adafruit_BME680.h"

extern void UpdateWeatherDisplay(strWeatherInfo &weather, strDateTime &dTime);
extern void UpdatePillsDisplay(strDateTime &dTime);
extern void UpdateVersionDisplay() ;
extern void UpdateErrorDisplay(strDateTime &dTime) ;
extern void UpdateSensorDisplay(Adafruit_BME680 &bme) ;

#endif