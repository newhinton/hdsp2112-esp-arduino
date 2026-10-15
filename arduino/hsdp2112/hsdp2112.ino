#include <WiFi.h>
#include <time.h>
#include "HDSP21xx.h"
#include <RTClib.h>
#include <Wire.h>

#define SDA_PIN 25
#define SCL_PIN 4

HDSP21xx segmentDisplay;
RTC_DS1307 rtc;

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";
const char* NTP_SERVER = "pool.ntp.org";

// Germany: CET/CEST handled automatically
const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3";

int lastMinute = -1;
int lastDay = -1;
bool colon = false;
unsigned long lastBlink = 0;

const char* days[] = {
  "Sun", "Mon", "Tue", "Wed",
  "Thu", "Fri", "Sat"
};


#define DEBUG_MODE 0

#ifdef DEBUG_MODE
  #define print(x)     Serial.print(x)
  #define println(x)   Serial.println(x)
#else
#endif

  #define print(x)     Serial.print(x)
  #define println(x)   Serial.println(x)

void setup() {

  segmentDisplay.prepare();
  segmentDisplay.setBrightness(7);
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!rtc.begin()) {
    println("RTC module is NOT found");
    segmentDisplay.setText("No RTC!");
    delay(1000);
  } else {
    println("RTC module is found");
    int x = rtc.isrunning();
    println("RTC CH: " + String(x));
    if (!rtc.isrunning()) {
      println("RTC is NOT running, let's set the time!");
      setRtcTime();
    }
  }
  setRtcTime();
}

void setRtcTime() {
  connectWifi();

  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  DateTime dnow = DateTime(
    t->tm_year + 1900,
    t->tm_mon + 1,
    t->tm_mday,
    t->tm_hour,
    t->tm_min,
    t->tm_sec);
  rtc.adjust(dnow);
  restoreRtcTime();
}

void restoreRtcTime() {
  DateTime dt = rtc.now();

  struct tm t = {};
  t.tm_year = dt.year() - 1900;
  t.tm_mon = dt.month() - 1;
  t.tm_mday = dt.day();
  t.tm_hour = dt.hour();
  t.tm_min = dt.minute();
  t.tm_sec = dt.second();
  t.tm_isdst = -1;

  time_t rtcTime = mktime(&t);

  if (rtcTime == (time_t)-1) {
    println("mktime() failed!");
    return;
  }

  struct timeval tv;
  tv.tv_sec = rtcTime;
  tv.tv_usec = 0;

  settimeofday(&tv, nullptr);
}


void printRtcTime() {
  DateTime now = rtc.now();
  print(now.year());
  print("-");
  print(now.month());
  print("-");
  print(now.day());
  print(" ");
  print(now.hour());
  print(":");
  print(now.minute());
  print(":");
  println(now.second());
}


void connectWifi() {
  WiFi.setHostname("HDSP211x Clock");
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    print(".");
    segmentDisplay.setText("Wifi");
    delay(250);
    segmentDisplay.setText("Wifi.");
    delay(250);
    segmentDisplay.setText("Wifi..");
    delay(250);
    segmentDisplay.setText("Wifi...");
    delay(250);
  }

  segmentDisplay.marquee("Connected!", 300);
  println("\nWiFi connected");
  getTime();
}

void getTime() {


  // Get time from NTP
  configTzTime(TZ_INFO, NTP_SERVER);

  // Wait until time has been synchronized
  struct tm timeinfo;

  while (!getLocalTime(&timeinfo)) {
    println("Waiting for NTP...");
    segmentDisplay.setText("NTP");
    delay(150);
    segmentDisplay.setText("NTP.");
    delay(150);
    segmentDisplay.setText("NTP..");
    delay(150);
    segmentDisplay.setText("NTP...");
    delay(150);
  }

  println("Time synchronized");
  segmentDisplay.marquee("Got time!", 300);
	WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  segmentDisplay.marquee("Wifi off", 300);
}

void loop() {

  unsigned long nowMs = millis();

  // Blink colon every 500 ms
  if (nowMs - lastBlink >= 1000) {
    lastBlink = nowMs;
    colon = !colon;

    segmentDisplay.setChar(colon ? ':' : ' ', 5);

    if (colon) {
      printRtcTime();
    }
  }

  time_t now = time(nullptr);
  struct tm* t = localtime(&now);

  if (t->tm_min != lastMinute) {
    lastMinute = t->tm_min;

    segmentDisplay.setChar('0' + t->tm_hour / 10, 3);
    segmentDisplay.setChar('0' + t->tm_hour % 10, 4);
    segmentDisplay.setChar('0' + t->tm_min / 10, 6);
    segmentDisplay.setChar('0' + t->tm_min % 10, 7);
  }

  // Update weekday when it changes
  if (t->tm_wday != lastDay) {
    lastDay = t->tm_wday;

    segmentDisplay.setChar(days[t->tm_wday][0], 0);
    segmentDisplay.setChar(days[t->tm_wday][1], 1);
    segmentDisplay.setChar(days[t->tm_wday][2], 2);
  }


  delay(100);
}