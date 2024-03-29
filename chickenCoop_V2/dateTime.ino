/* date/time API calling and parsing/printing data
 * - NTP and sunrise-sunset
 * - look at NTP documentation
 */
// helper functions for sunrise and sunset states w/ offsets
bool timeToOpen() {
  // if it is [offset_open] minutes til sunrise, e.g. 45 minutes before 6:20
  int8_t openHour, openMin;
  openMin = SUNRISE_MINUTE + offset_open;
  if (openMin > 59) { 
    openMin -= 60;
    openHour = SUNRISE_HOUR+1;
  } else if (openMin < 0) {
    openMin += 60;
    openHour = SUNRISE_HOUR-1;
  } else {
    openHour = SUNRISE_HOUR;
  }
  if (CURRENT_HOUR >= openHour && CURRENT_MINUTE >= openMin) {
    return true;
  } else {
    return false;
  }
}

bool timeToClose() {
  // if it is [offset_close] minutes after sunset
  int8_t closeHour, closeMin;
  closeMin = SUNSET_MINUTE + offset_close;
  if (closeMin > 59) { 
    closeMin -= 60;
    closeHour = SUNSET_HOUR+1;
  } else if (closeMin < 0) {
    closeMin += 60;
    closeHour = SUNSET_HOUR-1;
  } else {
    closeHour = SUNSET_HOUR;
  }
  if (CURRENT_HOUR >= closeHour && CURRENT_MINUTE >= closeMin) {
    return true;
  } else {
    return false;
  }
}

void updateLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
//    Serial.println(F("Failed to obtain time"));
    DATETIME_RDY = 0;
    return;
  }
  DATETIME_RDY = 1;
  CURRENT_YEAR = timeinfo.tm_year+1900;
  CURRENT_MONTH = timeinfo.tm_mon+1;
  CURRENT_DATE = timeinfo.tm_mday;
  CURRENT_HOUR = timeinfo.tm_hour;
  CURRENT_MINUTE = timeinfo.tm_min;
  DST_FLAG = timeinfo.tm_isdst;
  
  prevTime = currentTime;
  broadcastChange('t');
}

void getSunTimes() {
  // https://www.dfrobot.com/blog-917.html
  WiFiClient client;
  HTTPClient http;
  http.begin(client, sunRiseSetAPI);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    http.end();
    StaticJsonDocument<520> doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (err) {
      return;
    }
    parseUTCString(doc["results"]["sunrise"], sunriseVals);
    parseUTCString(doc["results"]["sunset"], sunsetVals);
    broadcastChange('n');
  }
}

// unpack the string from sunrise-sunset API
// input format: "HH:MM:SS XM" in UTC or "H:MM:SS XM"
// output: [ready, hour, min] in 24-hour Mountain time
void parseUTCString(String UTCString, int8_t (&buffArray)[3]) {
  uint8_t hour, minute;
  uint8_t firstColon = UTCString.indexOf(':');
  if (firstColon == 1 || firstColon == 2) {
    hour = UTCString.substring(0, firstColon).toInt();
    minute = UTCString.substring(firstColon+1, firstColon+3).toInt();
    char AMPM = (UTCString.charAt(firstColon+7));
    uint8_t DST;
    if (DST_FLAG > 0) { // if daylight savings
      DST = 1;} else {DST = 0;}
    if (AMPM == 'A') { // for AM, subtract GMT offset
      hour = ((hour % 12)+24+TZ+DST) % 24;
    } else if (AMPM == 'P') {
      hour = ((hour % 12)+12+TZ+DST) % 24;
    }
    buffArray[0] = 1;
    buffArray[1] = hour;
    buffArray[2] = minute;
    return;
  }
  buffArray[0] = 0;
  buffArray[1] = -1;
  buffArray[2] = -1; // error when hour/min is negative
}

void buildTimeString() {
  message = F("time: ");
  message += (CURRENT_HOUR > 12) ? CURRENT_HOUR - 12 : CURRENT_HOUR;
  message += ':';
  if (CURRENT_MINUTE < 10) {
    message += '0';
  }
  message += CURRENT_MINUTE;
  message += ' ';
  message += (CURRENT_HOUR > 12) ? F("PM") : F("AM");
}

//void printLocalTime() {
//  char DST;
//  if (DATETIME_RDY > 0) {
//    if (DST_FLAG > 0) { // if daylight savings,
//      DST = 'D';
//    } else { DST = 'S';}
//    Serial.printf("Today's date - %u/%u/%u\n", 
//      CURRENT_YEAR, CURRENT_MONTH, CURRENT_DATE);
//    Serial.printf("Local time - %u:%u M%cT\n", 
//      CURRENT_HOUR, CURRENT_MINUTE, DST);
//  } else {
//    Serial.printf("TIMEDATE ERRROR [%u] - %u/%u/%u %u:%u DST:%u\n", 
//      DATETIME_RDY, CURRENT_YEAR, CURRENT_MONTH, 
//      CURRENT_DATE, CURRENT_HOUR, CURRENT_MINUTE, 
//      DST_FLAG);
//  }
//}

//void printSunTimes() {
//  if (SUNRISE_RDY) {
//    Serial.printf("Sunrise time - %u:%u\n", 
//      SUNRISE_HOUR, SUNRISE_MINUTE);
//  } else {
//    Serial.printf("SUNRISE TIME ERROR [%u] - %u:%u\n", 
//      SUNRISE_RDY, SUNRISE_HOUR, SUNRISE_MINUTE);
//  }
//  if (SUNSET_RDY) {
//    Serial.printf("Sunset time - %u:%u\n", 
//      SUNSET_HOUR, SUNSET_MINUTE);
//  } else {
//    Serial.printf("SUNSET TIME ERROR [%u] - %u:%u\n", 
//      SUNSET_RDY, SUNSET_HOUR, SUNSET_MINUTE);
//  }
//}
