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
int LED = 4;                    // NodeMCU D2
int door = 5;                   // NodeMCU D1
unsigned char T_high_byte;
unsigned char T_low_byte;
int  iTARGET, iSENSOR;  // 부호 2byte 온도 저장 변수

const char* mqtt_server = "172.20.10.11";
const int mqtt_port = 1883;

int connection_try = 10;        // connection try
int STA_ENABLE = 0;
bool flag = 0;

/* Network Settings structure */
typedef  struct {
  byte   tag;
  String netSSID;
  String netPW;
  String brokerUrl;
  int brokerPort;
} NetworkSetting;

NetworkSetting userSetting;     // Hold User Setting
const NetworkSetting defaultSetting = {
  0, "default", "pass", "broker", 1883
};

ESP8266WebServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);
const char* topic_sub = "JAWS";   // MQTT Topic sub
long lastMsg = 0;
char msg[50];
int value = 0;

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
  data.brokerUrl = server.arg("broker");
  data.brokerPort = server.arg("port").toInt();
  
  writeEEPROM(&data);
  readEEPROM(&data);
  Serial.println("Form Received");

  server.send(200, "Configuration.html", Config_Page);
  flag = 0; // connection retry
  connection_try = 30;
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String str1;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    str1.concat(String((char)payload[i]));
  }
  Serial.println();
  
  if(str1.equals("open")){
    Serial.println("success");
    digitalWrite(door, HIGH);
    delay(500);
    digitalWrite(door, LOW);
  }
}

/* Wait for connection for 10 seconds */
//void connect_n() {
//  
//}

void connect_b() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...\n");
    // Create a random client ID
    String clientId = "JAWS_Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
//      client.publish("outTopic", "hello world");
//      // ... and resubscribe
//      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int SEND_COMMAND(unsigned char cCMD)
{
    digitalWrite(chipSelectPin , LOW);  // CS Low Level
    delayMicroseconds(10);              // delay(10us)
    SPI.transfer(cCMD);                // Send 1st Byte
    delay(10);                          // delay(10ms)          
    T_low_byte = SPI.transfer(0x22);   // Send 2nd Byte
    delay(10);                          //delay(10ms)  
    T_high_byte = SPI.transfer(0x22);  // Send 3rd Byte
    digitalWrite(chipSelectPin , HIGH); // CS High Level 
    
    return (T_high_byte<<8 | T_low_byte);  // 상위, 하위 바이트 연산 
}

void sensing() {
  while(true) {
    iTARGET = float(SEND_COMMAND(TARGET_CMD))/100;      // 대상 온도 Read 
    delay(50);    // 50ms : 이 라인을 지우지 마세요 

    if(iTARGET >= 30)
      digitalWrite(LED, HIGH);
    else
      digitalWrite(LED, LOW);
  }
}

/********** Main Code **********/
void setup() {
  Serial.begin(115200);
  pinMode(MISO, INPUT);
  pinMode(chipSelectPin , OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, false);
  pinMode(door, OUTPUT);
  
  readEEPROM(&userSetting);
  // 공장출하시 (code 업로드시) 모든 bit '1' 설정
    if(userSetting.tag == 0xFF) {
    writeEEPROM((NetworkSetting *)&defaultSetting);
    Serial.println("Set Default.");
  }

  //  while(true) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(userSetting.netSSID.c_str(), userSetting.netPW.c_str());

    for (int cnt = 1; cnt <= connection_try ;cnt++) {
      delay(1000);
      Serial.print(cnt);

      if (WiFi.status() == WL_CONNECTED)
        break;
      
      /* when fail to connect prev.net in 10", enable APmode. */
      if (cnt == connection_try ) {
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
        flag = 1;   // connection error
        break;
      }
    }
  
    delay(1000);
  
    Serial.println(flag ? "--AP mode--" : "--STA mode--");  // connection check
    
    if (flag == 0) {
//      client.setServer(userSetting.brokerUrl.c_str(), userSetting.brokerPort);
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callback);
      connect_b();
      client.subscribe(topic_sub);
    
      /* Setting CS & SPI */
      digitalWrite(chipSelectPin , HIGH);    // CS High Level
      SPI.setDataMode(SPI_MODE3);            // Setting SPI Mode 
      SPI.setClockDivider(SPI_CLOCK_DIV16);  // 16MHz/16 = 1MHz
      SPI.setBitOrder(MSBFIRST);             // MSB First
      SPI.begin();                           // Initialize SPI
      delay(500);                             // wating for DTS setup time
      sensing();
    }
//    else {
//      while(flag) {
//        Serial.print(".");
//        delay(1000);
//      }
//    }
//  }
}

void loop() {
  server.handleClient();
  if(flag == 0) MDNS.update();
/*
  iTARGET = float(SEND_COMMAND(TARGET_CMD))/100;      // 대상 온도 Read 
    delay(50);    // 50ms : 이 라인을 지우지 마세요 

  if(iTARGET >= 30)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
*/
}
