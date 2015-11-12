
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include "TweetingCheerLightsPrivate.h"

#include <Time.h>

#if __AVR__
  #include <avr/power.h>
#endif

struct colormap{
  String color;
  int rgbvals[3];
};

colormap myColorMap[] = {
    {"red", {0xFF,0x00,0x00}},
    {"green", {0x00, 0x80, 0x00}},
    {"blue", {0x00, 0x00, 0xFF}},
    {"cyan", {0x00, 0xFF, 0xFF}},
    {"white", {0xFF, 0xFF, 0xFF}},
    {"oldlace",{0xFD, 0xF5, 0xE6}},
    {"warmwhite",{0xFD, 0xF5, 0xE6}},
    {"purple", {0x80, 0x00, 0x80}},
    {"magenta", {0xFF, 0x00, 0xFF}},
    {"yellow", {0xFF, 0xFF, 0x00}},
    {"orange", {0xFF, 0xA5, 0x00}},
    {"pink", {0xFF, 0x70, 0xFF}}
};


#define DEBUG 1

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
char msg[128] = "";

////////////////////// NTP STUFF ////////////////////////////

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
///WiFiUDP udp;

int currentcolors[NUMPIXELS][3] = {{0xFF,0,0}, {0x0,0xFF,0}, {0x00,0,0xFF}};
unsigned long timeOfColorChange = 0;
char timeString[20] = "";
String currentColorString = "red";

unsigned long milliseconds = 0;

unsigned long nowTime = 0;

int boredomTime = 0;
#define BOREDMIN 10
#define BOREDMAX 20

#define DELAYLOOP 30000 // 30 seconds

void software_Reset()
{
  Serial.println("REBOOTING");
  wdt_enable(WDTO_15MS);
  while(1)
  {
    
  }
}

/***************************************** SETUP *******************************/
void setup() 
{
  Serial.begin(115200);
  delay(2);

  // We start by connecting to a WiFi network
#if DEBUG 
 Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  connectToNetwork();

  
  
  
  pixels.begin(); // This initializes the NeoPixel library.
  
 // udp.begin(localPort);

  timeOfColorChange =getCurrentTime(timeString);
  nowTime = timeOfColorChange;

  randomSeed(timeOfColorChange%100);
  boredomTime = random(BOREDMIN,BOREDMAX);
  #if DEBUG 
  Serial.print("Boredom initially set to " );   Serial.println(boredomTime);
  #endif
  getRecentColors();
  
  postWakeUpToTwitter();
  Serial.println("Setup Complete");
}

void connectToNetwork()
{
  WiFi.disconnect();
  delay(500);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
    Serial.print(".");
  }
  Serial.print ("WiFi connected : ");    Serial.print("IP address: ");  Serial.println(WiFi.localIP());
  delay(500);
}

/********************** LOOP ******************************/
void loop() 
{
  nowTime =getCurrentTime(timeString);
 

   /* get the current colours from the server */
  String colorString = askThingSpeakString();
  colorString.toLowerCase();
  if (colorString.compareTo(currentColorString) != 0) // different colour 
  {
    #if DEBUG 
    Serial.print(currentColorString); Serial.print("->"); Serial.println(colorString);
    #endif
      int rgb[3] = {0xFF,0,0};                           //define rgb pointer for ws2812
      currentColorString = colorString;
     getRGBFromColorMap(colorString, rgb);
     // we've got a new set of colours.
      /* move everything up one in the list */
      for (int loop = NUMPIXELS-1; loop >0; loop--)
      {
        for (int p = 0; p < 3; p++)
        {
          currentcolors[loop][p] = currentcolors[loop-1][p];
        }
      }
      /*now add the new stuff */
      for (int p = 0; p < 3; p++)
      {
          currentcolors[0][p] = rgb[p];
      }
      
      timeOfColorChange = nowTime;
      String boredomTimeString = timeFromSecs(nowTime + (boredomTime*60), NULL);
      String dbgString = "Whee new colour " ;
     dbgString += colorString ;
     dbgString += ", but I'll be bored again at ";
     dbgString += boredomTimeString;
     dbgString += " which is in ";
     dbgString +=  boredomTime;
     dbgString += " minutes";
     #if DEBUG 
      Serial.println(dbgString);
      #endif
      updatePixels();
      return; // don't wait, we can go and get the new colour now
  }
  else
  {
    if ((nowTime - timeOfColorChange) > boredomTime * 60)
    {
      boredomTime = random(BOREDMIN,BOREDMAX);
      #if DEBUG 
      Serial.print("BORED, setting to new boredom time of "); Serial.println(boredomTime);
      #endif
      postToTwitter();
    }
  }
  delay(DELAYLOOP);
}

void updatePixels()
{
        for(int i=0;i<NUMPIXELS;i++)
      {
        String ColorString  = String(i);
       ColorString += ":";
       ColorString +=currentcolors[i][0];
       ColorString +=currentcolors[i][1];
       ColorString +=currentcolors[i][2];
        //Serial.println(ColorString);
        pixels.setPixelColor(i, pixels.Color(currentcolors[i][0], currentcolors[i][1], currentcolors[i][2])); // Moderately bright green color.
     }
      pixels.show(); // This sends the updated pixel color to the hardware.
}


String askThingSpeakString()
{
  WiFiClient thingspeakClient;
  const int httpPort = 80;
  // Use WiFiClient class to create TCP connections
  
  #if DEBUG 
  Serial.println("Going to ask thingspeak");
  #endif
  if(!thingspeakClient.connected())
  {
#if DEBUG 
       Serial.println("Connecting to thingspeak");
#endif
      if (!thingspeakClient.connect(host, httpPort)) {
#if DEBUG         
        Serial.print("connection to "); Serial.print(host); Serial.println("  failed");
#endif
        if (WiFi.status() != WL_CONNECTED)
        {
#if DEBUG           
          Serial.print("WifiStatus is ");Serial.println(WiFi.status());
#endif          
        }
        software_Reset();
          return currentColorString;
      }
  }
  else
  {
#if DEBUG 
    Serial.println("It's connected");
#endif
  }
  thingspeakClient.flush();
  // We now create a URI for the request
  String url = textStreamId;
  
  
  // This will send the request to the server
  thingspeakClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  String rgbStr = "";
  bool begin = false;
  // Read all the lines of the reply from server and print them to Serial
  while(thingspeakClient.available() || !begin )
  {
    char in = thingspeakClient.read();
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
 #if DEBUG 
  Serial.print("Current Color = "); Serial.println(getColorString(rgbStr));
#endif  
  return getColorString(rgbStr);
}

void getRGBFromColorMap(String color, int* rgb)
{
  color.toLowerCase();
  int entries = sizeof(myColorMap)/sizeof(myColorMap[0]);
  int x = 0;
  for (x = 0; x < entries; x++)
  {
    if (color.compareTo(myColorMap[x].color) == 0)
    {
      rgb[0] = myColorMap[x].rgbvals[0];
      rgb[1] = myColorMap[x].rgbvals[1];
      rgb[2] = myColorMap[x].rgbvals[2];
      break;
    }
  }
  
}

String getColorString(String input)
{
  input.toUpperCase();
  int s =  input.indexOf("FIELD1") + 9;
  String color = input.substring(s);
  int end = color.indexOf("\"");
  color = color.substring(0, end);
   color.toLowerCase();
   return color;
}

void postToTwitter()
{
  String currentColorStringRaw = askThingSpeakString();
  String theCurrentColorString = getColorString(currentColorStringRaw);
  
   WiFiClient client;
  const int httpPort = 80;
  
  const String colors[] = {"red", "green", "blue", "cyan", "purple", "yellow", "orange", "pink", "magenta"};
  int rndCol = random(sizeof(colors)/sizeof(colors[0]));
  while (theCurrentColorString.compareTo(colors[rndCol]) == 0)
  {
#if DEBUG     
    Serial.print("already is "); Serial.println(colors[rndCol]);
#endif    
    rndCol = random(sizeof(colors)/sizeof(colors[0]));
  }
  
  memset(msg, 0, 128);
  snprintf(msg, 128, "@cheerlights, I'm a bored ESP8266 and I want the %s lights. I'll be bored again soon so keep me interested", colors[rndCol].c_str());
  
#if DEBUG     
  Serial.print("Trying to twat "); 
  Serial.println(msg);
#endif

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

void postWakeUpToTwitter()
{
   WiFiClient client;
  const int httpPort = 80;
  
  nowTime = getCurrentTime(timeString);
  
  memset(msg, 0, 128);
  snprintf(msg, 128, "Howdy, I'm a ESP8266 low on patience, waking up at %s and the lights are apparently %s. I'll be bored in %d minutes", timeString, currentColorString.c_str(), boredomTime);

#if DEBUG   
  Serial.print("Trying to twat wakeup "); 
  Serial.println(msg);
#endif

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
static int timerLoop = 0;
static int firstTime = 1;
unsigned long getCurrentTime(char* timeString)
{
  
  unsigned long thisMillis = millis();
  unsigned long secondsDelta = (thisMillis - milliseconds)/1000;
  milliseconds = thisMillis;
  
  //get a random server from the pool
  //WiFi.hostByName(ntpServerName, timeServerIP); 

 //   sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(2000);
  
  int cb = 0;
  if (timerLoop%10 == 0)
  {
    //cb = udp.parsePacket();
    if (firstTime == 0)
    {
      timerLoop++;
    }
  }
  if (!cb) {
    if (timerLoop%10 == 0)
    {
#if DEBUG 
      Serial.print("no packet yet, returning"); Serial.println(nowTime + secondsDelta);
#endif
    }
    if (WiFi.status() != WL_CONNECTED)
    {
#if DEBUG     
      Serial.print("WifiStatus is ");Serial.println(WiFi.status());
#endif
    }
    return nowTime + secondsDelta;
  }
  else {
    /*
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    
    timeFromSecs(secsSince1900, timeString);
    firstTime = 0;
#if DEBUG     
    Serial.print("Returning time "); Serial.println(nowTime + secondsDelta);
#endif    
    return secsSince1900;
    */
  }
  return 0;
}

String timeFromSecs(unsigned long secs, char* timeStr)
{
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secs - seventyYears;
    
    int h, m, s;
    h = (epoch  % 86400L) / 3600;
    m = (epoch  % 3600) / 60;
    s = (epoch%60);

    if (timeStr != NULL)
    {
      sprintf(timeString, "%02d:%02d:%02d", h, m, s);
    }
    char localTimer[10] = "";
    snprintf(localTimer, 10, "%02d:%02d:%02d", h, m, s);
    String timer = String(localTimer);
    return timer;
    
}
/*
// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
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
void getRecentColors(void)
{
WiFiClient thingspeakClient;
  const int httpPort = 80;
  int connected = 0;
  
   if(!thingspeakClient.connected())
  {
#if DEBUG     
    Serial.println("Connecting to thingspeak to get recent colors");
#endif    
      if (!thingspeakClient.connect(host, httpPort)) {
#if DEBUG         
        Serial.print("connection to "); Serial.print(host); Serial.println("  failed");
#endif        
        if (WiFi.status() != WL_CONNECTED)
        {
#if DEBUG           
          Serial.print("WifiStatus is ");Serial.println(WiFi.status());
#endif          
        }
        software_Reset();
          return;
      }
  }
    thingspeakClient.flush();
  // We now create a URI for the request
  String url = "/channels/1417/feed.json";
  
  // This will send the request to the server
  thingspeakClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  String all = "";
  bool begin = false;
  // Read all the lines of the reply from server and print them to Serial
  while(thingspeakClient.available() || !begin )
  {
    char in = thingspeakClient.read();
    if (in == '[') 
    {
        begin = true;
    }
    if (begin) all += (in);
    if (in == ']') 
    {
        break;
    }
   delay(1);
  }
 
 /* at this stage we have the last number of colours . we want to find the last N elements of FIELD2 
 and start filling in an array of previous colours. for some reason the strings are doubled so go back 2 every time....*/
  
  all.toUpperCase();

#if DEBUG 
Serial.println("History says: ");
#endif
  for (int x = 0; x < NUMPIXELS; x++)
 {
   int lastEntryPos = all.lastIndexOf("{");
   String lastEntry = all.substring(lastEntryPos);
   
   int lastFieldPos = lastEntry.lastIndexOf("FIELD1") + 9;
   String color = lastEntry.substring(lastFieldPos);
   int endOfColorPos = color.indexOf("\"");
   color = color.substring(0, endOfColorPos);
#if DEBUG    
   Serial.println(color);
#endif   
   getRGBFromColorMap(color, currentcolors[x]);
   
   /* now we remove the last 2 entries */
   all.remove(lastEntryPos);
   lastEntryPos = all.lastIndexOf("{");
   all.remove(lastEntryPos);
   
   if (x == 0)
   {
     color.toLowerCase();
     currentColorString = color;
   }
 } 
 
 
 updatePixels();
 
 
  //Serial.println (all);
}


