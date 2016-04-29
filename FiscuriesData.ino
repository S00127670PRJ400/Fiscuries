#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h"
#include <Process.h>

int pir_pin = 8;

Process date;
int hours, minutes, seconds;
int lastSecond = -1;

const String GOOGLE_CLIENT_ID = "307718354357-ffc00nigjnmo3bibb18n7o14kap72q13.apps.googleusercontent.com";
const String GOOGLE_CLIENT_SECRET = "HhMaGDKb3Bwi-GxMHJpBUGgf";
const String GOOGLE_REFRESH_TOKEN = "1/UE8jUlR5cU1cDS9J_9Hw1lPqPzMhkIrLjNFJxqAHroA";

const String SPREADSHEET_TITLE = "FiscuriesData";

int numRuns = 1;
int maxRuns = 100;

void setup() {

  Serial.begin(9600);
  delay(4000);

  pinMode(pir_pin, INPUT);

  while (!Serial);
  Serial.println("Time Check");

  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%T %D");
    date.run();
  }

  Serial.print("Initializing the bridge... ");
  Bridge.begin();
  Serial.println("Done!\n");
}

void loop()
{
  if (digitalRead(pir_pin) == true) {

    if (!date.running())  {
      date.begin("date");
      date.addParameter("+%T %D");
      date.run();
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

      if (numRuns <= maxRuns) {

        Serial.println("Running AppendRow - Run #" + String(numRuns++));

        unsigned long now = millis();

        Serial.println("Getting sensor value...");

        Serial.println("Appending value to spreadsheet...");


        TembooChoreo AppendRowChoreo;


        AppendRowChoreo.begin();


        AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
        AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);


        AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");


        AppendRowChoreo.addInput("ClientID", GOOGLE_CLIENT_ID);


        AppendRowChoreo.addInput("ClientSecret", GOOGLE_CLIENT_SECRET);


        AppendRowChoreo.addInput("RefreshToken", GOOGLE_REFRESH_TOKEN);


        AppendRowChoreo.addInput("SpreadsheetTitle", SPREADSHEET_TITLE);



        String rowData = timeString;

        AppendRowChoreo.addInput("RowData", rowData);


        unsigned int returnCode = AppendRowChoreo.run();


        if (returnCode == 0) {
          Serial.println("Success! Appended " + rowData);
          Serial.println("");
        } else {

          while (AppendRowChoreo.available()) {
            char c = AppendRowChoreo.read();
            Serial.print(c);
          }
        }

        AppendRowChoreo.close();
      }

      Serial.println("Waiting...");
      delay(5000);

    }
  }
}
