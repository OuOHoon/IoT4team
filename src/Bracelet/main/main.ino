
#include <Arduino_JSON.h>
#include <AWS_IOT.h>
#include <Adafruit_SH1106.h>
#include <WiFi.h>
#include <SPI.h>
#define OLED_MOSI 13 // 9
#define OLED_CLK 14 //10
#define OLED_DC 26 //11
#define OLED_CS 12 //12
#define OLED_RESET 27 //13
Adafruit_SH1106 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH 16 
#define NONE 0
#define DOOR 1
#define FIRE 2
int curState = NONE;
int vibeMil=0;
AWS_IOT testButton;
AWS_IOT testButton2;
//회로관련 변수
const int anaPin = 25;
const int ledPin=16;
const int vibPin=23;
//wifi, aws연결
const char* ssid = "SO070VOIP3EF1";
const char* password = "BEEDCB3EF0";
char HOST_ADDRESS[]="af5ulj9zhi6n3-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "ESP32";
//char CLIENT_ID2[]= "ESP32/2";
char sTOPIC_NAME[]= "deafBell/bracelet"; // subscribe topic name
//char s2TOPIC_NAME[]= "$aws/things/ESP32/door"; // subscribe2 topic name
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0,msgReceived2 = 0;
char payload[512];
char rcvdPayload[512];
//char payload2[512];
//char rcvdPayload2[512];
const int buttonPin = 15; // pushbutton pin
unsigned long preMil = 0; 
const long intMil = 5000;
int ledcheck=0;
bool isVibe=false;
void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
strncpy(rcvdPayload,payLoad,payloadLen);
rcvdPayload[payloadLen] = 0;
msgReceived = 1;
}
//디스플레이 초기 로고
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
B00000001, B11000000,
B00000001, B11000000,
B00000011, B11100000,
B11110011, B11100000,
B11111110, B11111000,
B01111110, B11111111,
B00110011, B10011111,
B00011111, B11111100,
B00001101, B01110000,
B00011011, B10100000,
B00111111, B11100000,
B00111111, B11110000,
B01111100, B11110000,
B01110000, B01110000,
B00000000, B00110000 };
#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif
void setup() {
  bool stat;
  //serial, display begin
  Serial.begin(115200);
  display.begin(SH1106_SWITCHCAPVCC);
  display.display();
  //wifi 연결
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode()); //++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  
  //aws 연결
  if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
  
    if(0==testButton.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
      Serial.println("Subscribe Successfull");
    }
    else {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
  //pinmode 설정
  pinMode(buttonPin, INPUT);
  //pinMode(vibPin,OUTPUT);
  ledcAttachPin(anaPin, 0);
  ledcSetup(0, 5000, 8);
  delay(2000);
}
//진동 함수 선언
void vibeAction(int power, int interval) {       
  if((millis() - vibeMil) > interval) {
    if(isVibe)   ledcWrite(0, 0);
    else         ledcWrite(0, power);
    isVibe=!isVibe;
    vibeMil=millis();
  }
}
void loop() {
//curState값에 따라 진동 다르게 작동
  switch(curState) {
    case NONE:
      vibeAction(0, 0);
    break;
    case DOOR:
      vibeAction(200, 2000);
    break;
    case FIRE:
      vibeAction(255, 500);
    break;
    default: break;
  }
  
  if(msgReceived == 1) {
    msgReceived = 0;
    Serial.print("FIRE message received");
    Serial.println(rcvdPayload);
    JSONVar myObj=JSON.parse(rcvdPayload);
    JSONVar state=myObj["state"];
    String now=(const char*)state;
    Serial.println(now);
    //초인종이 눌리면
    if(now == "DOOR"){
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(15,25);
      display.println("DOOR!!");
      display.display();
      curState=DOOR;
      isVibe=false;
      vibeMil=millis();
    }
    //화재경보기가 감지되면
    else if(now =="FIRE"){
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(15,25);
      display.println("FIRE!!");
      display.display();
      curState=FIRE;
      isVibe=false;
      vibeMil=millis();
    }
    else curState = NONE;
  }
  //택트스위치가 눌리면 진동과 디스플레이 초기화
  if((millis()-preMil) > intMil) {
    // read the state of the pushbutton value
    if (digitalRead(buttonPin)) {
      preMil = millis();
      msgReceived = 0;
      curState=NONE;
      display.clearDisplay();
      display.display();
    }
  }
}
