#include <Arduino.h>
#include <SoftwareSerial.h>    // подключаем библиотеку
SoftwareSerial mySerial(2,3);  // указываем пины tx и rx

int ledPin = 13;
byte incomingByte;

void setup() {
  pinMode(2,INPUT);
  pinMode(3,OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);         // включаем hardware-порт
  mySerial.begin(9600);
  // Serial.print("start");
  // Serial.println(incomingByte, DEC);
}

void loop() {
  if (mySerial.available() > 0) {
    incomingByte = mySerial.read();

    if(incomingByte == '1'){
      digitalWrite(ledPin, HIGH);
    }
    else if (incomingByte == '0'){
      digitalWrite(ledPin, LOW);
    }

    mySerial.print("I received: ");
    mySerial.println(incomingByte);
    // Serial.print("I received: ");
    // Serial.println(incomingByte);
  }
  delay(10);
}
