#include "DHT.h"
#include <Servo.h>   // 引入 Servo 库

// DHT11 设置
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 传感器引脚
int rainAnalog = A0;   // 雨水模拟输出
int rainDigital = 3;   // 雨水数字输出 (V0)
int waterPin = A1;     // 水位传感器
int ldrPin = A2;       // LDR 光敏电阻

// LED 引脚
int led1 = 11;
int led2 = 12;
int led3 = 13;
int led4 = 10;   // 温度过高时亮起

// 蜂鸣器引脚
int buzzer = 9;

// Servo
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

  myservo.attach(8);   // 舵机信号线接 Arduino D8（你可以改成其他 PWM 引脚）
}

void loop() {
  // 读取温湿度
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // 读取其他传感器
  int rainValue = analogRead(rainAnalog);
  int rainState = digitalRead(rainDigital);
  int waterValue = analogRead(waterPin);
  int lightValue = analogRead(ldrPin);

  // 检查 DHT11 是否正常
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 输出到串口
  Serial.print("Humidity: "); Serial.print(h); Serial.print(" %\t");
  Serial.print("Temperature: "); Serial.print(t); Serial.print(" *C\t");
  Serial.print("Rain(A0): "); Serial.print(rainValue);
  Serial.print(" Rain(V0): "); Serial.print(rainState);
  Serial.print("\tWater: "); Serial.print(waterValue);
  Serial.print("\tLight: "); Serial.println(lightValue);

  // LED 控制示例
  if (lightValue < 300) {
    digitalWrite(led1, HIGH);  // 光照弱 → LED11亮
  } else {
    digitalWrite(led1, LOW);
  }

  if (waterValue > 600) {
    digitalWrite(led2, HIGH);  // 水位高 → LED12亮
  } else {
    digitalWrite(led2, LOW);
  }

  if (rainValue < 500 || rainState == 0) {
    digitalWrite(led3, HIGH);  // 下雨 → LED13亮
  } else {
    digitalWrite(led3, LOW);
  }

  // 温度过高时 LED10 亮起
  if (t > 30) {   // 阈值 30°C
    digitalWrite(led4, HIGH);
  } else {
    digitalWrite(led4, LOW);
  }

  // 蜂鸣器控制：水位过高 或 温度过高
  if (waterValue > 600 || t > 30) {
    digitalWrite(buzzer, HIGH);   // 发声
  } else {
    digitalWrite(buzzer, LOW);    // 静音
  }

  // Servo 控制：平常 90°，下雨时 0°
  if (rainValue < 500 || rainState == 0) {
    myservo.write(0);   // 下雨 → 转到 0°
  } else {
    myservo.write(90);  // 平常保持在 90°
  }

  delay(2000);
}

