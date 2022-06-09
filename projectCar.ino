#include <ECE3.h>


const int sensorPin7 = 65; // left 
const int sensorPin6 = 48;
const int sensorPin5 = 64;
const int sensorPin4 = 47;
const int sensorPin3 = 52;
const int sensorPin2 = 68;
const int sensorPin1 = 53;
const int sensorPin0 = 69; // right

const int evenCtrPin = 61;
const int oddCtrPin = 45;
//
//
uint16_t sensorValues[8]; // right -> left, 0 -> 7

float sensorWeight[8] = {-1.0, -0.7, -0.35, -0.1, 0.1, 0.35, 0.7, 1.0};


const int left_nslp_pin = 31; // nslp ==> awake & ready for PWM
const int left_dir_pin = 29;
const int left_pwm_pin = 40;

const int right_nslp_pin = 11; // nslp ==> awake & ready for PWM
const int right_dir_pin = 30;
const int right_pwm_pin = 39;


const float kP = 50.0;
const float kD = 320.0; 
const int baseSpeed = 80;  
 float error = 0;
 float oldErr = 0;
int line;
double pastReading;
int pastBlackCounts;


const int LED_RF = 41;


void setup() {
   ECE3_Init();
//  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(2000);

 pinMode(sensorPin0,INPUT);
 pinMode(sensorPin1,INPUT);
 pinMode(sensorPin2,INPUT);
 pinMode(sensorPin3,INPUT);
 pinMode(sensorPin4,INPUT);
 pinMode(sensorPin5,INPUT);
 pinMode(sensorPin6,INPUT);
 pinMode(sensorPin7,INPUT);

 
 
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);
  // right 
  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);


  oldErr = 0;
  line = 0;
  pastBlackCounts = 0;
  
}

void loop() {

   ECE3_read_IR(sensorValues);
   float sensorErr = 0.0;
   float fSensorValues[8];
   double totalValues = 0.0;
   int blackCounts = 0;  // count numbers of sensors that see blackline.
   int averageEncoder = (getEncoderCount_left()+getEncoderCount_right())/2;
 
   
   
  
    for (unsigned char i = 0; i < 8; i++){
      if(sensorValues[i] > 2400)
      {
        blackCounts++;
      }
       totalValues += sensorValues[i];
       fSensorValues[i] = sensorValues[i];
       sensorErr += ((fSensorValues[i]/100) * sensorWeight[i]);
    }
    error = sensorErr/10;
    float speedCorrection = kP * error + kD * (error - oldErr);  // maybe oldErr - error
   

 
  if (blackCounts >= 4 && pastBlackCounts < 4)
  {
    line++;
  
    switch(line)
    {
      case 1:         // doughnut turn
      digitalWrite(left_dir_pin, HIGH);
      analogWrite(right_pwm_pin, 150);  // 150
      analogWrite(left_pwm_pin, 150);
      delay(330);  // 700 
      digitalWrite(left_dir_pin, LOW);
      break;
   
    case 2:
     {
      
      analogWrite(right_pwm_pin,0);
      analogWrite(left_pwm_pin, 0);
      delay(50);
      break;
     }
     default:
     break;
     
    }
    
  }

  else if(line != 2){

    if(  ( (averageEncoder > 100) && (averageEncoder < 500) ) || ( (averageEncoder > 1450) && (averageEncoder < 5800) )){     // encoder controls: Speed up on straight line
    
    analogWrite(right_pwm_pin, 180 + speedCorrection); // 140
    analogWrite(left_pwm_pin, 180 - speedCorrection);
    }
    else if((averageEncoder > 7300) && (averageEncoder < 11500) ){
     analogWrite(right_pwm_pin, 180 + speedCorrection); // 180
    analogWrite(left_pwm_pin, 180 - speedCorrection);
      }
    else if(averageEncoder > 12620){   // ending
     analogWrite(right_pwm_pin, 180 + speedCorrection); // 180
    analogWrite(left_pwm_pin, 180 - speedCorrection);
      }
    else{
      analogWrite(right_pwm_pin, baseSpeed + speedCorrection);
      analogWrite(left_pwm_pin, baseSpeed - speedCorrection);
      }
 
    }

     
    oldErr = error;
    pastReading = totalValues;
    pastBlackCounts = blackCounts;   

 

  } // end of loop
