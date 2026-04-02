// UpdateDisplay.cpp - Functions for screen update.

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "UpdateDisplay.h"

void UpdateWeatherDisplay(strWeatherInfo &weather, strDateTime &dTime)
{
	// display.clearScreen();
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.drawLine(104, 0, 104, 127, 0);

	// Temp
	display.drawBitmap(118, 10, epd_bitmap_temperature, 32, 32, 0);
	display.setCursor(155, 32);
	display.setFont(&FreeSans18pt7b);
	int len = display.print(String(weather.temperature,1));
	display.setFont(&FreeSans12pt7b);
	display.drawChar(160 + len * 16, 32, 'C', GxEPD_BLACK,GxEPD_WHITE,1,1) ;

	// Rain 
	display.drawBitmap(117, 60, epd_bitmap_humidity, 32, 32, 0);
	display.setCursor(155, 89);
	display.setFont(&FreeSans18pt7b);
	display.print(String(weather.precipitation,1));
	display.setFont(&FreeSans12pt7b);
	display.setCursor(208, 89);
	display.print("mm");

	// Weather Icon
	display.drawBitmap(18, 24, selectWeatherIcon(weather.weather_code), 64, 64, 0);

	// Wind info
	display.drawXBitmap(4,93,wind_new,32,27,GxEPD_RED) ;
	display.setFont(&FreeSans12pt7b);
	display.setCursor(37, 114);
	display.print(createWindData(weather.wind_speed_10m, weather.wind_direction_10m));

	// Date and time

	String 	formatted_date = String(dTime.day) + "-" + String(dTime.month) + "-" + String(dTime.year);
	String 	formatted_time = String(dTime.hour < 10 ? "0" : "") + String(dTime.hour) + ":" + String(dTime.minute < 10 ? "0" : "") + String(dTime.minute); 

	display.setFont(&FreeSans9pt7b);
	display.setTextColor(GxEPD_RED);
	display.setCursor(6, 12);
	display.print(formatted_date);

	display.setCursor(155, 120);
	display.print(formatted_time);
	display.display();
}

void UpdatePillsDisplay(strDateTime &dTime)
{
	display.fillScreen(GxEPD_RED);
	display.setFont(&FreeSans18pt7b);
	display.setTextColor(GxEPD_WHITE);
	display.setTextSize(1);

	display.setCursor(11, 38);
	display.print("Neem je pillen!");

	display.setCursor(9, 117);
	String 	formatted_date = String(dTime.day) + "-" + String(dTime.month) + "-" + String(dTime.year);
	display.print(formatted_date);

	display.display();
}

void UpdateVersionDisplay()
{
	display.fillScreen(GxEPD_WHITE);
	display.setFont(&FreeSans9pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	// Title
	display.setCursor(10, 15);
	display.print("Weather Station v1.0");

	// Build info
	display.setCursor(10, 32);
	display.print("ESP32-C6 + E-Paper");

	// Features
	display.setCursor(10, 49);
	display.print("Features:");
	display.setCursor(20, 67);
	display.print("- Weather Display");
	display.setCursor(20, 83);
	display.print("- Pill Reminder");
	display.setCursor(20, 100);
	display.print("- Deep Sleep");

	// API info
	display.setCursor(10, 117);
	display.print("Open-Meteo API");

	display.display();
}
