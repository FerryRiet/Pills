#ifndef UpdateDisplay_H
#define UpdateDisplay_H

#include <Arduino.h>

#include "Weather.h"
#include "NTPtimeESP.h"

extern void UpdateWeatherDisplay(strWeatherInfo &weather, strDateTime &dTime);
extern void UpdatePillsDisplay(strDateTime &dTime);
extern void UpdateVersionDisplay() ;
extern void UpdateErrorDisplay(strDateTime &dTime) ;

#endif