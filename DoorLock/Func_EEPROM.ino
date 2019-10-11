/* esp8266 내 EEPROM Data 를 Read */
void readEEPROM(NetworkSetting *data) {
  Serial.println("\nEEPROM Read request..");

  byte *buf = (byte *)(data);
  EEPROM.begin(sizeof(NetworkSetting));
  for (int i = 0; i < (int)sizeof(NetworkSetting); i++ ) {
    buf[i] = EEPROM.read(i);
    delay(1);
  }
  EEPROM.end();

  Serial.println("Read Data : " + data->netSSID + "," + data->netPW + "," + data->brokerUrl + "," + data->brokerPort);
}

/* 구조체 포인터를 받아서 esp8266 EEPROM 에 기록 */
void writeEEPROM(NetworkSetting *data) {
  Serial.println("\nEEPROM write request : " + data->netSSID);

  byte *buf = (byte *)data;
  EEPROM.begin(sizeof(NetworkSetting));
  for (int i = 0; i < (int)sizeof(NetworkSetting); i++ ) {
    EEPROM.write(i,buf[i]);
    delay(10);
  }
  EEPROM.commit();
  EEPROM.end();

  Serial.println("Write complete.");
}

/* EEPROM 내 사용자 네트워크 설정 존재 여부 검사 */
void chkUserData(void) {
   NetworkSetting data;
   readEEPROM(&data);
   
   // 공장출하시 (code 업로드시) 모든 bit '1' 설정
   if(data.tag == 0xFF) {
    writeEEPROM((NetworkSetting *)&defaultSetting);
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
