#include <SPI.h>

//change the following line to #include <Ethernet.h> to use the eithent shield
#include <Ethernet.h> 

// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//thingspeak server
char server[] = "api.thingspeak.com";

//if DHCP fails, use a static IP
IPAddress ip(192,168,0,177);

// Initialize the Ethernet client library
EthernetClient client;

//API key for the Thingspeak ThingHTTP already configured
const String apiKey = "HC0FX8OSAMFTOLCA";

//the number the message should be sent to
const String sendNumber = "+353873172540";

void setup()
{
  Serial.begin(9600);

  //set up Ethernet:
  setupEthernet();
  
  //send the sms
  Serial.println("Sending SMS");
  
  //this function will send the sms
  //the first argument is the number to send to, formatted like this +12345678901
  //the second argument is the body of the text message, which must be within URLEncode()
  sendSMS(sendNumber, URLEncode("Fiscuries Alert! There Has Been Activity At Your Compound. Please Investigate. Data About This Activity Can Be Accessed Online."));

}

void loop()
{

}


void sendSMS(String number,String message)
{
  // Make a TCP connection to remote host
  if (client.connect(server, 80))
  {

    //should look like this...
    //api.thingspeak.com/apps/thinghttp/send_request?api_key={api key}&number={send to number}&message={text body}

    client.print("GET /apps/thinghttp/send_request?api_key=");
    client.print(apiKey);
    client.print("&number=");
    client.print(number);
    client.print("&message=");
    client.print(message);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println(F("Connection failed"));
  } 

  // Check for a response from the server, and route it
  // out the serial port.
  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }      
  }
  Serial.println();
  client.stop();
}



void setupEthernet()
{
  Serial.println("Setting up Ethernet...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg!='\0'){
    if( ('a' <= *msg && *msg <= 'z')
      || ('A' <= *msg && *msg <= 'Z')
      || ('0' <= *msg && *msg <= '9') ) {
      encodedMsg += *msg;
    } 
    else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}

