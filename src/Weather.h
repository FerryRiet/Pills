#ifndef WEATHER_H
#define WEATHER_H

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

// Function to select weather icon based on weather code
const unsigned char *selectWeatherIcon(int weather_code);

#endif // WEATHER_H