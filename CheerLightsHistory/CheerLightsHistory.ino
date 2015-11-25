
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
    {"orange", {0xFF, 0xA5, 0x00}},
    {"blue", {0x00, 0x00, 0xFF}},
    {"purple", {0x80, 0x00, 0x80}},
    {"white", {0xFF, 0xFF, 0xFF}},
    {"magenta", {0xFF, 0x00, 0xFF}},
    {"cyan", {0x00, 0xFF, 0xFF}},
    {"yellow", {0xFF, 0xFF, 0x00}},
    {"pink", {0xFF, 0x70, 0xFF}}, 
    {"oldlace",{0xFD, 0xF5, 0xE6}},
     {"warmwhite",{0xFD, 0xF5, 0xE6}},
};


#define DEBUG 1
#define MULTICOLORS 0

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


int currentcolors[NUMPIXELS][3] = {{0xFF,0,0}, {0x0,0xFF,0}, {0x00,0,0xFF}};
unsigned long timeOfColorChange = 0;
char timeString[20] = "";
String currentColorString = "red";


#define DELAYLOOP 1000 // 30 seconds
static unsigned int loopCount = 0;
#if MULTICOLORS
#define ASK_FREQUENCY  (60/(DELAYLOOP/1000)) //ask every minute
#else
#define ASK_FREQUENCY  (20/(DELAYLOOP/1000)) //ask every minute
#endif
void software_Reset()
{
  /*Serial.println("REBOOTING");
  wdt_enable(WDTO_15MS);
  while(1)
  {
    
  }*/
}

/***************************************** SETUP *******************************/
void setup() 
{
  //wdt_disable();
  Serial.begin(115200);
  delay(2);

 
  pixels.begin(); // This initializes the NeoPixel library.
  
 // udp.begin(localPort);

  getRecentColors();
  
   //wdt_enable(WDTO_8S);
  Serial.println("Setup Complete");
}

void startWIFI(void) {
    // Connect to WiFi network
    Serial.println();
    delay(10);
    Serial.println();
    delay(10);
    for (int loop = 0; loop < 2; loop++)
    {
          Serial.print("Connecting to ");
          delay(10);
          Serial.println(ssid[loop]);
          delay(10);

        unsigned long start = millis();
        WiFi.begin(ssid[loop], password[loop]);
  
      while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(500);
          unsigned long now = millis();
          if ((now - start) > 20000)
          {
            Serial.println("Trying a different ssid");
            break;
          }
      }
      
      if(WiFi.status() == WL_CONNECTED) 
      {     
              Serial.println("");
              Serial.println("WiFi connected");
              break;
      }
    }


    // Print the IP address
    Serial.print("ESP8266 IP: ");
    Serial.println(WiFi.localIP());

     delay(300);
}



static int rotationStart = 0;
/********************** LOOP ******************************/
void loop() 
{
  //connect wifi if not connected
if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    startWIFI();
    return;
}
  
  //wdt_reset();
  loopCount++;
   /* get the current colours from the server */
  if (loopCount % ASK_FREQUENCY == 0)
  {
    
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
#if MULTICOLORS
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
#else
        for (int loop = 0; loop < NUMPIXELS; loop++)
        {
          for (int p = 0; p < 3; p++)
          {
            currentcolors[loop][p] = rgb[p];
          }
        }
#endif
        
  
         String dbgString = "Whee new colour " ;
       dbgString += colorString ;
       #if DEBUG 
        Serial.println(dbgString);
        #endif
        rotationStart = 0;
        updatePixels(0);
    }
  }
  #if MULTICOLORS
  else
  {
     #if DEBUG 
     if(rotationStart == 0)
     {
       Serial.println("SPIN");
     }
    #endif
    rotationStart++;
    if (rotationStart == NUMPIXELS)
    {
      rotationStart = 0;
    }
    updatePixels(rotationStart);
  }
  #endif
  delay(DELAYLOOP);
}

void updatePixels(int rotationStart)
{
        for(int i=0;i<NUMPIXELS;i++)
      {
        String ColorString  = String(i);
       ColorString += ":";
       ColorString +=currentcolors[i][0];
       ColorString +=currentcolors[i][1];
       ColorString +=currentcolors[i][2];
        //Serial.println(ColorString);
        pixels.setPixelColor((i+rotationStart)%NUMPIXELS, pixels.Color(currentcolors[i][0], currentcolors[i][1], currentcolors[i][2])); // Moderately bright green color.
#if MULTICOLORS
#else
       pixels.show();
       delay(100);
#endif
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



void getRecentColors(void)
{
  for (int x = 0; x < NUMPIXELS; x++)
 {
  #if MULTICOLORS 
      String color= myColorMap[x].color;
  #else
      String color= myColorMap[0].color;
  #endif
#if DEBUG    
   Serial.println(color);
#endif   
   getRGBFromColorMap(color, currentcolors[x]);
     
   if (x == 0)
   {
     color.toLowerCase();
     currentColorString = color;
   }
 } 
 
 
 updatePixels(0);
 
}

void reallygetRecentColors(void)
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
 
 
 updatePixels(0);
 
 
  //Serial.println (all);
}


