#include "DHT.h"
#include <Servo.h>   

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int rainAnalog = A0;   
int rainDigital = 3;   
int waterPin = A1;     
int ldrPin = A2;      

int ledLight = 11;
int ledWater = 12;
int ledRain = 13;
int ledTemp = 10;   

int buzzer = 9;
Servo myservo;

const int LIGHT_THRESHOLD = 300;
const int WATER_THRESHOLD = 600;
const int TEMP_THRESHOLD = 30;
const int RAIN_THRESHOLD = 500;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(rainDigital, INPUT);
  pinMode(ledLight, OUTPUT);
  pinMode(ledWater, OUTPUT);
  pinMode(ledRain, OUTPUT);
  pinMode(ledTemp, OUTPUT);
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

  Serial.print("Humidity: "); Serial.print(h); Serial.print("% | ");
  Serial.print("Temp: "); Serial.print(t); Serial.print("C | ");
  Serial.print("Rain: "); Serial.print(rainValue); Serial.print("/");
  Serial.print(rainState); Serial.print(" | ");
  Serial.print("Water: "); Serial.print(waterValue); Serial.print(" | ");
  Serial.print("Light: "); Serial.println(lightValue);

  digitalWrite(ledLight, lightValue < LIGHT_THRESHOLD ? HIGH : LOW);
  digitalWrite(ledWater, waterValue > WATER_THRESHOLD ? HIGH : LOW);
  digitalWrite(ledRain, (rainValue < RAIN_THRESHOLD || rainState == 0) ? HIGH : LOW);
  digitalWrite(ledTemp, t > TEMP_THRESHOLD ? HIGH : LOW);

  if (waterValue > WATER_THRESHOLD && t > TEMP_THRESHOLD) {
    tone(buzzer, 1000, 500); 
  } else if (waterValue > WATER_THRESHOLD) {
    tone(buzzer, 600, 300);  
  } else if (t > TEMP_THRESHOLD) {
    tone(buzzer, 400, 300);  
  } else {
    noTone(buzzer);
  }

  if (rainValue < RAIN_THRESHOLD || rainState == 0) {
    myservo.write(0);   
  } else {
    myservo.write(90); 
  }

  delay(2000);
}


