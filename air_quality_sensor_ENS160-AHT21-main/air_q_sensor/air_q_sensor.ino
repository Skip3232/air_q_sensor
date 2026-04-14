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
        server.on("/inline", []() {
        server.send(200, "text/plain", "this works without need of authentification");
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
}
