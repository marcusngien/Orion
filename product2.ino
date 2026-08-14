#include "DHT.h"
#include <Servo.h>   


#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


int rainAnalog = A0;   
int rainDigital = 3;   
int waterPin = A1;     
int ldrPin = A2;      


int led1 = 11;
int led2 = 12;
int led3 = 13;
int led4 = 10;   


int buzzer = 9;


Servo myservo;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(rainDigital, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(buzzer, OUTPUT);

  myservo.attach(8);   
}

void loop() {
 
  float h = dht.readHumidity();
  float t = dht.readTemperature();

 
  int rainValue = analogRead(rainAnalog);
  int rainState = digitalRead(rainDigital);
  int waterValue = analogRead(waterPin);
  int lightValue = analogRead(ldrPin);

 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

 
  Serial.print("Humidity: "); Serial.print(h); Serial.print(" %\t");
  Serial.print("Temperature: "); Serial.print(t); Serial.print(" *C\t");
  Serial.print("Rain(A0): "); Serial.print(rainValue);
  Serial.print(" Rain(V0): "); Serial.print(rainState);
  Serial.print("\tWater: "); Serial.print(waterValue);
  Serial.print("\tLight: "); Serial.println(lightValue);

  
  if (lightValue < 300) {
    digitalWrite(led1, HIGH);  
  } else {
    digitalWrite(led1, LOW);
  }

  if (waterValue > 600) {
    digitalWrite(led2, HIGH);  
  } else {
    digitalWrite(led2, LOW);
  }

  if (rainValue < 500 || rainState == 0) {
    digitalWrite(led3, HIGH);  
  } else {
    digitalWrite(led3, LOW);
  }

  
  if (t > 30) {   
    digitalWrite(led4, HIGH);
  } else {
    digitalWrite(led4, LOW);
  }

 
  if (waterValue > 600 || t > 30) {
    digitalWrite(buzzer, HIGH);   
  } else {
    digitalWrite(buzzer, LOW);    
  }

 
  if (rainValue < 500 || rainState == 0) {
    myservo.write(0);   
  } else {
    myservo.write(90); 
  }

  delay(2000);
}

