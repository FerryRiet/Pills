# Pills — AGENTS.md

## Board & Build

- **Board:** `seeed-xiao-esp32-c6` (ESP32-C6, not C3).
- **Platform:** `https://github.com/Seeed-Studio/platform-seeedboards.git` (custom, not standard espressif32).
- **Build:** `pio run`, upload `pio run --target upload`, monitor `pio run -t monitor`, clean `pio run --target clean`.
- **Two environments:** `seeed_xiao_esp32_c6` (160MHz default) and `seeed-xiao-esp32-c6` (80MHz, power-efficient, with `huge_app.csv` partition). Use `pio run -e seeed-xiao-esp32-c6` to target one.
- **Flags:** `-std=gnu++17`, `-Wall`. Partition table: `huge_app.csv` on the low-power env.

## Display

- **Compile-time selection** via `#define` — options: `USE_290_C90C` (2.9", 128×296), `USE_420C_Z21`, `USE_213_Z98C` (2.13", currently active). Change in `main.cpp:1` and `include/UpdateDisplay.h:5`.
- Pins: CS=16, BUSY=21, RES=22, DC=23. Init: `display.init(115200, true, 20, false); display.setRotation(1);`
- Always call `display.display(false)` (partial update) then `display.hibernate()`.

### Display Layouts (5 functions in `src/UpdateDisplay.cpp`)

| Function | Trigger | Layout |
|---|---|---|
| `UpdateWeatherDisplay` | Normal cycle | Weather icon (64×64, left), temp + °C (right), precip (mm, right), wind dir+Beaufort (bottom-left), date+time (red). Vertical divider at x=104 |
| `UpdatePillsDisplay` | New day (GPIO 1) | Red background, "Neem je pillen!" + date + pills icon |
| `UpdateErrorDisplay` | Weather fetch failed | Stop-sign icon, date (red), time (red) |
| `UpdateVersionDisplay` | Boot after GPIO 1 wake | White bg, "Weather Station v1.0", features list, "Open-Meteo API" |
| `UpdateSensorDisplay` | Local BME680 | Local temp, humidity %, pressure hPa (y=140,160,180) — called after weather draw |

## Wake / Sleep

- **GPIO 0** = debug bypass button, **GPIO 1** = new-day pill reminder button (both EXT1 wakeup, active HIGH).
- Configured as `INPUT` with internal pulldown enabled, pullup disabled.
- **Debug mode:** if `Serial.isPlugged()` → `doBail()` causes ESP.restart() in 10s; in `loop()` it re-sleeps for 10s. Plug USB to disable deep sleep.
- Sleep schedule: before 7am → wake at 7am; 7-17h → sync on half-hour boundary; after 17h → sleep ~1h.

## Weather API

- URL defined in `include/open-meteo.h:12` as `const String WeatherURL`.
- Open-Meteo with `knmi_seamless` model. Latitude=52.35, longitude=4.80 (Amsterdam).
- Response parsed into `strWeatherInfo` (8 fields + `time` string) in `open-meteo.cpp:37-45`.

## Local Sensor (BME680)

- I²C on SDA=17, SCL=2. **Temperature correction** `TEMP_CORRECTION = -2.7` K in `UpdateDisplay.cpp:130`.
- `setupSensor()` in `LocalSensor.cpp`, `UpdateSensorDisplay()` in `UpdateDisplay.cpp`.

## WiFi

- **Credentials are hardcoded** in `main.cpp:42-43`. Change ssid/password there.
- Hostname set to `"Pills"`. 4s timeout (200×20ms) before deep-sleep bail.

## NTP

- Server: `nl.pool.ntp.org`, CET timezone (GMT+1), European summer time auto-detect.
- Library is a local file `src/NTPtimeESP.cpp` (not TimeLib).

## Misc

- Weather icons stored in `src/Weather.cpp` as `PROGMEM` arrays. Icon selection in `selectWeatherIcon()` (`src/Weather.cpp:443`).
- Wind: Beaufort scale + Dutch cardinal directions (N/NO/O/ZO/Z/ZW/W/NW) in `src/Wind.cpp`.
- No tests, no CI, no linter/typechecker.
