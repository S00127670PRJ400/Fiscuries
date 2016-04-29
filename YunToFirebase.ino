#include <Bridge.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Mailbox.h>
#include <Process.h>
#include <YunClient.h>
#include <YunServer.h>

const char *FIREBASE = "https://fiscuries.firebaseio.com/";
const char *NODE = ".json";

int calibrationTime = 30;

long unsigned int lowIn;

long unsigned int pause = 5000;

boolean lockLow = true;
boolean takeLowTime;

int pirPin = 3;

Process process;

void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  
  Serial.print("Calibrating Sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" SENSOR ACTIVE!");
    delay(50);
  }

void loop(){
     if(digitalRead(pirPin) == HIGH){
       if(lockLow){
         lockLow = false;            
         Serial.println("---");
         Serial.print("Motion Detected At: ");
         Serial.print(millis()/1000);
         Serial.println(" Sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       if(takeLowTime){
        lowIn = millis();          
        takeLowTime = false;
        }
       if(!lockLow && millis() - lowIn > pause){
           lockLow = true;                        
           Serial.print("Motion Ended At: ");
           Serial.print((millis() - pause)/1000);
           Serial.println(" Sec");
           delay(50);
           }
       }
  }
