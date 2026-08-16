#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(12,13);

const int IN1=2, IN2=3, IN3=4, IN4=7; 
const int ENA=6;  
const int ENB=5; 

const int trigPin=10, echoPin=11;

Servo myServo;
const int servoPin=9;

bool manualMode=false;
char lastCommand='S';
char command;
long duration;

int motorSpeed = 200;  
const int FRONT_THRESHOLD = 20;
const int SIDE_THRESHOLD = 10;

void moveForward(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);
}
void moveBackward(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);
}
void turnLeft(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
  analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);
}
void turnRight(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  analogWrite(ENA,motorSpeed); analogWrite(ENB,motorSpeed);
}
void stopMotors(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  analogWrite(ENA,0); analogWrite(ENB,0);
}

int getDistance(){
  digitalWrite(trigPin,LOW); delayMicroseconds(2);
  digitalWrite(trigPin,HIGH); delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration=pulseIn(echoPin,HIGH,30000);
  if(duration==0) return -1;
  int dist=duration*0.0343/2;
  if(dist<2||dist>400) return -1;
  return dist;
}
int getAverageDistance(){
  int sum=0,count=0;
  for(int i=0;i<3;i++){
    int dist=getDistance();
    if(dist>0){sum+=dist;count++;}
    delay(10);
  }
  if(count==0) return -1;
  return sum/count;
}

void checkBluetooth(){
  if(!BT.available()) return;
  command=BT.read();

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

  if(command=='V'){  
    String speedStr = BT.readStringUntil('\n'); 
    int newSpeed = speedStr.toInt();
    if(newSpeed>=0 && newSpeed<=255){
      motorSpeed = newSpeed;
      BT.print("Speed set to: ");
      BT.println(motorSpeed);
    }
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

void autoAction(void (*action)(),int ms){
  unsigned long t=millis();
  while(millis()-t<ms){
    checkBluetooth();
    if(manualMode){stopMotors();return;}
    action();
    delay(20);
  }
}

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
  Serial.print("Front: "); Serial.println(front);

  if(front==-1){
    stopMotors();
    delay(100);
    return;
  }

  if(front>FRONT_THRESHOLD){
    autoAction(moveForward,100);
  }else{
    stopMotors();
    delay(200);

    autoAction(moveBackward,400);
    stopMotors();

    myServo.write(30);
    delay(700);
    int right=getAverageDistance();
    Serial.print("Right: "); Serial.println(right);

    myServo.write(150);
    delay(700);
    int left=getAverageDistance();
    Serial.print("Left: "); Serial.println(left);

    myServo.write(90);
    delay(300);

    if(left<SIDE_THRESHOLD && right<SIDE_THRESHOLD){
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

