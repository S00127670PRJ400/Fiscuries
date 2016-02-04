#include <SPI.h>
#include <Ethernet.h>

// Ethernet shield MAC address (sticker in the back)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Azure Mobile Service address
// You can find this in your service dashboard
const char *server = "fiscuries.azure-mobile.net";

// Azure Mobile Service table name
// The name of the table you created
const char *table_name = "fiscuriesdata";

// Azure Mobile Service Application Key
// You can find this key in the 'Manage Keys' menu on the dashboard
const char *ams_key = "qkbJJkiLDtQrPjiNjLtLQbeiMxddih26";

EthernetClient client;

char buffer[64];

/*
** Send an HTTP POST request to the Azure Mobile Service data API
*/

void send_request(int value)
{
  Serial.println("\nconnecting...");

  if (client.connect(server, 80)) {

    Serial.print("sending ");
    Serial.println(value);

    // POST URI
    sprintf(buffer, "POST /tables/%s HTTP/1.1", table_name);
    client.println(buffer);

    // Host header
    sprintf(buffer, "Host: %s", server);
    client.println(buffer);

    // Azure Mobile Services application key
    sprintf(buffer, "X-ZUMO-APPLICATION: %s", ams_key);
    client.println(buffer);

    // JSON content type
    client.println("Content-Type: application/json");

    // POST body
    sprintf(buffer, "{\"value\": %d}", value);

    // Content length
    client.print("Content-Length: ");
    client.println(strlen(buffer));

    // End of headers
    client.println();

    // Request body
    client.println(buffer);
    
  } else {
    Serial.println("connection failed");
  }
}

/*
** Wait for response
*/

void wait_response()
{
  while (!client.available()) {
    if (!client.connected()) {
      return;
    }
  }
}

/*
** Read the response and dump to serial
*/

void read_response()
{
  bool print = true;
  
  while (client.available()) {
    char c = client.read();
    // Print only until the first carriage return
    if (c == '\n')
      print = false;
    if (print)
      Serial.print(c);
  }
}

/*
** Close the connection
*/

void end_request()
{
  client.stop();
}

/*
** Arduino Setup
*/

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.println("ethernet");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("ethernet failed");
    for (;;) ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

/*
** Arduino Loop
*/

void loop()
{
  int val = analogRead(A0);
  
  send_request(val);
  wait_response();
  read_response();
  end_request();

  delay(1000);
}

