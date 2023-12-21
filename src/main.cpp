#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//pwm pin  2,4, 12-19, 21-23 , 25-27 , 32,33
#define BLYNK_TEMPLATE_ID "TMPL6874Jge6t"
#define BLYNK_TEMPLATE_NAME "Smart Dustbin"
#define BLYNK_AUTH_TOKEN "LBMJn604jXa3G0r1GECjAk7o2VmCE5jz"
#define BLYNK_PRINT Serial
#define SERVO_MOTOR_SIGNAL_PIN 4
#define LID_START_POSITION 0
#define LID_ENDING_POSITION 180

#define GARBAGE_LEVEL_TRIGGER_PIN 2
#define GARBAGE_LEVEL_ECHO_PIN 4

#define OBJECT_DISTANCE_TRIGGER_PIN 19
#define OBJECT_DISTANCE_ECHO_PIN 21

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Manzil";
char pass[] = "password123";

BlynkTimer timer;

void rotateMotor(int,int);
void openLid();
void closeLid();

Servo myservo;
bool isLidOpen;

void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_MOTOR_SIGNAL_PIN);  // attaches the servo on pin 5 to the servo object
  Blynk.begin(auth, ssid , pass);

  pinMode(GARBAGE_LEVEL_TRIGGER_PIN, OUTPUT);
  pinMode(GARBAGE_LEVEL_ECHO_PIN, INPUT);

  pinMode(OBJECT_DISTANCE_TRIGGER_PIN , OUTPUT);
  pinMode(OBJECT_DISTANCE_ECHO_PIN ,INPUT);

  delay(2000);
  timer.setInterval(10L, SetData);
}

void loop() {
  openLid();
  delay(5000);
  closeLid();
  delay(5000);
  Blynk.run();
  timer.run();
}

void openLid(){
  if(isLidOpen == true)
    return;

  Serial.println("Opening Lid !!");
  rotateMotor(LID_START_POSITION,LID_ENDING_POSITION);
  isLidOpen = true;
}

void closeLid(){
  if(isLidOpen == false)
    return;

  Serial.println("Closing Lid !!");
  rotateMotor(LID_ENDING_POSITION,LID_START_POSITION);
  isLidOpen = false;
}

void rotateMotor(int startingPosition , int endingPosition){
  Serial.printf("Roatting motor from %d degrees to %d degrees !!\n", startingPosition,endingPosition);
  for (int pos = startingPosition; pos <= endingPosition; startingPosition < endingPosition ? pos++ : pos--) { // goes from 0 degrees to 180 degrees
    Serial.println("Pos: "+ pos);
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
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
  Serial.printf("Object Distance :  %d \n", distance);
  return distance;
}

void SetData(){
  Blynk.virtualWrite(V0, getGarbageLevel());
  Blynk.virtualWrite(V1, getObjectDistance());
}