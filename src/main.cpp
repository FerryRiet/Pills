#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "driver/rtc_io.h"

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "Weather.h"

time_t getNtpTime();

#include <TimeLib.h>

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 900	   // Time ESP32 will sleep (in seconds)

// ESP32-C6 CS(SS)=16,SCL(SCK)=4,SDA(MOSI)=6,BUSY=21,RES(RST)=22,DC=23
#define CS_PIN (16)
#define BUSY_PIN (21)
#define RES_PIN (22)
#define DC_PIN (23)

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ CS_PIN, /*DC=*/DC_PIN, /*RES=*/RES_PIN, /*BUSY=*/BUSY_PIN)); // GDEY0213Z98 122x250, SSD1680

// Date/time  strings
String formatted_date = "01-01-2000";
String formatted_time = "00:00:00";

// WiFi Credentials
const char *ssid = "Superwome";
const char *password = "0206697723";
const char *IP = "192.168.2.49";
const char *NM = "192.168.2.0";
const char *GW = "192.168.2.254";


// OpenWeatherMap API Info

const String weatherURL = "https://api.open-meteo.com/v1/forecast?latitude=52.35&longitude=4.80&&current=wind_direction_10m,temperature_2m,precipitation,weather_code,wind_speed_10m,rain&models=knmi_seamless";

int wifitimeout = 0;

String temperature = "";
String precipitation = "";
String windSpeed = "";
String windDirection = "";
String wind = "";

RTC_DATA_ATTR const unsigned char *icon = nullptr;
RTC_DATA_ATTR float savedTemp = 0.0;
RTC_DATA_ATTR float savedHumid = 0.0;
RTC_DATA_ATTR bool newDay = false;
RTC_DATA_ATTR int toDay = 0;


#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO) // 2 ^ GPIO_NUMBER in hex
#define WAKEUP_GPIO_0 GPIO_NUM_0				// Only RTC IO are allowed
#define WAKEUP_GPIO_1 GPIO_NUM_1				// Only RTC IO are allowed - ESP32 Pin example

// Button state constants for better readability
#define DEBUG_BUTTON 0
#define NEW_DAY_BUTTON 1
#define WAKEUP_GPIO_0_BITMASK (1ULL << WAKEUP_GPIO_0)
#define WAKEUP_GPIO_1_BITMASK (1ULL << WAKEUP_GPIO_1)

time_t getNtpTime();
void doBail(int timeOut) ;

int getBeaufort(double kmh);
void createWindData(float speed, int direction);

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
	display.drawInvertedBitmap(5, 90, wind_bitmap, 32, 32, 0);
	display.setFont(&FreeSans12pt7b);
	display.setCursor(37, 112);
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

unsigned long calculateSleepTime(int lhour, int lminute, int lsec)
{
	if (lhour < 7)
		return (7 - lhour) * 3600;
	if (lhour >= 17)
		return 3600; // Max sleep in 32bits time 10^6

	// Sync on halv hour
	lminute %= 30;
	return (29 - lminute) * 60 + 60 - lsec;
}

void setup()
{
	display.init(115200, true, 20, false);
	display.setRotation(1);

	Serial.begin(115200);

	if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1)
	{
		// Pills taken - check which button was pressed
		uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
		int pin = (wakeup_pin_mask & WAKEUP_GPIO_1_BITMASK) >> 1; // Convert bitmask to pin number

		// Button 1 (GPIO 1) = new day, Button 0 (GPIO 0) = debug mode
		newDay = (pin == NEW_DAY_BUTTON) ? true : false;
		if ( newDay) {
			UpdateVersionDisplay() ;
			doBail(10) ;
		}
	}

/* BUG */	
//	WiFi.config(IPAddress(192,168,2,49),IPAddress(192,168,2,254),IPAddress(192,168,2,0),IPAddress(8,8,8,8)) ; 
	WiFi.mode(WIFI_STA) ;
	WiFi.setHostname("Pills") ;
	WiFi.begin(ssid, password);
/* end BUG*/

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(20); // Connect in 20 msec
		wifitimeout++;
		if (wifitimeout > 200)
		{
			WiFi.disconnect();
			break;
		}
	}

	esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(WAKEUP_GPIO_0), ESP_EXT1_WAKEUP_ANY_HIGH);
	esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(WAKEUP_GPIO_1), ESP_EXT1_WAKEUP_ANY_HIGH);

	gpio_pulldown_en(WAKEUP_GPIO_0); // LP_GPIO0 is tied to GND in order to wake up in HIGH
	gpio_pullup_dis(WAKEUP_GPIO_0);	 // Disable PULL_UP in order to allow it to wakeup on HIGH
	gpio_pulldown_en(WAKEUP_GPIO_1); // LP_GPIO0 is tied to GND in order to wake up in HIGH
	gpio_pullup_dis(WAKEUP_GPIO_1);	 // Disable PULL_UP in order to allow it to wakeup on HIGH
}

//
#define SLEEPAFTERFAIL 300
void doBail(int timeOut )
{
	esp_sleep_enable_timer_wakeup(timeOut * uS_TO_S_FACTOR);
	Serial.println("Sleeping after failed: " + String(SLEEPAFTERFAIL));
	delay(10000) ;
	ESP.restart() ;
	//esp_deep_sleep_start();
}

void loop()
{
	time_t dTime = 0;
	unsigned long sleepTime;
	int lhour;
	int lminute;
	int lsec;
	int lday;
	int lyear;

	if (WiFi.status() == WL_CONNECTED)
	{
		int count = 0;
		while (dTime == 0 && count++ < 2)
		{
			dTime = getNtpTime();
		}

		lyear = year(dTime);
		if (lyear == 1970)
			doBail(300);

		lhour = hour(dTime);
		lminute = minute(dTime);
		lday = day(dTime);
		lsec = second(dTime);

		if (lday != toDay)
		{
			newDay = true;
			toDay = lday;
		}

		formatted_date = String(day(dTime)) + "-" + String(month(dTime)) + "-" + String(lyear);
		formatted_time = String(lhour < 10 ? "0" : "") + String(lhour) + ":" + String(lminute < 10 ? "0" : "") + String(lminute); //  + ":" + String(second(dTime) < 10 ? "0" : "") + String(second(dTime));

		HTTPClient http;
		http.begin(weatherURL);
		int httpCode = http.GET();

		if (httpCode > 0)
		{
			String payload = http.getString();

			JsonDocument doc;
			DeserializationError error = deserializeJson(doc, payload);
			if (!error)
			{
				savedTemp = doc["current"]["temperature_2m"];
				savedHumid = doc["current"]["precipitation"];
				int weather_code = doc["current"]["weather_code"];

				icon = selectWeatherIcon(weather_code);
				createWindData(doc["current"]["wind_speed_10m"], doc["current"]["wind_direction_10m"]);

				temperature = String(savedTemp, 1);	   //+ " °C" ;
				precipitation = String(savedHumid, 1); // + " " + String(doc["current_units"]["precipitation"]);;
			}
			else
			{
				temperature = "!JSON";
				precipitation = "!JSON";
			}
		}
		else
		{
			temperature = "!" + httpCode;
			precipitation = "!" + httpCode;
		}
		http.end();
	}

	if (WiFi.status() != WL_CONNECTED)
	{
		display.drawBitmap(4, 3, epd_bitmap_warning, 16, 16, 0);
	}
	else
		WiFi.disconnect(true, true); // Save power?

	if (newDay)
	{
		UpdatePillsDisplay(formatted_date, formatted_time);
	}
	else
	{
		sleepTime = calculateSleepTime(lhour, lminute, lsec);
		esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
		Serial.println("Sleeping for :" + String(sleepTime));
		UpdateWeatherDisplay(formatted_date, formatted_time);
	}

	display.hibernate();

	if (Serial.isPlugged())
	{ // Debug if connected.
		newDay = !newDay;
		delay(5000);
		ESP.restart();
	}
	else
	{
		esp_deep_sleep_start();
	}
}
