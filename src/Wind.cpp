#include "Arduino.h"

int getBeaufort(double kmh) {
    if (kmh < 1)   return 0;
    if (kmh <= 5)  return 1;
    if (kmh <= 11) return 2;
    if (kmh <= 19) return 3;
    if (kmh <= 28) return 4;
    if (kmh <= 38) return 5;
    if (kmh <= 49) return 6;
    if (kmh <= 61) return 7;
    if (kmh <= 74) return 8;
    if (kmh <= 88) return 9;
    if (kmh <= 102) return 10;
    if (kmh <= 117) return 11;
    return 12; // Orkaankracht
}

String createWindData(float speed, int deg) {
    const char* dirs[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                          "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    int idx = ((deg + 11) / 22) % 16;
	return String(dirs[idx])  + " " + String(getBeaufort(speed)) ;
}