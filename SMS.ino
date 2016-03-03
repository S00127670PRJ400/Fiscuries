/*
  SendAnSMS

  Demonstrates sending an SMS via a Twilio account using the Temboo Arduino Yun SDK.
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
//#include "TembooAccount.h" // contains Temboo account information

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

#define TEMBOO_ACCOUNT "fiscuries"  // your Temboo account name 
#define TEMBOO_APP_KEY_NAME "Fiscuries"  // your Temboo app key name
#define TEMBOO_APP_KEY  "WJBSw1GTHLIKYiOqBrodBUNjr4SlRpbZ"  // your Temboo app key

// the Account SID from your Twilio account
const String TWILIO_ACCOUNT_SID = "AC9dca8b62db99575d7b79b746f85cf385";

// the Auth Token from your Twilio account
const String TWILIO_AUTH_TOKEN = "f23ccb643dbdf207437e59c1229b5eba";

// your Twilio phone number, e.g., "+1 555-222-1212"
const String TWILIO_NUMBER = "+441792720611";

// the number to which the SMS should be sent, e.g., "+1 555-222-1212"
const String RECIPIENT_NUMBER = "+353873172540";

boolean success = false; // a flag to indicate whether we've sent the SMS yet or not

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);

  Bridge.begin();
}

void loop()
{
  // only try to send the SMS if we haven't already sent it successfully
  if (!success) {

    Serial.println("Running SendAnSMS...");
    
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

    // the first input is a your AccountSID
    SendSMSChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID);
    
    // next is your Auth Token
    SendSMSChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);
 
    // next is your Twilio phone number
    SendSMSChoreo.addInput("From", TWILIO_NUMBER);
    
    // next, what number to send the SMS to
    SendSMSChoreo.addInput("To", RECIPIENT_NUMBER);

    // finally, the text of the message to send
    SendSMSChoreo.addInput("Body", "Hey, there! This is a message from your Arduino Yun!");
    
    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = SendSMSChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! SMS sent!");
        success = true;
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendSMSChoreo.available()) {
        char c = SendSMSChoreo.read();
        Serial.print(c);
      }
    } 
    SendSMSChoreo.close();

    // do nothing for the next 60 seconds
    Serial.println("Waiting...");
    delay(60000);
  }
}
