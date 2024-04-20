#include <Arduino.h>
#include <servo.h>

#define EEP 10  //DRV8833 SLEEP pin.  Low by default.  Pull high to enable output and logic.
#define AIN1 11  //DRV8833 AIN1 pin
#define AIN2 12  //DRV8833 AIN2 pin
#define POT1 A1
#define FADER1 A0

#define FADER_MAX_SPEED 20 //max PWM duty cycle (0-255) for fader motor movement
#define UP 1
#define DOWN -1
#define NEUTRAL 512
#define TOGGLE_UP 52
#define TOGGLE_DOWN 53

#define PWM_OUT 9

//REQUIRED PWM PARAMETERS FOR SPARK MAX --- From https://docs.revrobotics.com/sparkmax/specifications#pwm-input-specifications
#define REV_MIN_PULSE_WIDTH     1000
#define REV_MAX_PULSE_WIDTH     2000
#define REV_NEUTRAL_PULSE_WIDTH 1500
#define REV_PWM_DEAD_BAND       75  // value is added to each side of neutral pulse width - within deadband, Spark Max output state is neutral



// put function declarations here:
int myFunction(int, int);
int readPot(int, int);
void fader_goto(int val);
void fader_up(int speed);
void fader_down(int speed);
void fader_stop();
void set_PWM_duty(int duty_cycle);
void set_PWM_10bit(int value);
void set_PWM_Neutral();
Servo frcServo;

void setup() {
  // put your setup code here, to run once:
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(POT1, INPUT);  
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(EEP, OUTPUT);
  pinMode(TOGGLE_UP, INPUT_PULLUP);
  pinMode(TOGGLE_DOWN, INPUT_PULLUP);

  frcServo.attach(PWM_OUT, REV_MIN_PULSE_WIDTH, REV_MAX_PULSE_WIDTH);

}




void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(readPot(POT1, 10));
  Serial.print("\t");
  Serial.print(readPot(FADER1, 10));
  Serial.print("\t");
  Serial.println(map(readPot(FADER1, 10), 0, 1023, REV_MIN_PULSE_WIDTH, REV_MAX_PULSE_WIDTH));

  if (digitalRead(TOGGLE_UP) == LOW) 
  {
    fader_goto(readPot(POT1, 10));
  }
  else if (digitalRead(TOGGLE_DOWN) == LOW){
    fader_stop();
  }
  
  
  else {
    fader_goto(NEUTRAL);
  }
  set_PWM_10bit(readPot(FADER1, 10));

//fader_goto(readPot(POT1, 10));
//fader_follow_pot();
//delay(1);
}


int readPot(int pin, int samples) {
  int accumulator = 0;
  for (int i = 0; i < samples; i++)
  {
    accumulator += analogRead(pin);
  }
  
  return accumulator / samples;

}





void fader_goto(int setpoint){

int fader_now = readPot(FADER1, 10);
int distance = abs(setpoint-fader_now);
int speed = map(distance, 0, 1023, 0, FADER_MAX_SPEED);

if (setpoint-fader_now > 20)
{
  fader_up(speed);
}
else if (setpoint-fader_now < -20)
{
  fader_down(speed);
}
else 
{
  fader_stop();
  set_PWM_Neutral();
}


}
 
void fader_up(int speed){
    digitalWrite(EEP, HIGH);
    analogWrite(AIN1, speed);
    digitalWrite(AIN2, HIGH);
}

void fader_down(int speed){

    digitalWrite(EEP, HIGH);  
    analogWrite(AIN2, speed);
    digitalWrite(AIN1, HIGH);
}

void fader_stop(){
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
}

void set_PWM_duty(int duty_cycle){
  int usec = map(duty_cycle, -100, 100, REV_MIN_PULSE_WIDTH, REV_MAX_PULSE_WIDTH);
  frcServo.writeMicroseconds(usec);
}

void set_PWM_10bit(int value){
  int usec = map(value, 0, 1023, REV_MIN_PULSE_WIDTH, REV_MAX_PULSE_WIDTH);
  frcServo.writeMicroseconds(usec);  
}
void set_PWM_Neutral(){
  frcServo.writeMicroseconds(REV_NEUTRAL_PULSE_WIDTH);
}
