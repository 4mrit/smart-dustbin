#define BLYNK_TEMPLATE_ID "TMPL6874Jge6t"
#define BLYNK_TEMPLATE_NAME "Smart Dustbin"
#define BLYNK_AUTH_TOKEN "LBMJn604jXa3G0r1GECjAk7o2VmCE5jz"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

unsigned long int lidStartTime; 

//pwm pin  2,4, 12-19, 21-23 , 25-27 , 32,33

#define SERVO_MOTOR_SIGNAL_PIN 4
#define LID_START_POSITION 0
#define LID_ENDING_POSITION 180
#define LID_OPENING_TIME_IN_MS 3000

#define GARBAGE_LEVEL_TRIGGER_PIN 2
#define GARBAGE_LEVEL_ECHO_PIN 5

#define OBJECT_DISTANCE_TRIGGER_PIN 19
#define OBJECT_DISTANCE_ECHO_PIN 21

// #define int MIN_DISTANCE_TO_OPEN_LID 10;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Name";
char pass[] = "amrit12345";

BlynkTimer timer;

void rotateMotor(int,int);
void openLid();
void closeLid();
bool personIsNear();

Servo myservo;
bool isLidOpen;

void setup() {

  Serial.begin(115200);
  myservo.attach(SERVO_MOTOR_SIGNAL_PIN);  // attaches the servo on pin 5 to the servo object
  Blynk.begin(auth, ssid , pass);

  Serial.println("Blynk initited");

  pinMode(GARBAGE_LEVEL_TRIGGER_PIN, OUTPUT);
  pinMode(GARBAGE_LEVEL_ECHO_PIN, INPUT);

  pinMode(OBJECT_DISTANCE_TRIGGER_PIN , OUTPUT);
  pinMode(OBJECT_DISTANCE_ECHO_PIN ,INPUT);

  delay(2000);
  timer.setInterval(10L, SetData);
}

void loop() {

  if(Blynk.connected()){
    Blynk.run();
    timer.run();
  }else{
    Serial.println("Blynk Connection Broken !!");
  }
  
  if(personIsNear()){
    openLid();
  }else{
    closeLid();
  }
}

void openLid(){
   if(isLidOpen == true )
    return;

  if(lidStartTime + LID_OPENING_TIME_IN_MS > millis())
    return;

  Serial.println("Closing Lid !!");
  rotateMotor(LID_ENDING_POSITION,LID_START_POSITION);
  isLidOpen = true;
}

void closeLid(){
  if(isLidOpen == false)
    return;

  Serial.println("Opening Lid !!");
  rotateMotor(LID_START_POSITION,LID_ENDING_POSITION);
  lidStartTime = millis();
  isLidOpen = false;
 
}

void rotateMotor(int startingPosition , int endingPosition){
  Serial.printf("Roatting motor from %d degrees to %d degrees !!\n", startingPosition,endingPosition);
  for (int pos = startingPosition; pos <= endingPosition; startingPosition < endingPosition ? pos++ : pos--) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  myservo.write(endingPosition);
}
int calculateSonarDistance(int triggerPin , int echoPin){

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration *0.034 / 2;

  return distance;
}

int getGarbageLevel(){
  int level = calculateSonarDistance(GARBAGE_LEVEL_TRIGGER_PIN, GARBAGE_LEVEL_ECHO_PIN);
  Serial.printf("Garbage Level :  %d \n", level);
  return level;
}

int getObjectDistance(){
  int distance = calculateSonarDistance(OBJECT_DISTANCE_TRIGGER_PIN , OBJECT_DISTANCE_ECHO_PIN);
  return distance;
}

void SetData(){
  Blynk.virtualWrite(V0, getGarbageLevel());
  Blynk.virtualWrite(V1, getObjectDistance());
}

bool personIsNear(){
  int total = 0;
  for(int i = 0; i < 10; i++){
    total += getObjectDistance();
  } 
  int avg = total / 10;
  Serial.print("Avg Distance : ");
  Serial.println(avg);
  if(avg < 15 )
  {
    return true;
  }
  return false;
}