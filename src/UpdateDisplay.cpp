// UpdateDisplay.cpp - Functions for screen update.

#include "Weather.h"

void UpdateWeatherDisplay(String &dateStr, String &timeStr)
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
	int len = display.print(temperature);
	display.setFont(&FreeSans12pt7b);
	display.setCursor(160 + len * 16, 32); // loc waqs 208
	display.print("C");

	// Rain 
	display.drawBitmap(117, 60, epd_bitmap_humidity, 32, 32, 0);
	display.setCursor(155, 89);
	display.setFont(&FreeSans18pt7b);
	display.print(precipitation);
	display.setFont(&FreeSans12pt7b);
	display.setCursor(208, 89);
	display.print("mm");

	// Weather Icon
	display.drawBitmap(18, 24, icon, 64, 64, 0);

	// Wind info
	display.drawXBitmap(4,93,wind_new,32,27,GxEPD_RED) ;
	display.setFont(&FreeSans12pt7b);
	display.setCursor(37, 114);
	display.print(wind);

	// Date and time
	display.setFont(&FreeSans9pt7b);
	display.setTextColor(GxEPD_RED);
	display.setCursor(6, 12);
	display.print(dateStr);

	display.setCursor(155, 120);
	display.print(timeStr);
	display.display();
}

void UpdatePillsDisplay(String &dateStr, String &timeStr)
{
	display.fillScreen(GxEPD_RED);
	display.setFont(&FreeSans18pt7b);
	display.setTextColor(GxEPD_WHITE);
	display.setTextSize(1);

	display.setCursor(11, 38);
	display.print("Neem je pillen!");

	display.setCursor(9, 117);
	display.print(dateStr);

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
