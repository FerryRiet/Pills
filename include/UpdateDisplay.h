#ifndef UpdateDisplay_H
#define UpdateDisplay_H

#include <Arduino.h>

extern void UpdateWeatherDisplay(String &dateStr, String &timeStr);
extern void UpdatedatePillsDisplay(String &dateStr, String &timeStr);
extern void UpdateVersionDisplay() ;

#endif