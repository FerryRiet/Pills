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


String createWindData(float speed, int direction) {
	String windDirection;
	if ( direction > ( 360 - 22 ) || direction < 22  ) windDirection = "N" ;
	else if ( direction >= 22 && direction < 67  ) windDirection = "NO" ;
		else if ( direction >= 67 && direction < 112  ) windDirection = "O" ;
			else if ( direction >= 112 && direction < 156  ) windDirection = "ZO" ;
				else if ( direction >= 156 && direction < 201  ) windDirection = "Z" ;
					else if ( direction >= 201 && direction < 246  ) windDirection = "ZW" ;
						else if ( direction >= 246 && direction < 291  ) windDirection = "W" ;
							else windDirection = "NW" ;

	return windDirection + " " + String(getBeaufort(speed)) ;
}