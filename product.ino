#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(12,13);

// Motor
const int IN1=2, IN2=3, IN3=4, IN4=7; // 方向控制
const int ENA=6;  // PWM ✅
const int ENB=5;  // PWM ✅

// Ultrasonic
const int trigPin=10, echoPin=11;

// Servo
Servo myServo;
const int servoPin=9;

bool manualMode=false;
char lastCommand='S';
char command;
long duration;

// ---------------- Motor ----------------
void moveForward(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  analogWrite(ENA,255); analogWrite(ENB,255);
}
void moveBackward(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  analogWrite(ENA,255); analogWrite(ENB,255);
}
void turnLeft(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  analogWrite(ENA,255); analogWrite(ENB,255);
}
void turnRight(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  analogWrite(ENA,255); analogWrite(ENB,255);
}
void stopMotors(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  analogWrite(ENA,0); analogWrite(ENB,0);
}

// ---------------- Ultrasonic ----------------
int getDistance(){
  digitalWrite(trigPin,LOW); delayMicroseconds(2);
  digitalWrite(trigPin,HIGH); delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration=pulseIn(echoPin,HIGH,30000);
  if(duration==0) return 0;
  int d=duration*0.0343/2;
  if(d<2||d>400) return 0;
  return d;
}
int getAverageDistance(){
  int sum=0,c=0;
  for(int i=0;i<3;i++){
    int d=getDistance();
    if(d>0){sum+=d;c++;}
    delay(10);
  }
  if(c==0) return 0;
  return sum/c;
}

// ---------------- Bluetooth ----------------
void checkBluetooth(){
  if(!BT.available()) return;
  command=BT.read();

  // 忽略换行符和空格
  if(command=='\r' || command=='\n' || command==' ') return;

  if(command=='m'){
    manualMode=true;
    lastCommand='S';
    BT.println("Manual");
    return;
  }
  if(command=='a'){
    manualMode=false;
    lastCommand='S';
    BT.println("Auto");
    return;
  }
  if(manualMode){
    lastCommand=command;
    if(command=='F') BT.println("Forward");
    else if(command=='B') BT.println("Backward");
    else if(command=='L') BT.println("Left");
    else if(command=='R') BT.println("Right");
    else if(command=='S') BT.println("Stop");
  }
}

// ---------------- 自动动作 ----------------
void autoAction(void (*action)(),int ms){
  unsigned long t=millis();
  while(millis()-t<ms){
    checkBluetooth();
    if(manualMode){stopMotors();return;}
    action();
    delay(20);
  }
}

// ---------------- Setup ----------------
void setup(){
  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT); pinMode(ENB,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  myServo.attach(servoPin);
  myServo.write(90);

  Serial.begin(9600);
  BT.begin(9600);
}

// ---------------- Loop ----------------
void loop(){
  checkBluetooth();

  if(manualMode){
    switch(lastCommand){
      case 'F': moveForward(); break;
      case 'B': moveBackward(); break;
      case 'L': turnLeft(); break;
      case 'R': turnRight(); break;
      case 'S': stopMotors(); break;
      default: stopMotors(); break;
    }
    return;
  }

  int front=getAverageDistance();

  if(front==0){
    stopMotors();
    delay(100);
    return;
  }

  if(front>20){
    autoAction(moveForward,100);
  }else{
    stopMotors();
    delay(200);

    autoAction(moveBackward,400);
    stopMotors();

    myServo.write(30);
    delay(700);
    int right=getAverageDistance();

    myServo.write(150);
    delay(700);
    int left=getAverageDistance();

    myServo.write(90);
    delay(300);

    if(left<10 && right<10){
      autoAction(moveBackward,500);
      autoAction(turnLeft,700);
    }else if(left>right){
      autoAction(turnLeft,800);
    }else{
      autoAction(turnRight,800);
    }
    stopMotors();
  }
}


