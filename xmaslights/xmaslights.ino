#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

/*****************************************************************************
Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

//////
// white : d2
// green : d3
// swtch white d5, gnd, 5vts

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(50, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

void setup() {
    Serial.begin(9600);
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
}


void loop() {
  // Some example procedures showing how to display to the pixels
/*
  colorWipe(Color(255, 0, 0), 5);
  Serial.println("color red\n");  
  delay(5000);
  colorWipe(Color(0, 255, 0), 5);
  Serial.println("color green\n");  
  delay(5000);
  colorWipe(Color(0, 0, 255), 5);
  Serial.println("color blue\n");  
  delay(5000);
  rainbow(5);
  Serial.println("rainbow\n");  
  delay(5000);
  rainbowCycle(5);
  Serial.println("rainbowCycle\n");  
  delay(5000);
  */
  
 /* 
 chase();
 stripSet(0,0);
 delay(5000);
 
 theaterChase(Color(0,255,0),100);
 stripSet(0,0);
 delay(5000);

 theaterChaseRainbow(100);
 stripSet(0,0);
 delay(5000);

 twinkleRand(5,Color(255,255,255),Color(0, 0, 0),190);
 stripSet(0,0);
 delay(5000);
*/
 //twinkleRandColor(3,Color(255,255,255),Color(0, 0, 0),500);
 //stripSet(0,0);
 //delay(5000);

 //simpleWave(0.1,5,10);
 //stripSet(0,0);
 //delay(5000);

//topToBottom();

 fillup();
}


void fillup()
{
  int lastpos = strip.numPixels() -1;
  int x, currentpos = 0;
  stripSet(0,0);
  for(x = 0; x < strip.numPixels(); x++)
  {
    for (int y = 0; y < lastpos; y++)
    {
      strip.setPixelColor(y-1, 0,0,0);
      strip.setPixelColor(y, 30,30,0);
      strip.show();
//      delay(1);
    }
    lastpos -= 1;
  }
}
int shift = 0;
void alternate()
{
  stripSet(0,0);
  for(int x = 0; x < strip.numPixels(); x++)
  {
    if (x+shift%2 == 0)
    {
      strip.setPixelColor(x, 100,0,0);
    }
    else
    {
      strip.setPixelColor(x, 0,100,0);
    }
  }
  shift= shift+1;
  shift = shift % 2;
}


void fill()
{
  int pos = 0;
//  stripSet(0,0);
  for (pos = 0; pos < strip.numPixels(); pos++)
    { 
      strip.setPixelColor(pos, 30,30,80);
      strip.show();
      delay(10);
    }
    for (pos = 0; pos < strip.numPixels(); pos++)
    { 
      strip.setPixelColor(pos, 80,30,30);
      strip.show();
      delay(10);
    }
    for (pos = 0; pos < strip.numPixels(); pos++)
    { 
      strip.setPixelColor(pos, 30, 80,30);
      strip.show();
      delay(10);
    }
}

void topToBottom(){
  int start,finish,pos;
  start = 0; 
  finish = strip.numPixels();
  for (pos = 0; pos < strip.numPixels(); pos++)
  { stripSet(0,0);
    strip.setPixelColor(pos, 255,0,0);
    strip.setPixelColor(pos+1, 0,255,0);
    strip.setPixelColor(pos+2, 0,0,255);
    
    
    strip.setPixelColor(finish-pos, 0,255,0);
    strip.setPixelColor(finish-pos-1, 255,0,0);
    strip.setPixelColor(finish-pos-2, 0,0,255);

    strip.show();
    delay(10);
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

int colorSweep = 0;
void chase(){
  int i,previous ;
  for (i = 0; i < strip.numPixels(); i++)
  {
    previous = i -1;
    if (previous < 0)
    {
      previous = strip.numPixels()-1;
    }
    strip.setPixelColor(previous,Color(0,0,0));
    strip.setPixelColor(i,Wheel(colorSweep%255));
    strip.show();
    delay(1);
  }
  colorSweep += 10;

}
  

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
        delay(wait);
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}
// twinkle random number of pixels
void twinkleRand(int num,uint32_t c,uint32_t bg,int wait) {
	// set background
	 stripSet(bg,0);
	 // for each num
	 for (int i=0; i<num; i++) {
	   strip.setPixelColor(random(strip.numPixels()),c);
	 }
	strip.show();
	delay(wait);
}

// twinkle random number of pixels
void twinkleRandColor(int maxnum,uint32_t c,uint32_t bg,int wait) {
	// set background
	 stripSet(bg,0);
	 // for each num
         int num = random(maxnum) + 1;
	 for (int i=0; i<num; i++) {
           uint32_t col = Color(random(255), random(255), random(255));
	   strip.setPixelColor(random(strip.numPixels()),col);
	 }
	strip.show();
	delay(wait);
}

void simpleWave(float rate,int cycles, int wait) {
   float pos=0.0;
  // cycle through x times
  for(int x=0; x<(strip.numPixels()*cycles); x++)
  {
      pos = pos+rate;
      for(int i=0; i<strip.numPixels(); i++) {
        // sine wave, 3 offset waves make a rainbow!
        float level = (sin(i+pos) * 127) + 128;
        // set color level 
        strip.setPixelColor(i,(int)level, 0, 0);
      }
         strip.show();
         delay(wait);
  }
}  


/**************************************************************/
/* Helper functions */

void stripSet(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  // move the show outside the loop
  strip.show();
  delay(wait);
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
