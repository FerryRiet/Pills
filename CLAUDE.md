# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Pills** is an ESP32-C3 based weather station and pill reminder device using e-paper display. The device:
- Displays weather information from Open-Meteo API (temperature, precipitation, wind, weather icon)
- Shows daily pill reminders ("Neem je pillen!")
- Uses deep sleep between updates for power efficiency
- Wakes up via GPIO buttons (GPIO 0 for debug, GPIO 1 for new day toggle)

## Build System

This is a **PlatformIO** project. Build using PlatformIO CLI:

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio run -t monitor

# Clean build
pio run --target clean
```

## Architecture

### Source Files
- **`src/main.cpp`** - Main entry point, WiFi/HTTP client for weather API, deep sleep logic, button wakeup handling
- **`src/Wind.cpp`** - Wind data functions: `createWindData()` calculates Beaufort scale and wind direction (N, NO, O, ZO, Z, ZW, W, NW)
- **`src/Time.cpp`** - NTP time sync via `europe.pool.ntp.org`, timezone CET (GMT+1)

### Display Implementation
Uses **GxEPD2** library with GDEY0213Z98c e-paper (122x250 pixels, SSD1680 controller):
- ESP32-C3 pins: CS=16, BUSY=21, RES=22, DC=23
- Weather icons are bitmap arrays defined in main.cpp (clear_sky, rain, thunderstorm, etc.)
- Display hibernates after update via `display.hibernate()`

### External Dependencies
- **GxEPD** - E-paper display driver library
- **ArduinoJson** - JSON parsing for Open-Meteo API response
- **TimeLib** - NTP time handling
- **WiFi.h / WiFiUdp.h** - Network connectivity and NTP protocol

## Key Functions

### Weather Icon Selection (`selectWeatherIcon(int weather_code)`)
Maps WMO weather codes to bitmap icons:
| Weather Code | Icon |
|--------------|------|
| 0-3          | Clear/broken clouds |
| 40-49        | Mist |
| 50-60        | Shower rain |
| 61-70        | Rain |
| 80-89        | Snow |
| 90-100      | Thunderstorm |

### Deep Sleep Logic (`loop()`)
- Checks for new day (triggers pill reminder display)
- Otherwise calculates sleep time based on current hour:
  - Before 7am: sleep until 7am
  - After 20h: sleep 2 hours
  - After 17h: sleep 1 hour
  - Between 7-17h: sync at half-hour

### Button Wakeup
Two GPIO pins (0 and 1) configured for deep sleep wakeup:
- GPIO 0: Debug mode button
- GPIO 1: New day button
Uses bitmask-based wakeup detection via `esp_sleep_get_ext1_wakeup_status()`

## PlatformIO Configuration

Check `.platformio.ini` for:
- Board definition (`seeed_xiao_esp32_c6`)
- Framework (ESP32 Arduino core)
- Compiler flags and library dependencies

## Common Development Tasks

### Debug Mode
Connect USB serial cable to trigger debug loop (bypasses deep sleep):
```bash
pio run -t monitor
```

### Modify WiFi Credentials
Edit constants in `main.cpp`:
```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

### Add New Weather Icon
Add a new bitmap array following the pattern of existing icons ( PROGMEM unsigned char array), then update `selectWeatherIcon()` to handle it.

## API Integration

The device uses Open-Meteo API at:
```
https://api.open-meteo.com/v1/forecast?latitude=52.35&longitude=4.80&current=wind_direction_10m,temperature_2m,precipitation,weather_code,wind_speed_10m,rain&models=knmi_seamless
```

Response is parsed via ArduinoJson to extract:
- `current.temperature_2m` → saved to temperature variable
- `current.precipitation` → saved to precipitation variable
- `current.weather_code` → used for icon selection
- `current.wind_speed_10m` & `current.wind_direction_10m` → passed to createWindData()
