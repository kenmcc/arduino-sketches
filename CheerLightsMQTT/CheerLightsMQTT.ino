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
#include <PubSubClient.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "private.h"

const char* mqtt_server = "mqtt.thingspeak.com";

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            4 //D2 on D1Mini

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;
char topicString[90] = "channels/1417/subscribe/fields/+/";




/////// setup ////////
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  for(int i=0;i<NUMPIXELS;i++)
  {
     pixels.setPixelColor(i, pixels.Color(255, 255, 255)); // RED.
  }
  pixels.show();
 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  strncat(topicString, APIKEY, strlen(APIKEY));

  first_connect();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
}

void callback(char* topic, byte* payload, unsigned int length) 
{
/*  Serial.print("Message arrived [");
     Serial.print(length);
     Serial.print("] ");
     for (int i = 0; i < length; i++) 
     {
       Serial.print((char)payload[i]);
     }
     Serial.println();

*/
  if(length == 7 && payload[0] == '#')
  {
     Serial.print("Message arrived [");
     Serial.print(length);
     Serial.print("] ");
     for (int i = 0; i < length; i++) 
     {
       Serial.print((char)payload[i]);
     }
     Serial.println();

     Serial.print("Converting to color");
     int rgb[3] = {0,0,0};   
     getRGB(payload, rgb);
     Serial.print(rgb[0]); Serial.print(" ");
     Serial.print(rgb[1]); Serial.print(" ");
     Serial.print(rgb[2]); Serial.println(" ");

     for(int i=0;i<NUMPIXELS;i++)
     {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.
     }
     pixels.show();  
  }
}

void getRGB(byte* c, int *rgb) {
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

boolean reconnect() {

  if (client.connect("reincheer",username, APIKEY)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(topicString);
    }
  return client.connected();
}

void first_connect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("reincheer",username, APIKEY)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(topicString);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
   if (!client.connected()) 
   {
      long now = millis();
      if (now - lastReconnectAttempt > 5000) 
      {
        lastReconnectAttempt = now;
        // Attempt to reconnect
        if (reconnect()) 
        {
          lastReconnectAttempt = 0;
        }
      }
   } 
   else 
   {
      // Client connected
      client.loop();
   }
}

