#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SPI.h>

#define TARGET_CMD	0xA0			// 대상 온도 커맨드
#define SENSOR_CMD	0xA1			// 센서 온도 커맨드

const int chipSelectPin  = 15;  // NodeMCU D8-CS
int LED = 4;                    // NodeMCU D2
int door = 5;                   // NodeMCU D1
unsigned char T_high_byte;
unsigned char T_low_byte;
int  iTARGET, iSENSOR;	// 부호 2byte 온도 저장 변수

const char* ssid = "Dmotion";
const char* password = "11270726";
const char* mqtt_server = "ddotmotion.kr";
const int mqtt_port = 9883;
const char* topic_sub = "jaws";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...\n");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
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

void setup() {
  /* Initalize PORT */
  Serial.begin(115200);
  pinMode(MISO, INPUT);
  pinMode(chipSelectPin , OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, false);
  pinMode(door, OUTPUT);
  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  client.subscribe(topic_sub);
  
  /* Setting CS & SPI */
  digitalWrite(chipSelectPin , HIGH);    // CS High Level
  SPI.setDataMode(SPI_MODE3);            // Setting SPI Mode 
  SPI.setClockDivider(SPI_CLOCK_DIV16);  // 16MHz/16 = 1MHz
  SPI.setBitOrder(MSBFIRST);             // MSB First
  SPI.begin();                           // Initialize SPI
  delay(500);                             // wating for DTS setup time 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
//  while(1) {
    iTARGET = float(SEND_COMMAND(TARGET_CMD))/100;			// 대상 온도 Read 
    delay(50);    // 50ms : 이 라인을 지우지 마세요 

//    iSENSOR = float(SEND_COMMAND(SENSOR_CMD))/100;			// 센서 온도 Read
//    delay(50);    // 500ms : 이 라인을 지우지 마세요.

    if(iTARGET >= 30)
      digitalWrite(LED, HIGH);
    else
      digitalWrite(LED, LOW);

//  }
}
