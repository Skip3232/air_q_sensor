String millis2time() {
    unsigned long totalSeconds = millis() / 1000;
    
    int days = totalSeconds / 86400;           // 86400 = 24 * 3600
    int hours = (totalSeconds % 86400) / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    
    String timeStr = String(days) + ":" + 
                     twoDigits(hours) + ":" + 
                     twoDigits(minutes) + ":" + 
                     twoDigits(seconds);
    return timeStr;
}

 String twoDigits(int digits){
        if(digits < 10) {
          String i = '0'+String(digits);
          return i;
         }else{
          return String(digits);
         }
      }

void time_work(){
   if (captivePortal()) {  
    return;
  }

  if (validateToken()){
    sensor_get();
    StaticJsonDocument<380> doc;

     doc["time"]      = millis2time();
     doc["temp"]      = String(temp);
     doc["hum"]       = String(hum);
     doc["AQI"]       = String(AQI);
     doc["TVOC"]      = String(TVOC);
     doc["ECO2"]      = String(ECO2);
     doc["MQTT"]      = MQTT_status();
     doc["ensStatus"] = String(ensStatus);

     String outjson;
     serializeJson(doc, outjson);
     server.send(200, "text", outjson);    
  }   
}



String JSON_DATA() {
    sensor_get();
    StaticJsonDocument<380> doc;  
    
     doc["temp"]      = String(temp);
     doc["hum"]       = String(hum);
     doc["AQI"]       = String(AQI);
     doc["TVOC"]      = String(TVOC);
     doc["ECO2"]      = String(ECO2);
     doc["sllep_t"]   = String(settings.off_time);
     doc["stat"]      = String(ensStatus);
    
    String outjson;
    serializeJson(doc, outjson);
    return outjson;
}
