/* 
 * Датчик качества воздуха
 *  © CYBEREX Tech, 2026
 *  на базе модуля  esp02 (esp8285)
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266SSDP.h>
#include <PubSubClient.h>
#include "const_js.h"
#include <ArduinoJson.h>
#include "ENS160_AHT2x.h" 
#include <Wire.h>
// Пины подключения к датчику
#define sda_pin    14                    // SDA 
#define scl_pin    12                    // SCL

// Пин для активации режима конфигурации
#define config_pin   5                   // RX UART

// Питание сенсора
#define sensor_pin   4                   // TX UART

// Максимальное количество записей (12 часов * 60 минут = 720)
#define MAX_RECORDS 720

// Структура для хранения данных датчика
struct SensorRecord {
    unsigned long timestamp;  // 4 байта
    short temp;               // 2 байта (температура * 10)
    short hum;                // 2 байта (влажность * 10)
    short aqi;                // 2 байта
    short tvoc;               // 2 байта
    short eco2;               // 2 байта
};  // Итого: 14 байт на запись вместо ~28

SensorRecord sensorHistory[MAX_RECORDS];
int recordCount = 0;
int currentIndex = 0;

float temp(NAN), hum(NAN);    // Переменные для хранения значений датчика

// WEBs
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
IPAddress apIP(10, 10, 20, 1);
IPAddress netMsk(255, 255, 255, 0);
ESP8266HTTPUpdateServer httpUpdater;

// DNS server
const byte             DNS_PORT = 53;
DNSServer              dnsServer;

// Текущий статус WLAN
unsigned int status = WL_IDLE_STATUS;

// Переменные для хранения статуса сетевого подключения
bool connect;
bool wi_fi_st;

bool stat_reboot, led_stat, stat_wifi, load_on, stat, annonce_mqtt_discovery, sens_stat_q, sens_stat_th; 

// Переменные используемые для CaptivePortal
const char *myHostname  = "cyberex_AQ_sensor";            // Имя создаваемого хоста cyberex_AQ_sensor.local 
const char *softAP_ssid = "CYBEREX-AIR-Q";                // Имя создаваемой точки доступа Wi-Fi

String version_code = "AIRQS-1.11.02.26";

float distance = 0.0;
float auto_on_dist;
//timers
uint32_t low_t, med_t, prevMills, lastMsg, impOnDelay, reboot_t, lastConnectTry, led;

String inputString = "";
bool off_status; 

int count_rf = 0;
int level_couts = 0;
int ensStatus(NAN), AQI(NAN), TVOC(NAN), ECO2(NAN); 


 // Структура для хранения токенов сессий 
struct Token {
    String value;
    long created;
    long lifetime;
};

Token   tokens[20];                    // Длина массива структур хранения токенов 

#define TOKEN_LIFETIME 6000000         // время жизни токена в секундах

#define MAX_STRING_LENGTH 30           // Максимальное количество символов для хранения в конфигурации


// Структура для хранения конфигурации устройства
struct {
     bool    mqtt_en;
     int     off_time;
     int     statteeprom; 
     char    mySSID[MAX_STRING_LENGTH];
     char    myPW[MAX_STRING_LENGTH];
     char    mqtt_serv[MAX_STRING_LENGTH];
     char    mqtt_user[MAX_STRING_LENGTH];
     char    mqtt_passw[MAX_STRING_LENGTH];
     char    mqtt_topic[MAX_STRING_LENGTH];
     char    passwd[MAX_STRING_LENGTH]; 
  } settings;


String ch_id = String(ESP.getChipId());

Sensor_ENS160 ENS160;     //  Инициализируем класс датчика качества воздуха
AHT20 AHT21;              //  Инициализируем класс датчика температуры и влажности

void setup() { 
    // В начале setup() добавьте:
    Serial.begin(115200);
    Serial.println();
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());                                         
    EEPROM.begin(sizeof(settings));                                 // Инициализация EEPROM в соответствии с размером структуры конфигурации       
    pinMode(sensor_pin, OUTPUT);
    digitalWrite(sensor_pin, HIGH);                                 // Включаем питание датчика
    Wire.begin(sda_pin, scl_pin);                                   // Инициализация шины I2C
    for(int x = 0; x < 160; x ++){                                  // Цикл ожидания подключения датчика качества воздуха
      if (ENS160.begin()){ 
            sens_stat_q = true;
            break; }
      delay(100); 
     }
    for(int x = 0; x < 160; x ++){                                  // Цикл ожидания подключения датчика влажности и температуры
      if (AHT21.begin()){ 
            sens_stat_th = true;
            break; }
      delay(100); 
     }
    read_config();                                                  // Чтение конфигурации из EEPROM
    check_clean();                                                  // Провека на запуск устройства после первичной прошивки
    if(String(settings.passwd) == ""){                               // Если  переменная settings.passwd пуста, то назначаем пароль по умолчанию
         strncpy(settings.passwd, "admin", MAX_STRING_LENGTH);
     }

     WiFi.mode(WIFI_STA);                                           // Выбираем режим клиента для сетевого подключения по Wi-Fi                
     WiFi.hostname("Q-Air-Sensor [CYBEREX TECH]");                  // Указываеем имя хоста, который будет отображаться в Wi-Fi роутере, при подключении устройства
     if(WiFi.status() != WL_CONNECTED) {                            // Инициализируем подключение, если устройство ещё не подключено к сети
           WiFi.begin(settings.mySSID, settings.myPW);
      }

     for(int x = 0; x < 160; x ++){                                  // Цикл ожидания подключения к сети (80 сек)
          if (WiFi.status() == WL_CONNECTED){ 
                stat_wifi = true;
                break;
           }
               delay(100); 
      
          }

     if(WiFi.status() != WL_CONNECTED) {                             // Если подключение не удалось, то запускаем режим точки доступа Wi-Fi для конфигурации устройства
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAPConfig(apIP, apIP, netMsk);
            WiFi.softAP(softAP_ssid);
            stat_wifi = false;
        }
        
        delay(2000);
        // Запускаем DNS сервер
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", apIP);
        // WEB страницы
        server.on("/", page_process);
        server.on("/login", handleLogin);
        server.on("/script.js", reboot_devsend);
        server.on("/style.css", css);
        server.on("/index.html", HTTP_GET, [](){
        server.send(200, "text/plain", "Radar Smart Switch"); });
        server.on("/description.xml", HTTP_GET, [](){SSDP.schema(server.client());});
        server.on("/chart", []() {
          server.send_P(200, "text/html", CHART_PAGE);
          });
        server.on("/inline", []() {
          server.send(200, "text/plain", "this works without need of authentification");
          });
        server.on("/api/data", HTTP_GET, []() {
          sendHistoryData();
          });
        server.on("/api/clear", HTTP_POST, []() {
          clearHistory();
          server.send(200, "application/json", "{\"success\": true}");
          });
        server.onNotFound(handleNotFound);
        // Здесь список заголовков для записи
        const char * headerkeys[] = {"User-Agent", "Cookie"} ;
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
        // Запускаем на сервере отслеживание заголовков 
        server.collectHeaders(headerkeys, headerkeyssize);
        server.begin();
        connect = strlen(settings.mySSID) > 0;                                            // Статус подключения к Wi-Fi сети, если есть имя сети
        SSDP_init();
        ENS160.setOperatingMode(SFE_ENS160_RESET);
        ENS160.setOperatingMode(SFE_ENS160_STANDARD);     
        initHistory();             
}

void loop() {
        server.handleClient();
        portals();
        dnsServer.processNextRequest();
        server.handleClient(); 
        reboot_dev_delay();
        MQTT_send();
        //led_ind();
}

void reboot_devsend(){
     String input = server.arg("script");         // Js скрипт процессинг
       if (input == "reb_d"){
               server.send(200, "text", FPSTR(reb_d));
         }else if(input == "config_js"){
               server.send(200, "text", FPSTR(config_js));
         }else if(input == "update_js"){
               server.send(200, "text", FPSTR(update_js));
         }else if(input == "pass_js"){
               server.send(200, "text", FPSTR(pass_js));
         }else if(input == "js_wifi"){
               server.send(200, "text", FPSTR(js_wifi));
         }
}

void sensor_get(){                                // Обновляем данные с датчика
    if(ENS160.checkDataStatus() && sens_stat_q){
         AQI  = ENS160.getAQI();
         TVOC = ENS160.getTVOC();
         ECO2 = ENS160.getECO2();
         ensStatus = ENS160.getFlags();
     }
     if(sens_stat_th){
         temp = AHT21.getTemperature() - 2.0;
         hum  = AHT21.getHumidity();;
     }
     
     // Добавляем данные в историю
     if (!isnan(temp) && !isnan(hum) && AQI != -1) {
         addToHistory(temp, hum, AQI, TVOC, ECO2);
     }
}

// Инициализация истории
void initHistory() {
    for (int i = 0; i < MAX_RECORDS; i++) {
        sensorHistory[i].timestamp = 0;
        sensorHistory[i].temp = NAN;
        sensorHistory[i].hum = NAN;
        sensorHistory[i].aqi = -1;
        sensorHistory[i].tvoc = -1;
        sensorHistory[i].eco2 = -1;
    }
}

// Добавление новых данных в историю
void addToHistory(float temp, float hum, int aqi, int tvoc, int eco2) {
    unsigned long now = millis() / 1000;
    
    if (recordCount > 0) {
        int lastIndex = (currentIndex - 1 + MAX_RECORDS) % MAX_RECORDS;
        if (now - sensorHistory[lastIndex].timestamp < 60) return;
    }
    
    sensorHistory[currentIndex].timestamp = now;
    sensorHistory[currentIndex].temp = (isnan(temp)) ? -32768 : (short)(temp * 10);
    sensorHistory[currentIndex].hum = (isnan(hum)) ? -32768 : (short)(hum * 10);
    sensorHistory[currentIndex].aqi = aqi;
    sensorHistory[currentIndex].tvoc = tvoc;
    sensorHistory[currentIndex].eco2 = eco2;
    
    currentIndex = (currentIndex + 1) % MAX_RECORDS;
    if (recordCount < MAX_RECORDS) recordCount++;
    
    cleanOldData(now);
}

// Очистка данных старше 24 часов
void cleanOldData(unsigned long currentTime) {
    unsigned long twentyFourHoursAgo = currentTime - (24 * 60 * 60);
    int cleaned = 0;
    
    for (int i = 0; i < MAX_RECORDS; i++) {
        if (sensorHistory[i].timestamp != 0 && sensorHistory[i].timestamp < twentyFourHoursAgo) {
            sensorHistory[i].timestamp = 0;
            sensorHistory[i].temp = NAN;
            sensorHistory[i].hum = NAN;
            sensorHistory[i].aqi = -1;
            sensorHistory[i].tvoc = -1;
            sensorHistory[i].eco2 = -1;
            cleaned++;
        }
    }
    
    if (cleaned > 0) {
        // Пересчитываем recordCount
        recordCount = 0;
        for (int i = 0; i < MAX_RECORDS; i++) {
            if (sensorHistory[i].timestamp != 0) {
                recordCount++;
            }
        }
    }
}

// Очистка всей истории
void clearHistory() {
    initHistory();
    recordCount = 0;
    currentIndex = 0;
}

// Отправка данных истории на веб-страницу
void sendHistoryData() {
    String response = "{\"success\":true,\"data\":[";
    
    int recordsSent = 0;
    // Отправляем только последние 120 записей (2 часа) для экономии памяти
    int startIdx = (currentIndex - min(recordCount, 120) + MAX_RECORDS) % MAX_RECORDS;
    
    for (int i = 0; i < min(recordCount, 120); i++) {
        int idx = (startIdx + i) % MAX_RECORDS;
        if (sensorHistory[idx].timestamp != 0) {
            if (recordsSent > 0) response += ",";
            
            char timeStr[6];
            unsigned long timestamp = sensorHistory[idx].timestamp;
            int hour = (timestamp % 86400) / 3600;
            int minute = (timestamp % 3600) / 60;
            sprintf(timeStr, "%02d:%02d", hour, minute);
            
            response += "{\"time\":\"" + String(timeStr) + "\"";
            
            if (!isnan(sensorHistory[idx].temp)) {
                response += ",\"temp\":" + String(sensorHistory[idx].temp, 1);
            } else {
                response += ",\"temp\":null";
            }
            
            if (!isnan(sensorHistory[idx].hum)) {
                response += ",\"hum\":" + String(sensorHistory[idx].hum, 1);
            } else {
                response += ",\"hum\":null";
            }
            
            response += ",\"aqi\":" + String(sensorHistory[idx].aqi);
            response += ",\"tvoc\":" + String(sensorHistory[idx].tvoc);
            response += ",\"eco2\":" + String(sensorHistory[idx].eco2) + "}";
            
            recordsSent++;
        }
    }
    
    response += "],\"latest\":{";
    
    int lastIdx = (currentIndex - 1 + MAX_RECORDS) % MAX_RECORDS;
    if (recordCount > 0 && sensorHistory[lastIdx].timestamp != 0) {
        if (!isnan(sensorHistory[lastIdx].temp)) {
            response += "\"temp\":" + String(sensorHistory[lastIdx].temp, 1) + ",";
        } else {
            response += "\"temp\":null,";
        }
        if (!isnan(sensorHistory[lastIdx].hum)) {
            response += "\"hum\":" + String(sensorHistory[lastIdx].hum, 1) + ",";
        } else {
            response += "\"hum\":null,";
        }
        response += "\"aqi\":" + String(sensorHistory[lastIdx].aqi) + ",";
        response += "\"tvoc\":" + String(sensorHistory[lastIdx].tvoc) + ",";
        response += "\"eco2\":" + String(sensorHistory[lastIdx].eco2);
    } else {
        response += "\"temp\":null,\"hum\":null,\"aqi\":null,\"tvoc\":null,\"eco2\":null";
    }
    
    response += "}}";
    server.send(200, "application/json", response);
}
