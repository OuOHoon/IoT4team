#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>

#define TEST 1 // wifi, aws 1이면 테스트 0이면 배포설정

// 회로 관련 변수
int soundDigitalPin = 12;
int ledPin = 18;
int soundAnalogPin = 14;
int buttonPin = 15;

// 시간 관련 변수
int readTickStart = 0, readTickEnd = 0;
int currentMils = 0, startMils = 0;

#define IS_DETECTED 2

// WIFI 관련 변수
int delayTime = 1000;
AWS_IOT testButton;
const char* ssid = (TEST == 0 ? "SO070VOIP3EF1" : "dlink-9A64");
const char* password = (TEST == 0 ? "BEEDCB3EF0" : "rvnej29490");

// AWS 관련 변수
char* HOST_ADDRESS=(TEST == 0 ? "af5ulj9zhi6n3-ats.iot.ap-northeast-2.amazonaws.com" : "a2s92ul56cnrz8-ats.iot.ap-northeast-2.amazonaws.com");
int detectCount = 0; // 1 감지 횟수
int tempCount = 0; // n초동안 연속 감지 횟수

// 클라이언트 ID
char CLIENT_ID[]= "SOUND_ESP32";

// publish 할 문자
char pTOPIC_NAME[]= "deafBell/bell";
int status = WL_IDLE_STATUS;
char payload[512];
unsigned long preMil = 0;
const long intMil = 10000;

// 회로 관련 초기화
void initPin(){
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(soundDigitalPin, INPUT);
}

// 와이파이 연결
void connectWiFi(){
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
}

// AWS 연결
void connectAWS(){
  if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1); 
  }
  delay(2000);
}


// 버튼 눌렀는지 체크
int pastButtonState = LOW, currentButtonState = LOW;
bool checkButtonState(){
  currentButtonState = digitalRead(buttonPin);
  if(currentButtonState == HIGH && pastButtonState == LOW){
    pastButtonState = HIGH;
    return true;
  }
  else{
    pastButtonState = currentButtonState;
  }
  return false;
}

void publishButton(){
  sprintf(payload,"{\"state\":\"DOOR\"}");
  if(testButton.publish(pTOPIC_NAME,payload) == 0) {
    Serial.print("Publish Message:");
    Serial.println(payload);
  }
  else{
    Serial.println("Publish failed");
  }
  delay(1000);
}

void publishSound(){
  sprintf(payload,"{\"state\":\"FIRE\"}");
    if(testButton.publish(pTOPIC_NAME,payload) == 0) {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else{
      Serial.println("Publish failed");
    }
}


void setup() {
  initPin();
  connectWiFi();
  connectAWS();
  
  startMils  = millis(); // 프로그램 시작 시간
  readTickStart = millis();
}

bool isValidSound(){
  currentMils = millis();
  readTickEnd = millis();
  if(readTickEnd - readTickStart >= 33){
    readTickStart = readTickEnd;
    int analogInput = analogRead(soundAnalogPin);
    int digitalInput = digitalRead(soundDigitalPin);
    if(digitalInput == 1)
      detectCount++;
  }
  if(currentMils - startMils >= 1000){
    if(detectCount > IS_DETECTED){ // 1초에 N번 감지됐을때
      Serial.printf("%d count\n", detectCount);
      tempCount++;
    }
    else{
      tempCount = 0;
    }
    if(tempCount >= 3){
       Serial.printf("3초 연속 감지\n");
       tempCount = 0;
       return true;
    }
    detectCount = 0;
    startMils = currentMils;
  }
  return false;
}

void loop() { 
  if(checkButtonState()){
    publishButton();
  }
  if(isValidSound()){
    publishSound();
  }
}
