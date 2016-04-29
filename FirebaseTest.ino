#include <Process.h>

// Pin
int pir_pin = 8;

Process date;
int hours, minutes, seconds;
int lastSecond = -1;

void setup()
{
  Bridge.begin();   // Initialize the Bridge

  pinMode(pir_pin, INPUT);

  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%T %D");
    date.run();
  }
}

void loop()
{
  if (digitalRead(pir_pin) == true) {

    if (!date.running())  {
      date.begin("date");
      date.addParameter("+%T %D");
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
