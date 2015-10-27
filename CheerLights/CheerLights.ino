

/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif


const char* ssid     = "S3AIR";
const char* password = "LovinglyDelig";

const char* host = "api.thingspeak.com";
const char* streamId   = "/channels/1417/field/2/last.json";

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  pixels.begin(); // This initializes the NeoPixel library.
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
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
  while(client.available() || !begin ){
    char in = client.read();
    if (in == '{') {
        begin = true;
    }

    if (begin) rgbStr += (in);

    if (in == '}') {
        break;
    }

    delay(1);
  }

  
 
  int rgb[3] = {0,0,0};                           //define rgb pointer for ws2812
  rgbStr.replace("%23","#");
  getRGB(rgbStr,rgb);  
    
  Serial.println();
  Serial.println("closing connection");
  
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.

  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(600);
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

int convertToInt(char upper,char lower)
{
  int uVal = (int)upper;
  int lVal = (int)lower;
  uVal = uVal >64 ? uVal - 55 : uVal - 48;
  uVal = uVal << 4;
  lVal = lVal >64 ? lVal - 55 : lVal - 48;
  return uVal + lVal;
}

