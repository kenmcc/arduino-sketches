// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 250; // delay for half a second

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
}

// orange = 230, 165, 0
//green = 0, 255, 0
//white = 255,255,255

void fillup(int red, int green, int blue)
{
  int lastpos = NUMPIXELS ;
  int x, currentpos = 0;
  //pixels.clear();
  uint32_t previousColours[NUMPIXELS];
  for(x = 0; x < NUMPIXELS; x++)
  {
    previousColours[x] = pixels.getPixelColor(x);
  }
 for(x = 0; x < NUMPIXELS; x++)
  { 
    for (int y = 0; y < lastpos; y++)
    {
      if (y > 0)
          pixels.setPixelColor(y-1, previousColours[y-1]); // Moderately bright green color.
      pixels.setPixelColor(y, pixels.Color(red, green, blue)); // Moderately bright green color.
      pixels.show();
      delay(80);
    }
    lastpos -= 1;
  }
}

void fill(int r, int g, int b)
{
 for(int x = 0; x < NUMPIXELS; x++)
  { 
    pixels.setPixelColor(x, pixels.Color(r, g, b)); // Moderately bright green color.
    pixels.show();
    delay(80);
  }
}

void sweep()
{
  for (int i = 0; i < NUMPIXELS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(230, 165, 0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.


  }
  delay(delayval); // Delay for a period of time (in milliseconds).
}

int shift = 0;
int colors [3][3] = {{0,255,0},{255,255,255},{230,165,0}};
void alternate(int count)
{
  for (int i = 0; i < count; i++)
  {
    for (int c = 0; c < 3; c++)
    {
      for(int x = 0; x < NUMPIXELS; x++)
      {
        pixels.setPixelColor(x, pixels.Color(colors[c][0], colors[c][1],colors[c][2]));
      }
      
      pixels.show();
      delay(1000);
    }
  }
}



void loop() {
  //sweep();
  
  fillup( 0,255,0);
  fillup(100, 100, 100);
  fillup(230, 165, 0);
  
  alternate(5);
  for (int y = 0; y < 5; y++)
  {
    fill(0,255,0);
    fill(100,100,100);
    fill(230,165,0);
  }
  alternate(5);

}
