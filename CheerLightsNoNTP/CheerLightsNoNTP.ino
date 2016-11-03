

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

#ifdef __AVR__
  #include <avr/power.h>
#endif

const char* ssid     = "ibcroutervpn2_5GHz";
const char* password = "S3demos-wifi";

const char* host = "api.thingspeak.com";
const char* streamId   = "/channels/1417/field/2/last.json";
const char* textStreamId   = "/channels/1417/field/1/last.json";

const char* timehost = "www.epochconverter.com";

// Which pin on the Arduino is connected to the NeoPixels?
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

ADC_MODE(ADC_VCC);

int currentcolors[3] = {-1,-1,-1};
unsigned long timeOfColorChange = 0;
char timeString[20] = "";
char currentColorString[15] = "";
String lastColorString = "black";

int boredomTime = 0;
#define BOREDMIN 5
#define BOREDMAX 15

#define DELAYLOOP (20 * 1000) // in seconds

typedef struct colorBlob{
  char* colorName;
  char rgb[3];
};

colorBlob theColors[]= {
  {"red", {0xFF,0,0}},
  {"green",{00, 0x80, 00}},
  {"blue", {00, 00, 0xFF}},
  {"cyan", {00,0xFF, 0xFF}},
  {"white", {0xFF, 0xFF, 0xFF}},
  {"warmwhite", {0xFD, 0xF5, 0xE6}},
  {"purple", {0x80, 0x00, 0x80}},
  {"magenta", {0xFF, 0x00, 0xFF}},
  {"yellow", {0xFF, 0xFF, 0x00}},
  {"orange", {0xFF, 0xA5, 0x00}},
  {"pink", {0xFF, 0xA0, 0xAB}},
};

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

  randomSeed(askForTime() & 0xFFFF);
  boredomTime = random(BOREDMIN,BOREDMAX);
  Serial.print("Boredom initially set to " );
  Serial.println(boredomTime);
}




/********************** LOOP ******************************/
unsigned long lastFoundSeconds = 0;
void loop() 
{
  Serial.print(">"); Serial.print(ESP.getVcc()/1000.0);Serial.println("<");
  unsigned long now = askForTime();
  int rgb[3] = {currentcolors[0],currentcolors[1],currentcolors[2]};                           //define rgb pointer for ws2812
 
  String colorString = getColorString();
  if(colorString != lastColorString)
  {
    Serial.print("Color is now ");Serial.println(colorString);
    for (int c = 0; c < 11; c++)
    {
      if (String(theColors[c].colorName) == colorString)
      {
        Serial.print("Found color ");Serial.print(theColors[c].colorName);
        for (int d = 0; d < 3; d++)
        {
          rgb[d] = theColors[c].rgb[d];
          Serial.print(" "); Serial.print( rgb[d], HEX);
        }
        Serial.println("");
        break;
      }
    }
  
     // capture the values and the time....
    if (rgb[0] != currentcolors[0] || rgb[1] != currentcolors[1] || rgb[2] != currentcolors[2])
    {
      for(int i=0;i<NUMPIXELS;i++){
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
          pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.
      }
      
      pixels.show(); // This sends the updated pixel color to the hardware.
      // we've got a new set of colours.
      currentcolors[0] = rgb[0];
      currentcolors[1] = rgb[1];
      currentcolors[2] = rgb[2];
      timeOfColorChange = now;
      lastColorString = colorString; 
      Serial.print("Wheeee new colours, bored again in "); Serial.print(boredomTime); Serial.println(" Minutes");
      tellTwitter();
    }
  }
  else
  {
    //Serial.print("Color hasn't changed from "); Serial.println(lastColorString);
    if ((now - timeOfColorChange) > boredomTime * 60)
    {
      int newboredomTime = random(BOREDMIN,BOREDMAX);
      while (newboredomTime == boredomTime)
      {
        newboredomTime = random(BOREDMIN,BOREDMAX);
      }
      boredomTime = newboredomTime;
      Serial.print("BORED, setting to new boredom time of ");
      Serial.println(boredomTime);
      postToTwitter();
    }
  }
  delay(DELAYLOOP);
}


unsigned long askForTime()
{
  WiFiClient client;
  const int httpPort = 80;
  unsigned long epoch;
  bool estimate = false;
  // Use WiFiClient class to create TCP connections
  if (!client.connect(timehost, httpPort)) {
    Serial.println("connection failed, estimating the time now");
    estimate = true;
  }
  else
  {
    Serial.println("OK, found a client connection, going to go and ask for the date");
    String url = "/";
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + timehost + "\r\n" + 
                 "Connection: close\r\n\r\n");
  
    delay(1000);
    String dateStr = "";
    bool begin = false;
    // Read all the lines of the reply from server and print them to Serial
    while(client.available() )
    {
      char in = client.read();
      dateStr += (in);
      if (dateStr.length() >=40)
      {
        if (!dateStr.startsWith("ecclock"))
        {
          dateStr.remove(0,1);
        }
        else
        {
          /* we have found the ecclock bit, so away we go..... */
          break;
        }
      }
     delay(1);
    }
    /* remove the crud before the value */
    int Pos = dateStr.indexOf('">')+1;
    if (Pos == -1)
    {
      estimate = true;
      Serial.println("Didn't receive the right amount of data to strip the end, will estimate");
    }
    else
    {
      dateStr.remove(0, Pos);
      /* and after the value */ 
      Pos = dateStr.indexOf("</div");
      if (Pos == -1)
      {
        estimate=true;
        Serial.println("Didn't find the end of the string");
        
      }
      else
      {
        dateStr.remove(Pos);
        epoch = dateStr.toInt();
      }
    }
  }
  if (estimate)
  {
    epoch =  lastFoundSeconds+(DELAYLOOP/1000); 
  }
  if (epoch < lastFoundSeconds)
  {
    estimate = true;
    epoch =  lastFoundSeconds+(DELAYLOOP/1000);
  }

  int h, m, s;
  h = (epoch  % 86400L) / 3600;
  m = (epoch  % 3600) / 60;
  s = (epoch%60);
  sprintf(timeString, "%02d:%02d", h+1, m);

  // print the hour, minute and second:
  if (estimate) Serial.print("*");
  else Serial.print(" ");
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  Serial.println(timeString); // print the second
  lastFoundSeconds = epoch;
  return epoch;
  
}

String askThingSpeak()
{
  Serial.println("Asking for the current colors");
  WiFiClient client;
  const int httpPort = 80;

  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed asking for the colors.");
  
    return "";
  }
  
  // We now create a URI for the request
  String url = "";
  url += streamId;
  
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  String rgbStr = "";
  bool begin = false;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available())// || !begin )
  {
    char in = client.read();
    if (in == '{') 
    {
      Serial.println("Found start of string, begin");
      begin = true;
    }
    if (begin) rgbStr += (in);
    if (in == '}') 
    {
      Serial.println("Found end of string, break");
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
    Serial.println("connection failed to thingspeak");
    
    return "";
  }
  
  // We now create a URI for the request
  String url = "";
  url += textStreamId;
  
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  String rgbStr = "";
  bool begin = false;
  int charsRead = 0;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available())// || !begin )
  {
    char in = client.read();
    charsRead += 1;
    if (in == '{') 
    {
        begin = true;
        Serial.println("Found start of string, begin");
    }
    if (begin) rgbStr += (in);
    if (in == '}') 
    {
      Serial.println("Found end of string, break");
      break;
    }
    if (charsRead >= 10000)
    {
      Serial.println("Failed to find the end in 10000chars, bailing"); 
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

  //Serial.print("Converting string "); Serial.print(hexRGB); Serial.print(rgb[0], HEX); Serial.print(rgb[1], HEX); Serial.println(rgb[2], HEX);
}

String getColorString()
{
  String input = askThingSpeakString();
  input.toUpperCase();
  int s =  input.indexOf("FIELD1") + 9;
  String color = input.substring(s);
  //  Serial.println(  color);
  int end = color.indexOf("\"");
  color = color.substring(0, end);
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


void tellTwitter()
{
  String theCurrentColorString = getColorString();
  
   WiFiClient client;
  const int httpPort = 80;
  
    
  memset(msg, 0, 128);
  snprintf(msg, 128, "Yay, someone has pushed the lights to %s. That should keep me going for %d mins. %X", theCurrentColorString.c_str(), boredomTime, timeOfColorChange);
  
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
  else
  {
    Serial.println("ARGH FAILED TO POST TO TWITTER");
  }
}

void postToTwitter()
{
  String theCurrentColorString = getColorString();
  
   WiFiClient client;
  const int httpPort = 80;
  
  const String colors[] = {"red", "green", "blue", "cyan", "purple", "yellow", "orange", "pink", "magenta", "white"};
  int rndCol = random(10);
  while (theCurrentColorString.compareTo(colors[rndCol]) == 0)
  {
    //Serial.print("already is ");
    //Serial.println(colors[rndCol]);
    rndCol = random(10);
  }

  Serial.print("Going to ask for it to be changed to ");Serial.println(colors[rndCol]);
  
  memset(msg, 0, 128);
  snprintf(msg, 128, "#cheerlights, I'm bored at %s in Dub.ie, gimme %s lights. I'll be bored again in %d mins. %X", timeString, colors[rndCol].c_str(), boredomTime, timeOfColorChange);
  
    
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
  else
  {
    Serial.println("ARGH FAILED TO POST TO TWITTER");
  }
}
/*
unsigned long getCurrentTime(char* timeString)
{
    //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(500);

  unsigned long secsSince1900;
  bool estimate = false;
  int cb = udp.parsePacket();
  if (!cb) {
    secsSince1900 = lastFoundSeconds+(DELAYLOOP/1000);
    estimate = true;
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
    secsSince1900 = highWord << 16 | lowWord;
  }
    
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

    sprintf(timeString, "%02d:%02d", h+1, m);

    // print the hour, minute and second:
    if (estimate) Serial.print("*");
    else Serial.print(" ");
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.println(timeString); // print the second
    lastFoundSeconds = secsSince1900;
    return secsSince1900;
  
  return 0;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
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
*/
