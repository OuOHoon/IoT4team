#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>

int soundDigitalPin = 12;
int ledPin = 18;
int soundAnalogPin = 14;

int delayTime = 1000;
AWS_IOT testButton;
const char* ssid = "dlink-9A64"; // 와이파이 이름
const char* password = "rvnej29490"; // 와이파이 비밀번호

// 사물 주소
char HOST_ADDRESS[] = "a2s92ul56cnrz8-ats.iot.ap-northeast-2.amazonaws.com";

// 클라이언트 ID
char CLIENT_ID[]= "JHESP32";

// publish 할 문자
char pTOPIC_NAME[]= "esp32/sound";열
int status = WL_IDLE_STATUS;
char payload[512];
unsigned long preMil = 0;
const long intMil = 10000;

void setup() {

  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(soundDigitalPin, INPUT);
  //++choi This is here to force the ESP32 to reset the WiFi and initialize correctly. Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi.."); 
  }
  Serial.println("Connected to wifi");

  if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1); 
  }
  // initialize the pushbutton pin as an input pinMode(buttonPin, INPUT);
  delay(2000);
}

void loop() { 
  int analogInput = analogRead(soundAnalogPin);
  int digitalInput = digitalRead(soundDigitalPin);
  if((millis()-preMil) > intMil) {
    if(digitalInput == 1){
      preMil = millis();
      sprintf(payload,"Sound sensor");
      if(testButton.publish(pTOPIC_NAME,payload) == 0) {
        Serial.print("Publish Message:");
        Serial.println(payload);
      }
      else{
        Serial.println("Publish failed");
      }
    }
  }
}
