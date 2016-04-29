// Sketch to upload pictures to Dropbox when motion is detected
#include <Bridge.h>
#include <Process.h>
#include <Temboo.h>

// Picture process
Process picture;
Process date;
int hours, minutes, seconds;
int lastSecond = -1;

// Filename
String filename;

// Pin
int pir_pin = 8;

// Path
String path = "/mnt/sda1/";

void setup() {

// Serial.begin(9600);
//
//  // for debugging, wait until a serial console is connected
//  delay(4000);
//  while(!Serial);
  
  // Bridge
  Bridge.begin();
  
  // Set pin mode
  pinMode(pir_pin,INPUT);

   if (!date.running())  {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
  }
}

#define TEMBOO_ACCOUNT "fiscuries"  // your Temboo account name 
#define TEMBOO_APP_KEY_NAME "Fiscuries"  // your Temboo app key name
#define TEMBOO_APP_KEY  "BPdtG1FSg56vObPxJVnrCfGTLqJLEeHD"  // your Temboo app key

boolean success = false; // a flag to indicate whether we've sent the SMS yet or not

void loop(void) 
{
  if (digitalRead(pir_pin) == true) {
    
    // Generate filename with timestamp
    filename = "";
    picture.runShellCommand("date +%s");
    while(picture.running());

    while (picture.available()>0) {
      char c = picture.read();
      filename += c;
    } 
    filename.trim();
    filename += ".png";
 
    // Take picture
    picture.runShellCommand("fswebcam " + path + filename + " -r 1280x720");
    while(picture.running());
    
    // Upload to Dropbox
    picture.runShellCommand("python " + path + "upload_picture.py " + path + filename);
    while(picture.running());

    // only try to send the SMS if we haven't already sent it successfully
  if (!success) {

//    Serial.println("Running SendAnSMS...");
    
    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo SendSMSChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    SendSMSChoreo.begin();
    
    // set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twilio > SMSMessages > SendSMS)
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twilio/SMSMessages/SendSMS/ 
    // for complete details about the inputs for this Choreo

    SendSMSChoreo.setProfile("FiscuriesAlert");
    
    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = SendSMSChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
//        Serial.println("Success! SMS sent!");
        success = true;
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendSMSChoreo.available()) {
        char c = SendSMSChoreo.read();
//        Serial.print(c);
      }
    } 
    SendSMSChoreo.close();

    // do nothing for the next 60 seconds
//    Serial.println("Waiting...");
//    delay(60000);
  }

   if (!date.running())  {
      date.begin("date");
      date.addParameter("+%T");
      date.run();
    }

  }

  while (date.available() > 0) {
    String timeString = date.readString();

    int firstColon = timeString.indexOf(":");
    int secondColon = timeString.lastIndexOf(":");

    String hourString = timeString.substring(0, firstColon);
    String minString = timeString.substring(firstColon + 1, secondColon);
    String secString = timeString.substring(secondColon + 1);

    hours = hourString.toInt();
    minutes = minString.toInt();
    lastSecond = seconds;
    seconds = secString.toInt();

    Process p;
    p.runShellCommand("curl -k -X POST https://fiscuries.firebaseio.com/Activity.json -d '{ \"Motion Detected At\" : " + String(hours) + String(minutes) + String(seconds) + "}'");

    while (p.running());
    delay(2000);
  }
}
