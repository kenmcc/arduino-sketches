// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <avr/sleep.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 50; // delay for half a second
int loopsAround = 5;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.
randomSeed(analogRead(0));
loopsAround = random(4,10)*NUMPIXELS;

  }
}

int loopcounter = 0;
void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  if (loopcounter <loopsAround)
  {
   for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
   }
   pixels.setPixelColor(loopcounter%NUMPIXELS, pixels.Color(10,10,0));
    pixels.show(); // This sends the updated pixel color to the hardware.

  } 
  else if (loopcounter < loopsAround *2)
  {
    for(int i=0;i<NUMPIXELS;i++)
    {
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
    }
    int offset = loopcounter%NUMPIXELS;
    pixels.setPixelColor(offset, pixels.Color(0,10,10));
    pixels.setPixelColor(NUMPIXELS-offset, pixels.Color(0,10,10));
    pixels.show(); // This sends the updated pixel color to the hardware.
 
  }
  else{
    
  
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,5,0)); // Moderately bright green color.


  
  }
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  loopcounter += 1;
  delay(delayval); // Delay for a period of time (in milliseconds).

 if (loopcounter > loopsAround * 5)
 {
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
  
  }
    pixels.show(); // This sends the updated pixel color to the hardware.
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
 }
 
}
