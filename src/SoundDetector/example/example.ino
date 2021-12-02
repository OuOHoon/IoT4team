
int soundDigitalPin = 12;
int ledPin = 18;
int soundAnalogPin = 14;

void setup(){
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(soundDigitalPin, INPUT);
}

void loop(){
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');
    soundAnalogPin = input.toInt();
  }
  
  int analogInput = analogRead(soundAnalogPin);
  int digitalInput = digitalRead(soundDigitalPin);

  Serial.printf("PinNumber %d: %d\n", soundAnalogPin, analogInput);
  Serial.printf("DigitalInput %d\n", digitalInput);
  
  delay(333);
}
