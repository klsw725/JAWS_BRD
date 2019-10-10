#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <SPI.h>

#include <EEPROM.h>
#include <ArduinoJson.h>

#include "index.h" // HTML code

#define EEP_DATA_LEN 512
#define TARGET_CMD  0xA0      // 대상 온도 커맨드
#define SENSOR_CMD  0xA1      // 센서 온도 커맨드

#ifndef APSSID
#define APSSID "JAWS_DoorLock_AP"
#define APPSK  "babyshark"
#endif

const int chipSelectPin  = 15;  // NodeMCU D8-CS
int detect = 4;                 // NodeMCU D2
int door = 5;                   // NodeMCU D1
unsigned char T_high_byte;
unsigned char T_low_byte;
int  iTARGET, iSENSOR;  // 부호 2byte 온도 저장 변수

int STA_ENABLE = 0;
bool flag = 0;

/* Network Settings structure */
typedef  struct {
  byte   tag;
  String netSSID;
  String netPW;
  String brokerUrl;
  String brokerPass;
} NetworkSetting;

NetworkSetting userSetting;
const NetworkSetting defaultSetting = {
  0, "Default_setting", "12345678", "JAWS_Broker", "1234"
};

ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

/* EEPROM Function Site */

/* WebBrowser 에서 제출한 form 을 처리하는 함수 */
void formReceive() {
  NetworkSetting data;
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

void setup() {
  Serial.begin(115200);
  pinMode(MISO, INPUT);
  pinMode(chipSelectPin , OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, false);
  pinMode(door, OUTPUT);
  
//  int cnt = 1;
//  chkUserData();
//  readEEPROM(&userSetting);
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(userSetting.netSSID.c_str(), userSetting.netPW.c_str());

 /* Wait for connection for 10 seconds */
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(cnt);

    /* when fail to connect prev.net in 10", enable APmode. */
    if (cnt == 10 ) {
      Serial.println("\nConnection Failed.");
      WiFi.softAP(APSSID);  // Start AP mode with name #def_APSSID
      Serial.println("AP Mode Activated.");

      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP : ");
      Serial.println(myIP);
      server.on("/", HTTP_GET, handleRoot);
      server.on("/", HTTP_POST, formReceive);
      server.begin();
      Serial.println("Please connect to this address.");
      tone(buzzer, 261, 100);
      delay(200);
      tone(buzzer, 261, 300);
      flag = 1;
      break;
    }
    cnt += 1;
  }

  delay(1000);

  Serial.println(flag ? "--AP mode--" : "--STA mode--");
  
  if (flag == 0) {
    // when connected to WIFI
    tone(buzzer, 261, 500);
    Serial.println();
    Serial.println("Connected to " + userSetting.netSSID);
    Serial.println("Router IP : " + WiFi.localIP());

    if (MDNS.begin("shacs")) {
      Serial.println("MDNS responder started");
    }
    
    /* Start TCP (HTTP) server */
    server.on("/setting", HTTP_GET, page_handle);
    server.begin();
    Serial.println("HTTP server started");
  }
}

void loop() {
  server.handleClient();
  if(flag == 0) MDNS.update();
}
