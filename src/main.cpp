#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "driver/rtc_io.h"

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "Weather.h"
#include "UpdateDisplay.h"
#include "NTPtimeESP.h"

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

void doBail(int timeOut);
int getBeaufort(double kmh);
void createWindData(float speed, int direction);


unsigned long calculateSleepTime(int lhour, int lminute, int lsec)
{
	if (lhour < 7)
		return (7 - lhour) * 3600;
	if (lhour >= 17)
		return (59 - lminute) * 60 + 60 - lsec; // Max sleep in 32bits time 10^6

	// Sync on halv hour
	lminute %= 30;
	return (29 - lminute) * 60 + 60 - lsec;
}

void setup()
{
	pinMode(16, 0x3) ;
	pinMode(22, 0x3) ;
	pinMode(23, 0x3) ;

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
	if (Serial.isPlugged())
	{ // Debug if connected.
		newDay = !newDay;
		delay(10000);
		ESP.restart();
	}
	else {
		esp_sleep_enable_timer_wakeup(timeOut * uS_TO_S_FACTOR);
		Serial.println("Sleeping after failed: " + String(SLEEPAFTERFAIL));
		esp_deep_sleep_start();
	}
}

void loop()
{
	NTPtime ntptime("nl.pool.ntp.org") ;
	strDateTime dTime;
	
	unsigned long sleepTime;


	if (WiFi.status() == WL_CONNECTED)
	{
		int count = 0;

		dTime = ntptime.getNTPtime(1.0,1) ;
		while ( !dTime.valid ) { // Wait for udp recieve and retransmit after timeout.
			delay(20) ;
			dTime = ntptime.getNTPtime(1.0,1) ;
		}
		
		if ( dTime.day != toDay)
		{
			newDay = true;
			toDay = dTime.day;
		}

		formatted_date = String(dTime.day) + "-" + String(dTime.month) + "-" + String(dTime.year);
		formatted_time = String(dTime.hour < 10 ? "0" : "") + String(dTime.hour) + ":" + String(dTime.minute < 10 ? "0" : "") + String(dTime.minute); 
		
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
		sleepTime = calculateSleepTime(dTime.hour, dTime.minute, dTime.second);
		esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
		Serial.println("Sleeping for :" + String(sleepTime));
		UpdateWeatherDisplay(formatted_date, formatted_time);
	}

	display.hibernate();

	if (Serial.isPlugged())
	{ // Debug if connected.
		newDay = !newDay;
		delay(10000);
		ESP.restart();
	}
	else
	{
		esp_deep_sleep_start();
	}
}
