#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <EEPROM.h>
#include <ArduinoJson.h>

#include "index.h"

#define EEP_DATA_LEN 512

#ifndef APSSID
#define APSSID "SHACS"
#define APPSK  "babyshark"
#endif

int STA_ENABLE = 0;
bool flag = 0;

/* Set these to your desired credentials. */
//const char *SSID = APSSID;
//const char *PASSWORD = APPSK;

/* Network Settings structure */
typedef  struct {
  byte   tag;
  String brokerUrl;
  String brokerPass;
  String netSSID;
  String netPW;
} UserSetting;

UserSetting userSetting;
const UserSetting defaultSetting = {
  0,"ddotmotion.kr", "1234", "SSID", "12345678"
};

ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

/* esp8266 내 EEPROM Data 를 Read */
void readEEPROM(UserSetting *data) {
  Serial.println();
  Serial.println("EEPROM Read request..");

  byte *buf = (byte *)(data);
  EEPROM.begin(sizeof(UserSetting));
  for (int i = 0; i < (int)sizeof(UserSetting); i++ ) {
    buf[i] = EEPROM.read(i);
    delay(1);
  }
  EEPROM.end();

  Serial.println("Read Data : " + data->netSSID);
}

/* 구조체 포인터를 받아서 esp8266 EEPROM 에 기록 */
void writeEEPROM(UserSetting *data) {
  Serial.println();
  Serial.println("EEPROM write request : " + data->netSSID);

  byte *buf = (byte *)data;
  EEPROM.begin(sizeof(UserSetting));
  for (int i = 0; i < (int)sizeof(UserSetting); i++ ) {
    EEPROM.write(i,buf[i]);
    delay(10);
  }
  EEPROM.commit();
  EEPROM.end();

  Serial.println("Write complete.");
}

/* EEPROM 내 사용자 네트워크 설정 존재 여부 검사 */
void chkUserData(void) {
   UserSetting data;
   readEEPROM(&data);
   
   if(data.tag == 0xFF) {
    writeEEPROM((UserSetting *)&defaultSetting);
    Serial.println("Set Default.");
   }
}

/* esp8266 내 EEPROM Data 삭제 */
void clearEEPROM(){
  for (int i = 0; i < EEP_DATA_LEN; ++i) {
    EEPROM.write(i, 0); // clear ROM
  }
  EEPROM.commit();

  Serial.println("EEPROM Reset.\n");
  ESP.restart();
}

/* WebBrowser 에서 제출한 form 을 처리하는 함수 */
void formReceive() {
  UserSetting data;
  StaticJsonDocument<200> doc;
  data.netSSID = server.arg("ssid");
  data.netPW = server.arg("pw");

  writeEEPROM(&data);
  readEEPROM(&data);
  Serial.print(data.netSSID);
  Serial.print(",");
  Serial.println(data.netPW);

  server.send(200, "Configuration.html", Config_Page);
  ESP.restart();
}

/* index.h 내 HTML 형식으로 작성된 Main Page 를 Load */
void handleRoot() {
  String s = Config_Page;
  server.send(200, "Configuration.html", s);
}

/* Page Handler */
void page_handle(){
  String s = Refresh_Page;
  server.send(200, "submit.html", s);
}

/* EEPROM read by JSON */
//void readROM() {
//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, readEEPROM().c_str());
//
//  server.sendHeader("Location", "/");
//  server.send(302, "text/plain", "Updated-- Press Back Button");
//}

void setup() {
  Serial.begin(115200);
  
  int cnt = 1;
  chkUserData();
  readEEPROM(&userSetting);
  WiFi.mode(WIFI_STA);
  WiFi.begin(userSetting.netSSID.c_str(), userSetting.netPW.c_str());
 // Serial.print("connecting to ");
 // Serial.print(userSetting.netSSID);
 // Serial.println(userSetting.netPW);

 /* Wait for connection for 10 seconds */
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(cnt);

    /* when fail to connect prev.net in 10", enable APmode. */
    if (cnt == 10 ) {
      Serial.println("\nConnection Failed.");
      WiFi.softAP(APSSID);
      Serial.println("AP Mode Activated.");

      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP : ");
      Serial.println(myIP);
      server.on("/", HTTP_GET, handleRoot);
      server.on("/", HTTP_POST, formReceive);
      server.begin();
      Serial.println("Please connect to this address.");
      flag = 1;
      break;
    }
    cnt += 1;
  }

  delay(1000);

  Serial.println(flag ? "--AP mode--" : "--STA mode--");
  
  if (flag == 0) {
    // when connected to WIFI
    Serial.println();
    Serial.println("Connected to " + userSetting.netSSID);
    Serial.println("Router IP : " + WiFi.localIP());

    if (MDNS.begin("shacs")) {
      Serial.println("MDNS responder started");
    }
    
    /* Start TCP (HTTP) server */
    server.on("/setting", HTTP_GET, page_handle);
//    server.on("/", HTTP_GET, handleRoot);
//    server.on("/", HTTP_POST, formReceive);
    server.begin();
    Serial.println("HTTP server started");
  }
}

void loop() {
  server.handleClient();
  if(flag == 0) MDNS.update();
}
