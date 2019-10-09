int sig = 14;
int door = 5;

void setup() {
  Serial.begin(115200);
  pinMode(sig, INPUT);
  pinMode(door, OUTPUT);

  digitalWrite(door, true);
}

void loop() {
  if(digitalRead(sig)){
    Serial.println("success");
    digitalWrite(door, false);
    delay(50);
    digitalWrite(door, true);
  }
}
