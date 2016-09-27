

/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include "MFRC522.h"

#define RST_PIN 5 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2 
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#ifdef __AVR__
  #include <avr/power.h>
#endif


const char* ssid     = "ibcroutervpn2_5GHz";
const char* password = "S3demos-wifi";

const char* host = "api.thingspeak.com";
const char* streamId   = "/channels/1417/field/2/last.json";
const char* textStreamId   = "/channels/1417/field/1/last.json";

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#define LIB_DOMAIN "arduino-tweet.appspot.com"

const char* token="4059494159-aI1CkCcDOsAPTek1yAt4c5A3hcyd5UrffHapXaJ";
char msg[128] = "";

////////////////////// NTP STUFF ////////////////////////////

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

int currentcolors[3] = {-1,-1,-1};
unsigned long timeOfColorChange = 0;
char timeString[20] = "";
char currentColorString[15] = "";

int boredomTime = 0;
#define BOREDMIN 5
#define BOREDMAX 10

#define DELAYLOOP 15000 // 15 seconds

/***************************************** SETUP *******************************/
void setup() 
{
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  pixels.begin(); // This initializes the NeoPixel library.
  
  Serial.println("Starting UDP");
  udp.begin(localPort);

  randomSeed(analogRead(0));
  boredomTime = random(BOREDMIN,BOREDMAX);
  Serial.print("Boredom initially set to " );
  Serial.println(boredomTime);

  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
}




/********************** LOOP ******************************/
unsigned long lastFoundSeconds = 0;
void loop() 
{
  unsigned long now =getCurrentTime(timeString);
 
  int rgb[3] = {0,0,0};                           //define rgb pointer for ws2812
  String rgbStr = askThingSpeak();
  rgbStr.replace("%23","#");
  getRGB(rgbStr,rgb);  

  if ( mfrc522.PICC_IsNewCardPresent()) 
  {
       // Select one of the cards
      if (  mfrc522.PICC_ReadCardSerial()) 
      {
    
        // Show some details of the PICC (that is: the tag/card)
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.println();
      rgb[0] = mfrc522.uid.uidByte[0];
      rgb[1] = mfrc522.uid.uidByte[1];
      rgb[2] = mfrc522.uid.uidByte[2];
      
      }
  }
  
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
   // capture the values and the time....
  if (rgb[0] != currentcolors[0] || rgb[1] != currentcolors[1] || rgb[2] != currentcolors[2])
  {
    Serial.print("Wheeee new colours, bored again in "); Serial.print(boredomTime); Serial.println(" Minutes");
     // we've got a new set of colours.
      currentcolors[0] = rgb[0];
      currentcolors[1] = rgb[1];
      currentcolors[2] = rgb[2];
     timeOfColorChange = now;
  }
  else
  {
    if ((now - timeOfColorChange) > boredomTime * 60)
    {
      boredomTime = random(BOREDMIN,BOREDMAX);
      Serial.print("BORED, setting to new boredom time of ");
      Serial.println(boredomTime);
      postToTwitter();
    }
  }
  delay(DELAYLOOP);
}

String askThingSpeak()
{
  WiFiClient client;
  const int httpPort = 80;

  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return "";
  }
  
  // We now create a URI for the request
  String url = "";
  url += streamId;
  
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  String rgbStr = "";
  bool begin = false;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available() || !begin )
  {
    char in = client.read();
    if (in == '{') 
    {
        begin = true;
    }
    if (begin) rgbStr += (in);
    if (in == '}') 
    {
        break;
    }
   delay(1);
  }
 
  return rgbStr;
}

String askThingSpeakString()
{
  WiFiClient client;
  const int httpPort = 80;

  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return "";
  }
  
  // We now create a URI for the request
  String url = "";
  url += textStreamId;
  
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  String rgbStr = "";
  bool begin = false;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available() || !begin )
  {
    char in = client.read();
    if (in == '{') 
    {
        begin = true;
    }
    if (begin) rgbStr += (in);
    if (in == '}') 
    {
        break;
    }
   delay(1);
  }
 
  return rgbStr;
}

void getRGB(String hexRGB, int *rgb) {
  hexRGB.toUpperCase();
  int s =  hexRGB.indexOf("FIELD2") + 9;
  int end = s + 7;
  String color = hexRGB.substring(s, end);
  char c[7];
  color.toCharArray(c,8);
  rgb[0] = convertToInt(c[1],c[2]); //red
  rgb[1] = convertToInt(c[3],c[4]); //green
  rgb[2] = convertToInt(c[5],c[6]); //blue 
}

String getColorString(String input)
{
  input.toUpperCase();
  int s =  input.indexOf("FIELD1") + 9;
  String color = input.substring(s);
    Serial.println(  color);
  int end = color.indexOf("\"");
  color = color.substring(0, end);
  Serial.print("NOW THE COLOR IS ");
  Serial.println(  color);
   color.toLowerCase();
   return color;
}

int convertToInt(char upper,char lower)
{
  int uVal = (int)upper;
  int lVal = (int)lower;
  uVal = uVal >64 ? uVal - 55 : uVal - 48;
  uVal = uVal << 4;
  lVal = lVal >64 ? lVal - 55 : lVal - 48;
  return uVal + lVal;
}


void postToTwitter()
{
  String currentColorStringRaw = askThingSpeakString();
  String theCurrentColorString = getColorString(currentColorStringRaw);
  
   WiFiClient client;
  const int httpPort = 80;
  
  const String colors[] = {"red", "green", "blue", "cyan", "purple", "yellow", "orange", "pink", "magenta", "white"};
  int rndCol = random(10);
  while (theCurrentColorString.compareTo(colors[rndCol]) == 0)
  {
    Serial.print("already is ");
    Serial.println(colors[rndCol]);
    rndCol = random(10);
  }
  
  memset(msg, 0, 128);
  snprintf(msg, 128, "@cheerlights, I'm a bored ESP8266. It's %s and I want the %s lights. I'll be bored again in %d minutes", timeString, colors[rndCol].c_str(), boredomTime);
  
    
  Serial.print("Trying to twat"); 
  Serial.println(msg);
  
  if (client.connect(LIB_DOMAIN, 80))
  {
    client.println("POST http://" LIB_DOMAIN "/update HTTP/1.0");
		client.print("Content-Length: ");
		client.println(strlen(msg)+strlen(token)+14);
		client.println();
		client.print("token=");
		client.print(token);
		client.print("&status=");
		client.println(msg);
  }
}

unsigned long getCurrentTime(char* timeString)
{
    //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(500);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
    return lastFoundSeconds+(DELAYLOOP/1000);
  }
  else {
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    
    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    
    int h, m, s;
    h = (epoch  % 86400L) / 3600;
    m = (epoch  % 3600) / 60;
    s = (epoch%60);

    sprintf(timeString, "%02d:%02d:%02d", h, m, s);

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.println(timeString); // print the second

    lastFoundSeconds = secsSince1900;
    return secsSince1900;
  }
  return 0;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
