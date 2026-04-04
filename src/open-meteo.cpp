// Open-Meteo implementation file
// Implementation of getWeatherInfo function

#include "open-meteo.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

strWeatherInfo getWeatherInfo()
{
  strWeatherInfo info;
  
  // Set default values
  info.valid = false;
  info.temperature = 0.0;
  info.surface_pressure = 0.0;
  info.weather_code = 0;
  info.relative_humidity_2m = 0;
  info.precipitation = 0.0;
  info.wind_speed_10m = 0.0;
  info.wind_direction_10m = 0;
  
  HTTPClient http;

  http.begin(WeatherURL);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    String payload = http.getString();
    
		JsonDocument doc;
		DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      info.temperature = doc["current"]["temperature_2m"] | 0.0f;
      info.surface_pressure = doc["current"]["pressure"] | 0.0f;
      info.weather_code = doc["current"]["weather_code"] | 0;
      info.relative_humidity_2m = doc["current"]["humidity_2m"] | 0;
      info.precipitation = doc["current"]["precipitation"] | 0.0f;
      info.wind_speed_10m = doc["current"]["wind_speed_10m"] | 0.0f;
      info.wind_direction_10m = doc["current"]["wind_direction_10m"] | 0;

      info.valid = true ;
    }
  }
  
  http.end();
  
  return info;
}
void printWeatherInfo(const strWeatherInfo &str) {
  Serial.println(str.temperature) ;
  Serial.println(str.wind_speed_10m) ;
  Serial.println(str.weather_code) ;
  Serial.println(str.precipitation) ;
  Serial.println(str.relative_humidity_2m) ;

}