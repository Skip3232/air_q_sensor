// Функция отправки данных по протоколу MQTT
void MQTT_send(){
   client.loop();
   uint32_t nows = millis();
  if ((nows - lastMsg > settings.off_time * 60000 && settings.mqtt_en or !annonce_mqtt_discovery && settings.mqtt_en)) {
    if(WiFi.status() == WL_CONNECTED) {
        MQTT_send_data("jsondata", JSON_DATA());
     }
    lastMsg = nows; 
  }
 }
void MQTT_send_data(String topic, String data){
         String root = settings.mqtt_topic;
         String top  = root +"/"+ topic;
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;
         IPAddress Remote_MQTT_IP;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
             WiFi.hostByName(ipAddress.c_str(), Remote_MQTT_IP);
          }
      
      client.setServer(Remote_MQTT_IP, port.toInt());

      client.setBufferSize(2048); 
           if(client.connected()){                                                                                    // Вторичная отправка данных при подключенном брокере 
          count_rf = 0;
          send_mqtt(top, data);
           }else{
              count_rf++;
              if (client.connect(ch_id.c_str(), settings.mqtt_user, settings.mqtt_passw)){                            // Первичная отправка данных, выполняестя попытка подключения к брокеру 
                    send_mqtt(top, data);          
                }else{
                  if(count_rf > 2){                                                                                    // Если были неудачные попытки подключения, то переподключаем Wi-fi
                     WiFi.disconnect();
                     WiFi.begin(settings.mySSID, settings.myPW);
                     count_rf = 0;
                }
            }
        }
     
}


void send_mqtt(String tops, String data) {
    if(!annonce_mqtt_discovery) {
        String device_id = "air_qality_sensor" + ch_id;
        String configuration_url = "http://" + WiFi.localIP().toString();
        String top = String(settings.mqtt_topic) + "/jsondata";

        // Создаем конфигурацию устройства
        DynamicJsonDocument deviceDoc(1024);
        deviceDoc["ids"][0] = device_id;
        deviceDoc["name"] = "Датчик качества воздуха";
        deviceDoc["mdl"] = version_code;
        deviceDoc["sw"] = "0.1.1";
        deviceDoc["mf"] = "CYBEREX TECH";
        deviceDoc["configuration_url"] = configuration_url;

        // Вспомогательная функция для публикации конфигурации
        auto publishConfig = [&](const String& type, const String& entity_id, JsonDocument& doc) {
            doc["device"] = deviceDoc.as<JsonObject>();
            String payload;
            serializeJson(doc, payload);
            client.publish(("homeassistant/" + type + "/" + entity_id + "/config").c_str(), 
                          payload.c_str(), true);
        };
        //  Датчик TVOC
        {
            DynamicJsonDocument doc = mqqt_d_annonce("TVOC", "TVOC", "volatile_organic_compounds_parts", "ppb", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_TVOC", doc);
        } 
        //  Датчик ECO2
        {
            DynamicJsonDocument doc = mqqt_d_annonce("eCO₂", "ECO2", "carbon_dioxide", "ppm", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_ECO2", doc);
        }
        //  Датчик температуры
        {
            DynamicJsonDocument doc = mqqt_d_annonce("Температура", "temp", "temperature", "°C", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_temp", doc);
        }
        //  Датчик влажности
        {
            DynamicJsonDocument doc = mqqt_d_annonce("Влажность", "hum", "humidity", "%", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_hum", doc);
        }
        //  Датчик общего индекса качества воздуха
        {
            DynamicJsonDocument doc = mqqt_d_annonce("AQI", "AQI", "aqi", "", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_AQI", doc);
        }
         //  Датчик статуса
        {
            DynamicJsonDocument doc = mqqt_d_annonce("Статус", "stat", "wind_direction", "", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_stat", doc);
        }
        //  Время опроса
        {
            DynamicJsonDocument doc = mqqt_d_annonce("Переодичность отправки", "sllep_t", "duration", "min", top, ch_id, "");
            publishConfig("sensor", ch_id + "_d_sllep_t", doc);
        }

        annonce_mqtt_discovery = true;
    }
    
    // Отправляем данные 
    client.publish(tops.c_str(), data.c_str());
}

// Вспомогательная функция для формирования json конфигурации сенсора
DynamicJsonDocument mqqt_d_annonce(String namec, String cn, String devclass, String unit, String top, String ch_id, String icon){
        DynamicJsonDocument doc(512);
            if(devclass != ""){
              doc["device_class"] = devclass;
            }
            doc["name"] = namec;
            doc["state_topic"] = top;
            if(unit != ""){
              doc["unit_of_measurement"] = unit;
            }
            doc["value_template"] = "{{ value_json."+ cn +" }}";
            //doc["suggested_display_precision"] = 2; 
            doc["unique_id"] = ch_id + "_d_" + cn;
            if (icon != "") {
              doc["icon"] = icon;
            }
        return doc;
}

String MQTT_status(){
    return client.connected() ? "подключен" : "отключен";
} 
