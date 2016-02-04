#include <Time.h> 
#include <Wire.h> 
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

/////////////////////////////
//VARS
int calibrationTime = 10;        //the time we give the sensor to calibrate itself (10-60 secs according to the datasheet)
long unsigned int lowIn;         //the time when the sensor outputs a low impulse
long unsigned int pause = 5000;  //the amount of milliseconds the sensor has to be low before we assume all motion has stopped

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;

char *dayOfWeek[] = {"", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


/////////////////////////////
//SETUP
void setup(){
 Serial.begin(9600);
 pinMode(pirPin, INPUT);
 pinMode(ledPin, OUTPUT);
 digitalWrite(pirPin, LOW);

 setSyncProvider(RTC.get);   // the function to get the time from the RTC
//  if(timeStatus()!= timeSet)
//     Serial.println("Unable to sync with the RTC");
//  else
//     Serial.println("RTC has set the system time");
     
 //give the sensor some time to calibrate
 Serial.print("Calibrating Sensor");
   for(int i = 0; i < calibrationTime; i++){
     Serial.print(".");
     delay(1000);
     }       
   Serial.println(" SENSOR ACTIVE");
   delay(50);
 }

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(year());
  Serial.print("-");
  if (month() < 10) {
    Serial.print('0');
  }
  Serial.print(month());
  Serial.print("-");
  if (day() < 10) {
    Serial.print('0');
  }
  Serial.print(day());
  Serial.print(" ");
  Serial.print(dayOfWeek[weekday()]);
  Serial.print(' ');
  if (hour() < 10) {
    Serial.print('0');
  }
  Serial.print(hour());
  Serial.print(':');
  if (minute() < 10) {
    Serial.print('0');
  }
  Serial.print(minute());
  Serial.print(':');
  if (second() < 10) {
    Serial.print('0');
  }
  Serial.print(second());
  Serial.println();
}

////////////////////////////
//LOOP
void loop(){
    
    if(digitalRead(pirPin) == HIGH){
      digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
      if(lockLow){  
        lockLow = false;            //make sure we wait for a transition to LOW before any further output is made
        Serial.println("---");
        Serial.print("Motion Detected At: ");
        digitalClockDisplay(); 
        //Serial.print(millis()/1000);
        //Serial.println(" sec"); 
        delay(50);
        }         
        takeLowTime = true;
      }
    
    if(digitalRead(pirPin) == LOW){       
      digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
      
      if(takeLowTime){
       lowIn = millis();          //save the time of the transition from high to LOW
       takeLowTime = false;       //make sure this is only done at the start of a LOW phase
       }
      
      if(!lockLow && millis() - lowIn > pause){  //if the sensor is low for more than the given pause, we assume that no more motion is going to happen
          lockLow = true;                        //makes sure this block of code is only executed again after a new motion sequence has been detected
          Serial.print("Motion Ended At: ");
          digitalClockDisplay(); //output
          //Serial.print((millis() - pause)/1000);
          //Serial.println(" sec");
          delay(50);
          }
      }
 }
