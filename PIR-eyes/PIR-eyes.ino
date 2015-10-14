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
#define NUMPIXELS      2

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(1,5, NEO_GRB + NEO_KHZ800);

int delayval =250; // delay for half a second

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
 
  pixels.begin(); // This initializes the NeoPixel library.
  //pixels2.begin();
   pinMode(7, INPUT);
  Serial.begin(19200);
  Serial.println("Hello world");
}


int redval = 0;
int ascend = 0;
int ignoreInputs = 0;
void loop() {
  
  if(!ignoreInputs)
  {
    if (digitalRead(7) == 1)
    {Serial.println(".");
      if (ascend == 0)ascend = 5;
      redval = 1;
      ignoreInputs = 1;
    }
  }

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

    for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(redval,0,0)); 
    pixels.show(); // This sends the updated pixel color to the hardware.

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    if (redval == 255)
    {
      ascend = -5;
      delay(3000);
    }
    else if (redval == 0)
    {
      ascend = 0;
      ignoreInputs = 0;
    }
    redval += ascend;
    if (redval > 255)redval = 255;
    if (redval < 0)redval = 0;
     
    
    }
    delay(30); // Delay for a period of time (in milliseconds).

  }

